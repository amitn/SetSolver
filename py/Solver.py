#!/usr/bin/python

import numpy
import itertools
import curses


NUM_FEATURES = 3
NUM_DIMS = 4
NUM_CARDS = 12


def random_deck():
    # initialize cards deck
    cards = numpy.array([card for card in
                         itertools.product(range(NUM_FEATURES),
                                           repeat=NUM_DIMS)]).T
    n = cards.shape[1]
    print "Deck Created, %d Card in the deck" % (n)
    # shuffle
    return cards[:, numpy.random.permutation(n)]


def printCard(stdscr, x, y, card):
    pass


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


def find_sets(cards):
    """Brute-force Sets solver."""
    return [indices
            for indices in itertools.combinations(range(cards.shape[1]), 3)
            if is_set(cards, indices)]

if (__name__ == "__main__"):
    cards = random_deck()
    print cards.shape
    #print cards
    #print cards.shape[1]
    play_cards = cards[:, range(NUM_CARDS)]
    all_cards = cards[:, NUM_CARDS:]
    while True:
        print "  0 1 2 3 4 5 6 7 8 9 0 1"
        print play_cards
        sets = find_sets(play_cards)
        nfound = len(sets)
        if (nfound > 0):
            sel = numpy.random.randint(len(sets), size=1)
            print "Found %d Sets, select set %d %s" % (nfound, sel, sets[sel])
            play_cards = numpy.delete(play_cards, sets[0], axis=1)
        if (all_cards.shape[1] > 0):
            play_cards = numpy.hstack((play_cards, all_cards[:, range(3)]))
            all_cards = all_cards[:, 3:]
        elif (nfound < 1):
            break

    print cards.shape
