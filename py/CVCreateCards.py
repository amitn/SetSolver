#!/usr/bin/python

import cv2
import json


NUM = 0
START_NUM = 34
DATABASE_PATH = './DataBase'
MY_CARDS = {}
MY_FEATURES = {}
CARD_NAME = ""
SAVE_IMG = False


def GetCardsDataBase():
    FILE_NAME = DATABASE_PATH + '/CardsData.json'
    f = open(FILE_NAME)
    cardData = json.load(f)
    f.close()
    return cardData


def UserInput():
    global CARD_NAME
    global MY_CARDS
    for f, v in MY_FEATURES.iteritems():
        print "%s %s" % (f, v)
        k = -1
        while k not in range(3):
            k = cv2.waitKey(0) - 49
        MY_CARDS[CARD_NAME][f] = v[k]


def SaveJson():
    global MY_CARDS
    with open('data.txt', 'w') as outfile:
        #s = json.dumps(MY_CARDS, sort_keys=True, indent=4)
        #print s
        json.dump(MY_CARDS, outfile, sort_keys=True, indent=4)


def SplitCards(img):
    global NUM
    global START_NUM
    global CARD_NAME
    global MY_CARDS
    global SAVE_IMG
    img_crop = img[605:2105, 60:1830]
    img_crop = cv2.resize(img_crop, (0, 0), fx=0.5, fy=0.5)

    #cv2.imshow("crop", img_crop)

    H = img_crop.shape[0] / 4
    W = img_crop.shape[1] / 3
    for i in range(3):
        for j in range(4):
            D = 5
            icard = img_crop[((j * H) + D):(((j + 1) * H) - D),
                             ((i * W) + D):(((i + 1) * W) - D)]
            cv2.imshow(str(NUM), icard)
            cv2.moveWindow(str(NUM), 200, 600)
            CARD_NAME = 'card%03d' % (NUM)
            if (SAVE_IMG):
                cv2.imwrite("./DataBase/%s.jpg" % (CARD_NAME), icard)
            if (NUM >= START_NUM):
                MY_CARDS[CARD_NAME] = {}
                UserInput()
            NUM += 1
            print "Next..."
            cv2.destroyAllWindows()


if (__name__ == "__main__"):
    cardData = GetCardsDataBase()
    MY_FEATURES = cardData['features']
    try:
        for i in range(1, 26):
            img = cv2.imread("./Docs/MatlabSolver/IMG%02d.jpg" % (i))
            SplitCards(img)
    finally:
        SaveJson()
