#!/usr/bin/python

import cv2
import json
import numpy
import sys
import itertools

DATABASE_PATH = './DataBase'

FEATURES = {"symbol": ["oval", "squiggle", "diamond"],
            "color": ["red", "green", "purple"],
            "amount": [1, 2, 3],
            "shading": ["solid", "open", "striped"]
            }

COLOR_LIST = [(0, 0, 255), (0, 255, 0), (255, 0, 0),
              (0, 255, 255), (255, 255, 0), (255, 0, 255)]

STYPE = {"oval": None,
         "squiggle": None,
         "diamond": None}
h = numpy.zeros((300, 256, 3))


class Card(object):
    def __init__(self, cardInfo=None, I=None, debug=False):
        self.cardInfo = cardInfo
        self._thresh = 115
        self._debug = debug
        self.myInfo = {}
        self.I = I
        self.Prosses()

    def Prosses(self):
        self.LoadImage()
        self.NormColors()
        self.DetectAmount()
        self.CutMasks()
        self.FindShading()
        self.FindColor()
        self.FindShape()
        pass

    def GetFeatures(self):
        global FEATURES
        r = []
        for f, v in self.myInfo.iteritems():
            r.append(FEATURES[f].index(v))
        return r

    def PrintFeature(self, f):
        if (self._debug):
            if (self.cardInfo[f] == self.myInfo[f]):
                print 'We found the right %s: %s' % (f, self.myInfo[f])
            else:
                print 'ERROR: Card %s is %s, We found %s' % (f, self.cardInfo[f], self.myInfo[f])

    def NormColors(self):
        In = self.I.astype(float)
        II = In ** 2
        II = numpy.sum(II, axis=2)
        II = numpy.sqrt(II)
        II = II[..., numpy.newaxis]
        II = numpy.ma.concatenate((II, II, II), axis=2)
        C = numpy.divide(self.I, II)
        C = cv2.normalize(C, alpha=0, beta=255,
                          norm_type=cv2.NORM_MINMAX)
        self.IC = numpy.uint8(C)
        #cv2.imshow('NewColors', self.IC)

    def DetectAmount(self):
        GRAY = cv2.cvtColor(self.I, cv2.COLOR_BGR2GRAY)
        #cv2.imshow('GRAY', GRAY)
        BW = cv2.threshold(GRAY, self._thresh, 255, cv2.THRESH_BINARY)[1]
        self.BW = numpy.invert(BW)
        #cv2.imshow('BW', self.BW)
        BWt = self.BW.copy()
        cntrs, hircy = cv2.findContours(BWt,
                                        cv2.RETR_EXTERNAL,    # cv2.RETR_TREE,
                                        cv2.CHAIN_APPROX_SIMPLE)
        F = numpy.zeros(self.BW.shape, dtype=numpy.uint8)
        self.MASK = numpy.zeros(self.BW.shape, dtype=numpy.uint8)
        areas = [cv2.contourArea(cnt) for cnt in cntrs]
        t = numpy.mean(filter(lambda x: x > 50, areas))
        t = t * 0.9
        self.MASKrect = None
        self.MASKcnt = None
        wasMasked = False
        for idx, cnt in enumerate(cntrs):
            if (areas[idx] > t):
                # thickness -1 will fill the conntours
                cv2.drawContours(F, [cnt], 0, (255), thickness=-1)
                if (not wasMasked):
                    wasMasked = True
                    cv2.drawContours(self.MASK, [cnt], 0, (255), thickness=-1)
                    self.MASKcnt = cnt
                    self.MASKrect = cv2.boundingRect(cnt)

        #cv2.imshow('Count', F)
        #cv2.imshow('Mask', self.MASK)
        cntrs, hircy = cv2.findContours(F,
                                        cv2.RETR_EXTERNAL,
                                        cv2.CHAIN_APPROX_SIMPLE)

        amount = len(cntrs)
        self.myInfo['amount'] = amount
        self.PrintFeature('amount')

    def CutMasks(self):
        rect = [0, 0, 0, 0]
        rect[0] = self.MASKrect[0] - 5
        rect[1] = self.MASKrect[1] - 5
        rect[2] = self.MASKrect[0] + self.MASKrect[2] + 5
        rect[3] = self.MASKrect[1] + self.MASKrect[3] + 5
        self.CutI = self.I[rect[1]:rect[3], rect[0]:rect[2]]
        self.CutBW = self.BW[rect[1]:rect[3], rect[0]:rect[2]]
        self.CutM = self.MASK[rect[1]:rect[3], rect[0]:rect[2]]
        #cv2.imshow('CI', self.CutI)
        #cv2.imshow('CBW', self.CutBW)
        #cv2.imshow('CM', self.CutM)

    def FindShading(self):
        S = cv2.bitwise_and(self.CutBW, self.CutM)
        E = cv2.Canny(S, 90, 200, apertureSize=3)
        #cv2.imshow('Edge', E)
        nE = numpy.count_nonzero(E)
        nM = numpy.count_nonzero(self.CutM)
        dEM = float(nE) / float(nM)
        #print "Mask %d, Edge %d, div %f" % (nM, nE, dEM)
        shading = ""
        if (dEM < 0.08):
            shading = 'solid'
        elif (dEM > 0.17):
            shading = 'striped'
        else:
            shading = 'open'
        self.myInfo['shading'] = shading
        self.PrintFeature('shading')

    def FindColor(self):
        S = cv2.bitwise_and(self.CutBW, self.CutM)
        S = cv2.bitwise_and(self.CutI, self.CutI, mask=S)
        #cv2.imshow('VS', S)
        #ShowHist(S, self.cardInfo['color'])
        HSV = cv2.cvtColor(S, cv2.COLOR_BGR2HSV)
        H = HSV[:, :, 0]
        nG = numpy.count_nonzero(cv2.inRange(H, 25, 90))
        nP = numpy.count_nonzero(cv2.inRange(H, 140, 170))
        nR = numpy.count_nonzero(cv2.inRange(H, 170, 255))
        C = ['red', 'green', 'purple']
        nC = [nR, nG, nP]
        i = numpy.argmax(nC)
        self.myInfo['color'] = C[i]
        self.PrintFeature('color')

    def FindShape(self):
        cnt = self.MASKcnt
        mmnts = cv2.moments(cnt)
        hu = cv2.HuMoments(mmnts)
        #print cv2.contourArea(cnt)
        symbol = ""
        if (hu[0] < 0.207):
            symbol = 'oval'
        elif (hu[0] > 0.23):
            symbol = 'squiggle'
        else:
            symbol = 'diamond'

        self.myInfo['symbol'] = symbol
        self.PrintFeature('symbol')
        if (self._debug):
            global STYPE
            ksymbol = self.cardInfo['symbol']
            if STYPE[ksymbol] is None:
                STYPE[ksymbol] = hu
            else:
                STYPE[ksymbol] = numpy.append(STYPE[ksymbol], hu, axis=1)

    def LoadImage(self):
        if not (self.cardInfo is None):
            cardName = self.cardInfo['name']
            self.I = cv2.imread(DATABASE_PATH + '/' + cardName + '.jpg')
        #cv2.imshow('Original', self.I)


