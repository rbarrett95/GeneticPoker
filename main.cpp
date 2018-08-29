//
//  main.cpp
//  pokerGTO
//
//  Created by Ryan Barrett on 8/20/18.
//  Copyright © 2018 Ryan Barrett. All rights reserved.
//

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

#include "Player.hpp"
#include "Deck.hpp"
#include "Hand.hpp"
#include "HandEvaluator.hpp"
#include "Round.hpp"

#define NUM_PLAYERS 9
#define SMALL_BLIND 1.5
#define BIG_BLIND 3.0
#define STARTING_STACK_SIZE 300.0
#define NUM_HANDS 1000000

using namespace std;


vector<double> roundOfBetting(int index, vector<Player> &players, vector<double> pot, vector<Card> board,
                              vector<map<Player, bool> > &inPot, bool preFlop);
vector<double> processBets(map<Player, double> moneyCommitted, vector<double> pot, vector<map<Player, bool> > &inPot);
double sumVector(vector<double> pot);

/* main method
 * run entire program
 * right now, it only simulates a given number of hands
 */
int main(int argc, const char * argv[]) {
    
    srand (time(0)); //seed the RNG
    
    vector<Player> players;
    
    for (short i = 0; i < NUM_PLAYERS; i++) {
        Player newPlayer(STARTING_STACK_SIZE, i);
        newPlayer.setInHand(true);
        players.push_back(newPlayer);
    }
    
    int dealerIndex = NUM_PLAYERS - 1;
    int index = 0;
    
    std::clock_t start;
    double duration;
    
    start = std::clock();
    
    // simulate a number of hands
    for (int handNumber = 0; handNumber < NUM_HANDS; handNumber++) {
        
        Deck deck;
        vector<double> pot;
        vector<map<Player, bool> > inPot;
        
        dealerIndex++;
        if (dealerIndex == NUM_PLAYERS) {
            dealerIndex = 0;
        }
        
        index++;
        if (index == NUM_PLAYERS) {
            index = 0;
        }
        
        //deal cards
        for (int i = 0; i < NUM_PLAYERS; i++) {
            players.at(i).setHand(deck.deal(), deck.deal());
        }
        
        vector<Card> board;
        
        //bet
        vector<Round> vec;
        index = 0;
        pot = roundOfBetting(index, players, pot, board, inPot, true);
        
        //flop
        board.push_back(deck.deal());
        board.push_back(deck.deal());
        board.push_back(deck.deal());
        
        //bet
        pot = roundOfBetting(index, players, pot, board, inPot, false);
        
        //turn
        board.push_back(deck.deal());
        
        //bet
        pot = roundOfBetting(index, players, pot, board, inPot, false);
        
        //river
        board.push_back(deck.deal());
        
        //bet
        pot = roundOfBetting(index, players, pot, board, inPot, false);
        
        //evaluate the hands using OMP
        omp::HandEvaluator eval;
        for (int i = 0; i < NUM_PLAYERS; i++) {
            if (players.at(i).isInHand()) {
                omp::Hand h = omp::Hand::empty();
                pair<Card, Card> holeCards = players.at(i).getHand();
                h += omp::Hand(board[0].numericValue())
                + omp::Hand(board[1].numericValue())
                + omp::Hand(board[2].numericValue())
                + omp::Hand(board[3].numericValue())
                + omp::Hand(board[4].numericValue())
                + omp::Hand(holeCards.first.numericValue())
                + omp::Hand(holeCards.second.numericValue());
                players.at(i).setHandStrength(eval.evaluate(h));
            }
        }
        
        vector<Player> sortedByStrength = players;
        
        sort(sortedByStrength.begin(), sortedByStrength.end(), [](Player& lhs, Player& rhs)
             {
                 return lhs.getHandStrength() > rhs.getHandStrength();
             });
        
        // determine the winners for each pot
        for (short i = 0; i < pot.size(); i++) {
            double winningHandStrength = 0.0;
            vector<short> winners;
            for (vector<Player>::iterator it = sortedByStrength.begin(); it != sortedByStrength.end(); it++) {
                if (inPot.at(i)[*it] && it->getHandStrength() > winningHandStrength) {
                    winningHandStrength = it->getHandStrength();
                }
            }
            
            for (int j = 0; j < NUM_PLAYERS; j++) {
                if (inPot.at(i)[players.at(j)] && players.at(j).getHandStrength() >= winningHandStrength) {
                    winners.push_back(j);
                }
            }
            
            for (int j = 0; j < winners.size(); j++) {
                int index = winners[j];
                players.at(index).addToStack(pot[i]/winners.size());
            }
            
            // add players back for the next hand
            for (int i = 0; i < NUM_PLAYERS; i++) {
                players.at(i).setInHand(true);
                players.at(i).resetStack(STARTING_STACK_SIZE);
            }
        }
    }
    
    duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    
    cout << "Time: " << duration << " seconds" << endl;

    // print out stack sizes
    for (int i = 0; i < NUM_PLAYERS; i++) {
        cout << i << "\t" << players.at(i).getBankroll() << endl;
    }
    
    return 0;
}

