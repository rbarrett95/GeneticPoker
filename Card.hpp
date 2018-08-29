//
//  Card.hpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#ifndef Card_hpp
#define Card_hpp

#include <stdio.h>

class Card {
private:
    short value;
    short suit;
public:
    Card(short v, short s): value(v), suit(s) { }
    Card(): value(-1), suit(-1) { }
    int numericValue() { return 4 * value + suit; }
};

#endif /* Card_hpp */
