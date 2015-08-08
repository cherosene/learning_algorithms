#ifndef CARDGROUP_H
#define CARDGROUP_H

#include <vector>
#include <SDL.h>
#include "Card.h"

class CardGroup {

public:
    typedef std::vector<Card>::iterator cardIterator;
    
    std::vector<Card> cards;
    bool isVisible;
    bool isPile;
    
    SDL_Renderer* ren;
    
    int x;
    int y;
    
    float xShift;
    float yShift;
    
public:
    CardGroup(bool visible = true, bool pile = true, SDL_Renderer* nren = nullptr, int nx = 0, int ny = 0, float nxShift = 0., float nyShift = 0.);
    
    bool mem(const Card& card) const;
    
    void pushTop(Card ncard);
    void pushBottom(Card ncard);
    void insert(Card ncard, int index);
    void insert(Card ncard, cardIterator position);
    
    void popTop();
    void popBottom();
    void erase(int index);
    void erase(cardIterator position);
    
    friend void move(CardGroup& from, CardGroup& to, const Card& card);
    friend void move(CardGroup& from, CardGroup& to, cardIterator card);
    friend void move(CardGroup& from, CardGroup& to, Card::Suit s, int v);
    friend void moveAll(CardGroup& from, CardGroup& to);
    friend void moveAll(std::vector<CardGroup*> from, CardGroup& to);
    friend void deal(CardGroup& from, CardGroup& to, int cardNum);
    
    void shuffle();
    
    std::pair<int,int> cardCoordinates(Card card);
    void render();
};


#endif // CARDGROUP_H