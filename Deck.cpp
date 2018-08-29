//
//  Deck.cpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#include <stdlib.h>

#include "Deck.hpp"

#define NUM_VALUES 13
#define NUM_SUITS 4

Deck::Deck() {
    initialize();
}

void Deck::initialize() {
    for (int i = 0; i < NUM_VALUES; i++) {
        for (int j = 0; j < NUM_SUITS; j++) {
            Card c(i, j);
            deck.push_back(c);
        }
    }
}

Card Deck::deal() {
    int index = rand() % deck.size();
    Card c = deck.at(index);
    deck.erase(deck.begin() + index);
    return c;
}

