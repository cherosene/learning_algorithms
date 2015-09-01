#ifndef SCOPA_H
#define SCOPA_H

#include "CardGroup.h"
#include <vector>
#include <string>

class Scopa {
public:
    typedef std::set<Card> capture;
    
    enum Who { PLAYER_ZERO, PLAYER_ONE };
    
    static const size_t PLAYER_NUMBER = 2;
    
    static const int MAX_CARD_VALUE = 10;
    static const int CARD_NUMBER    = 40;
    constexpr static Card::Suit suits[4] = { Card::BASTONI, Card::COPPE, Card::SPADE, Card::DENARI };
    
    
    // subclass to throw exceptions
    class exception : public std::exception {
    public:
        constexpr static const char* exceptionName = "Scopa::exception";
    	enum errcode {
            DECK_HAS_TOO_FEW_CARDS,
            MATCH_HAS_NOT_ENDED,
            HANDS_ARE_NOT_EMPTY,
            HAND_IS_EMPTY,
            NOT_A_VALID_CAPTURE,
            CARD_NOT_FOUND,
            NOT_A_SCORE_ARRAY,
            WRONG_TURN
        };
    private:
    	enum errcode type;
        static const char *Reason[8];
    public:
        exception( enum errcode x ) : type(x) {};
        const char *what() const throw() { return (std::string(exceptionName) + ": " + Reason[type]).c_str(); }
    };
    
    
    Who currentTurn, lastCapturePlayer;
    // groups of cards
    CardGroup table, deck, hand[PLAYER_NUMBER], capturedPile[PLAYER_NUMBER];
    // points
    unsigned int points[PLAYER_NUMBER], pointsMatch[PLAYER_NUMBER];
    
    
public:
    Scopa();
    // match control methods
    //// matchHasEnded: check if the match has ended
    bool matchHasEnded();
    //// roundHasEnded: check if the round has ended
    bool roundHasEnded();
    //// startMatch: start a new match (move all cards to deck; deal 4 cards to the table, 3 cards to each player)
    void startMatch(bool jumpControl = false);
    //// startRound: start a new round (deal 3 cards to each player)
    void startRound();
    //// swapTurn: swap the player turn
    void swapTurn();
    //// evaluateScore: at the end of the match, evaluate the score of boot players and store it in [pointsMatch]
    void evaluateScore();
    
    // card playing methods
    //// chooseBestCapture: (1) choose the best capture (based on [evaluationFun]) playing a Card with value [value] (eventually the empty one)
    ////                    (2) choose the best capture (based on [evaluationFun]) between the captures given in [captures] (eventually the empty one)
    CardGroup chooseBestCapture(unsigned int value, std::function<float(CardGroup)> evaluationFun);
    CardGroup chooseBestCapture(std::vector<CardGroup> captures, std::function<float(CardGroup)> evaluationFun);
    //// standardCaptureEvaluationFunction: define a standard evaluation function to use with chooseBestCapture
    float standardCaptureEvaluationFunction(CardGroup cg);
    //// playCard: play [card] as [player] to capture the set of cards [capt] (eventually empty); return the points earned with this play
    unsigned int playCard(Who player, Card card, const CardGroup& capt);
    unsigned int playCard(Who player, Card card, capture capt);
    
    // strategy methods
    //// possibleCaptures: return a vector with all possible captures on the table with the value given
    std::vector<CardGroup> possibleCaptures(unsigned int val);
    //// howManyCaptures: return the number of possible captures in [capturesVec] with value [val]
    static unsigned int howManyCaptures(std::vector<CardGroup> capturesVec, unsigned int val);
    //// strategyPlay: play the card choosen by [strategyCard] with capture choosen by [strtegyCapture]
    unsigned int strategyPlay(std::function<Card()> strategyCard, std::function<CardGroup(std::vector<CardGroup>)> strategyCapture);
    
    
protected:
    void dealHand(Who player);
    void dealTable();
    //// isPointCard: check if the capture of this card gives a point (7D or 10D)
    static bool isPointCard(const Card& card);
    std::vector<CardGroup> possibleCapturesAux(unsigned int val);
    
};


#endif // SCOPA_H