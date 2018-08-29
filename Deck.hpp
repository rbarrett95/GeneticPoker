//
//  Deck.hpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#ifndef Deck_hpp
#define Deck_hpp

#include <stdio.h>
#include <vector>

#include "Card.hpp"

using namespace std;

class Deck {
private:
    vector<Card> deck;
public:
    Deck();
    void initialize();
    Card deal();
};

#endif /* Deck_hpp */
