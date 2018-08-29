//
//  Player.cpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#include <utility>
#include <stdlib.h>
#include <iostream>

#include "Player.hpp"
#include "Card.hpp"

using namespace std;

Player::Player(double stack, short seat) {
    this->stack = stack;
    bankRoll = 300000.0;
    allIn = false;
    this->seat = seat;
}

double Player::bet(double amount) {
    if (amount >= stack) {
        allIn = true;
        double returnVal = stack;
        stack = 0;
        return returnVal;
    }
    stack -= amount;
    return amount;
}

double Player::decision(double amount, double pot, vector<Card> board, vector<Round> previousAction) {
    if (amount == 0.0) {
        return 0.0;
    }
    
    return bet(amount);
}

void Player::resetStack(double startingStack) {
    double diff = startingStack - stack;
    stack += diff;
    bankRoll -= diff;
}