def SolveSet(cards):
    def same(x):
        """Returns True if all elements are the same."""
        return numpy.all(x == x[0])

    def different(x):
        """Returns True if all elements are different."""
        return len(numpy.unique(x)) == len(x)

    def is_set(cards, indices):
        """Checks that the cards indexed by 'indices' form a valid set."""
        ndims = cards.shape[0]
        subset = cards[:, indices]
        for dim in range(ndims):
            if not same(subset[dim, :]) and not different(subset[dim, :]):
                return False
        return True

    return [indices
            for indices in itertools.combinations(range(cards.shape[1]), 3)
            if is_set(cards, indices)]


class PlayGame(object):
    def __init__(self, I):
        self.I = I
        self.cards = []
        self.cardsFtrs = None
        self.cardsCuts = None
        self._isFirst = True
        self.Prosses()

    def Prosses(self):
        self.SplitCards()
        self.SolveGame()
        self.DisplaySolution()

    def SolveGame(self):
        self.sets = SolveSet(self.cardsFtrs)

    def SaveImage(self, name):
        cv2.imwrite(name, self.Is)

    def DisplaySolution(self):
        global COLOR_LIST
        Is = self.Ic.copy()
        for j, s in enumerate(self.sets):
            for i in s:
                cut = self.cardsCuts[:, i]
                cv2.rectangle(Is, (cut[2]+(j*3), cut[0]+(j*3)),
                              (cut[3]-(j*3), cut[1]-(j*3)), COLOR_LIST[j % 6],
                              thickness=2)
                cv2.putText(Is, str(j), (cut[2]+j*50, cut[0]+50),
                        cv2.FONT_HERSHEY_PLAIN, 2.0, COLOR_LIST[j%6],
                        thickness=2)

        cv2.imshow('Solution', Is)
        self.Is = Is
        k = cv2.waitKey(0)
        cv2.destroyAllWindows()

    def SplitCards(self):
        Ic = self.I[605:2105, 60:1830]
        Ic = cv2.resize(Ic, (0, 0), fx=0.5, fy=0.5)
        self.Ic = Ic
        #cv2.imshow("crop", img_crop)

        H = Ic.shape[0] / 4
        W = Ic.shape[1] / 3
        for i in range(3):
            for j in range(4):
                D = 5
                cuts = [((j * H) + D), (((j + 1) * H) - D),
                        ((i * W) + D), (((i + 1) * W) - D)]
                Icard = Ic[cuts[0]:cuts[1], cuts[2]:cuts[3]]
                card = Card(cardInfo=None, I=Icard, debug=False)
                self.cards.append(card)
                F = numpy.array([card.GetFeatures()]).T
                C = numpy.array([cuts]).T
                if (self._isFirst):
                    self.cardsFtrs = F
                    self.cardsCuts = C
                    self._isFirst = False
                else:
                    self.cardsFtrs = numpy.hstack([self.cardsFtrs, F])
                    self.cardsCuts = numpy.hstack([self.cardsCuts, C])


