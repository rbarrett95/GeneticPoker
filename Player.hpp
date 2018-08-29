//
//  Player.hpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#ifndef Player_hpp
#define Player_hpp

#include <stdio.h>
#include <utility>
#include <vector>

#include "Card.hpp"
#include "Round.hpp"

using namespace std;

class Player {

private:
    Card card1;
    Card card2;
    
    short seat;
    int handStrength;
    double stack;
    double bankRoll;
    bool inHand;
    bool allIn;
    
    vector<double> strategy;
    

public:
    Player(double stack, short seat);
    double getStack() { return stack; }
    void setStack(double stack) { this->stack = stack; }
    bool isInHand() { return inHand; }
    bool isAllIn() { return allIn; }
    void setAllIn(bool allIn) { this->allIn = allIn; }
    void setInHand(bool inHand) { this->inHand = inHand; }
    double bet(double amount);
    pair<Card, Card> getHand() { return make_pair(card1, card2); }
    void setHand(Card card1, Card card2) { this->card1 = card1; this->card2 = card2; }
    void addToStack(double amount) { stack += amount; }
    double decision(double amount, double pot, vector<Card> board, vector<Round> previousAction);
    int getHandStrength() { return handStrength; }
    void setHandStrength(int handStrength) { this->handStrength = handStrength; }
    void resetStack(double startingStack);
    double getBankroll() { return bankRoll; }
    
    
    // included so Player can be used as a key in an STL map
    bool operator< (const Player& playerObj) const {
        if(playerObj.seat < this->seat) {
            return true;
        }
        return false;
    }
};

#endif /* Player_hpp */