/*
 * index: the index of the player who is first to act
 * players: vector of players at the table
 * pot: the vector of pots
 * board: the community cards. size = 0 preflop, 3 postflop, 4 turn, 5 river
 * inPot: keeps track of who is eligible for winning each pot
 * preflop: boolean that indicates if this is preflop action.  preflop actions differs due to blind bets
 * called for each round of new action
 * returns the resulting pot after the round of betting
 */
vector<double> roundOfBetting(int index, vector<Player> &players, vector<double> pot, vector<Card> board,
                              vector<map<Player, bool> > &inPot, bool preFlop) {
    vector<Round> vec;
    map<Player, double> moneyCommitted;
    
    for (int i = 0; i < players.size(); i++) {
        Player p = players.at(i);
        moneyCommitted[p] = 0.0;
    }
    
    double currentBet = 0.0;
    if (preFlop) {
        moneyCommitted[players.at(index)] = SMALL_BLIND;
        players.at(index).bet(SMALL_BLIND);
        index++;
        moneyCommitted[players.at(index)] = BIG_BLIND;
        players.at(index).bet(BIG_BLIND);
        index++;
        currentBet = BIG_BLIND;
    }
    
    int lastToAct = index;
    
    // while there is still action ahead
    do {
        if (players.at(index).isInHand() && !players.at(index).isAllIn()) {
            moneyCommitted[players.at(index)] += players.at(index).decision(currentBet-moneyCommitted[players.at(index)],
                                                                            sumVector(pot), board, vec);
            
            if (moneyCommitted[players.at(index)] < currentBet && !players.at(index).isAllIn()) {
                players.at(index).setInHand(false);
            } else if (moneyCommitted[players.at(index)] > currentBet) {
                currentBet = moneyCommitted[players.at(index)];
                lastToAct = index;
            }
            
        }
        index++;
        
        if (index == NUM_PLAYERS) {
            index = 0;
        }
        
    } while (index != lastToAct);
    
    pot = processBets(moneyCommitted, pot, inPot);
    
    return pot;
}

/*
 * process the amounts committed to the pot by each player
 * determine which players are still eligible for which pots
 * divide the pots into side-pots based on how much was committed by each player
 * moneyCommited is a map that maps a player to the amount he committed
 * pot is a vector of doubles: the amount in each pot previous to this round of betting
 * inPot: a map for each pot detailing which players are eligible to win said pot
 * called at the end of each betting round
 * returns the resulting pot (the inPot information is passed by reference)
 */
vector<double> processBets(map<Player, double> moneyCommitted, vector<double> pot, vector<map<Player, bool> > &inPot) {
    vector<double> bets;
    
    // get a vector containing the amounts committed
    for (map<Player, double>::iterator it = moneyCommitted.begin(); it != moneyCommitted.end(); it++) {
        bets.push_back(it->second);
    }
    
    // sort said vector
    sort(bets.begin(), bets.end());
    
    for (vector<double>::iterator it = bets.begin(); it != bets.end(); it++) {
        double amount = *it;
        
        // separate pots in order of increasing exclusiveness
        if (amount > 0.0) {
            pot.push_back(amount *(distance(it, bets.end()))); // move the bets into the pot
            
            for (vector<double>::iterator it2 = bets.begin(); it2 != bets.end(); it2++) {
                *it2 -= amount; //decrement the rest of the bets
            }
            
            map<Player, bool> inNextPot;
            
            for (map<Player, double>::iterator mapIt = moneyCommitted.begin(); mapIt != moneyCommitted.end(); mapIt++) {
                Player p = mapIt->first;
                moneyCommitted[p] = moneyCommitted[p] - amount;
                
                if (moneyCommitted[p] >= 0) {
                    inNextPot[p] = true;
                } else {
                    inNextPot[p] = false;
                }
            }
            
            inPot.push_back(inNextPot);
        }
    }
    
    return pot;
}

/* 
 * parameter: a vector of doubles
 * called to total the pot size.  total pot size is used for decision making rather than the size
 * of individual side-pots
 * returns the sum of a vector of doubles
 */
double sumVector(vector<double> pot) {
    double sum = 0;
    for (auto it = pot.begin(); it != pot.end(); it++) {
        sum += *it;
    }
    return sum;
}