def ShowHist(I, cc):
    #h = numpy.zeros((300, 256, 3))
    global h
    bins = numpy.arange(256).reshape(256, 1)
    color = [(255, 0, 0), (0, 255, 0), (0, 0, 255)]

    HSV = cv2.cvtColor(I, cv2.COLOR_BGR2HSV)
    I = HSV
    for ch, col in enumerate(color):
        hist_item = cv2.calcHist([I], [ch], None, [256], [30, 90])
        cv2.normalize(hist_item, hist_item, 0, 255, cv2.NORM_MINMAX)
        hist = numpy.int32(numpy.around(hist_item))
        pts = numpy.column_stack((bins, hist))
        #cv2.polylines(h, [pts], False, col)
        if (ch == 0):
            if (cc == 'red'):
                c = color[2]
            elif (cc == 'green'):
                c = color[1]
            else:
                c = color[0]
            cv2.polylines(h, [pts], False, c)
            break
    R = numpy.rot90(h)
    #cv2.imshow('hist', R)


def NextCard():
    k = cv2.waitKey(0)
    cv2.destroyAllWindows()
    if (k == 27):
        sys.exit()


def GetCardsDataBase():
    FILE_NAME = DATABASE_PATH + '/CardsData.json'
    f = open(FILE_NAME)
    cardData = json.load(f)
    f.close()
    return cardData


def CehckSTYPE():
    global STYPE
    print STYPE
    for ty in STYPE.iterkeys():
        print ty
        mean = numpy.mean(STYPE[ty], axis=1)
        print "Mean: %s" % (mean)
        std = numpy.std(STYPE[ty], axis=1)
        print "Std: %s" % (std)
        var = numpy.var(STYPE[ty], axis=1)
        print "Var: %s" % (var)


def CheckCards():
    cardsData = GetCardsDataBase()
    for cardName in cardsData['cards'].iterkeys():
        cardInfo = cardsData['cards'][cardName]
        cardInfo["name"] = cardName
        card = Card(cardInfo=cardInfo, I=None, debug=True)
        NextCard()
        #cv2.destroyAllWindows()
    #CehckSTYPE()


def ProssesGame():
    for i in range(1, 6):
        img = cv2.imread("./Images/IMG%02d.jpg" % (i))
        g = PlayGame(img)
        g.SaveImage("./Sol/Sol%02d.jpg" % (i))

if __name__ == "__main__":
    #CheckCards()
    ProssesGame()
