#ifndef SCOPA_H
#define SCOPA_H

#include <map>
#include <SDL.h>
#include "CardGroup.h"


class Scopa {

public:

    static constexpr int SCREEN_X      = 100;
    static constexpr int SCREEN_Y      = 100;
    static constexpr int SCREEN_WIDTH  = 960;
    static constexpr int SCREEN_HEIGHT = 640;
    static constexpr int CURSOR_WIDTH  = 20;
    static constexpr int CURSOR_HEIGHT = 29;

    static constexpr int HANDPLAYER_X          = 100;
    static constexpr int HANDPLAYER_Y          = SCREEN_HEIGHT - 200;
    static constexpr float HANDPLAYER_SHIFT_X  = 1.;
    static constexpr float HANDPLAYER_SHIFT_Y  = 0.;

    static constexpr int HANDENEMY_X           = SCREEN_WIDTH - HANDPLAYER_X - 70;
    static constexpr int HANDENEMY_Y           = SCREEN_HEIGHT - HANDPLAYER_Y - 100;
    static constexpr float HANDENEMY_SHIFT_X   = -1.;
    static constexpr float HANDENEMY_SHIFT_Y   = 0.;

    static constexpr int CAPTUREDPILEPLAYER_X  = SCREEN_WIDTH - 170;
    static constexpr int CAPTUREDPILEPLAYER_Y  = HANDPLAYER_Y;

    static constexpr int CAPTUREDPILEENEMY_X  = SCREEN_WIDTH - CAPTUREDPILEPLAYER_X - 70;
    static constexpr int CAPTUREDPILEENEMY_Y   = HANDENEMY_Y;

    static constexpr int TABLE_X               = SCREEN_WIDTH/2 - 210;
    static constexpr int TABLE_Y               = SCREEN_HEIGHT/2 - 50;
    static constexpr float TABLE_SHIFT_X       = 1.;
    static constexpr float TABLE_SHIFT_Y       = 0.;

    static constexpr int DECK_X                = CAPTUREDPILEPLAYER_X;
    static constexpr int DECK_Y                = TABLE_Y;

    enum Who {PLAYER, ENEMY};

// FIXME: create classes captureType and cursorType
    typedef std::pair<int,std::vector<Card>> captureType;
    typedef std::pair<CardGroup,Card> cursorType;
    
    CardGroup handPlayer;
    CardGroup handEnemy;
    CardGroup capturedPilePlayer;
    CardGroup capturedPileEnemy;
    CardGroup table;
    CardGroup deck;
    
    int scopaPointsPlayer;
    int scopaPointsEnemy;
    
    SDL_Renderer* ren;
    std::map<std::string,SDL_Texture*> cardTex;
    
    SDL_Texture* cursorTex;
    std::vector<cursorType> cursors;
    cursorType *focus;
    

    
public:
    Scopa(SDL_Renderer* nren);
    
    void loadDeck();
    void loadCursor();
    
    std::vector<captureType> filterByValue(std::vector<captureType> cVec, int value);
    
    void addCursor(cursorType cur);
    void addCursor(CardGroup cg, std::vector<Card> cardVec);
    void resetCursors();
    void setFocus(Who who);
    void setFocus(CardGroup cg);
    void setFocus(CardGroup cg, Card card);
    void loseFocus();
    void moveFocus(int n);
    
    std::pair<int,int> cursorCoordinates(cursorType cur);
    void renderAll();

    void dealHand(Who who);
    void dealTable();
    void startMatch();
    std::pair<int,int> evaluateScore();
    
    void sideOfTheTable(Who who, CardGroup*& hand, CardGroup*& capturedPile, int*& scopaPoints);
    bool hasCard(Who who, Card card);
    
    // create a vector containing all the possible captures
    std::vector<captureType> generateTableCaptures();
    int howManyChoice(Card card, std::vector<captureType> captures);
    bool hasMultipleChoice(Card card, std::vector<captureType> captures);
    
    bool playCard(Who who, Card card);
    bool playCard(Who who, Card card, std::vector<Card> capturedOnTable);
    void playOnFocus(Who who);
    void playOnFocus(Who who, std::vector<Card> capturedOnTable);
    
    void shuffle();

};


#endif // SCOPA_H