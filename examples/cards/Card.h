#ifndef CARD_H
#define CARD_H


#include <vector>
#include <utility>
#include <SDL.h>


class Card {
public:
    
    static const int CARD_WIDTH = 70;
    static const int CARD_HEIGHT = 100;
    
    enum Suit {VOID, DENARI, BASTONI, SPADE, COPPE};
    typedef std::pair<Card::Suit,int> t;
    
    Suit suit;
    int value;
    SDL_Texture* frontTex = nullptr;
    SDL_Texture* backTex = nullptr;
    
    int w;
    int h;
    
    
public:
    Card(Suit s, int v, SDL_Texture* nfrontTex, SDL_Texture* nbackTex, int nwidth = CARD_WIDTH, int nheight = CARD_HEIGHT);
    
    friend bool operator==(const Card& lc, const Card& rc);
    friend bool operator!=(const Card& lc, const Card& rc);
    friend bool operator< (const Card& lc, const Card& rc);
    friend bool operator<=(const Card& lc, const Card& rc);
    
    friend bool cvcmp(std::vector<Card> v1, std::vector<Card> v2);
};


#endif // CARD_H