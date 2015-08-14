#include "CardGroup.h"
#include <algorithm>
#include "utility.h"


CardGroup::CardGroup(bool visible, bool pile, SDL_Renderer* nren, int nx, int ny, float nxShift, float nyShift) : isVisible(visible), isPile(pile), ren(nren), x(nx), y(ny), xShift(nxShift), yShift(nyShift) {}


bool CardGroup::mem(const Card& card) const {
    return ( std::find(cards.begin(),cards.end(),card) != cards.end() );
}


void CardGroup::pushTop(Card ncard) { cards.insert(cards.begin(), ncard); }

void CardGroup::pushBottom(Card ncard) { cards.push_back(ncard); }

void CardGroup::insert(Card ncard, int index) { cards.insert(cards.begin()+index, ncard); }

void CardGroup::insert(Card ncard, cardIterator position) { cards.insert(position, ncard); }


void CardGroup::popTop() { cards.erase(cards.begin()); }

void CardGroup::popBottom() { cards.pop_back(); }

void CardGroup::erase(int index) { cards.erase(cards.begin()+index); }

void CardGroup::erase(cardIterator position) { cards.erase(position); }


void move(CardGroup& from, CardGroup& to, const Card& card) {
    
    CardGroup::cardIterator it = std::find(from.cards.begin(),from.cards.end(),card);
    if( it != from.cards.end() ) {
        to.pushTop( *it );
        from.erase( it );
    }
    else {
        // FIXME: throw exception
    }

}

void move(CardGroup& from, CardGroup& to, CardGroup::cardIterator card) {
    to.pushTop(*card);
    from.erase(card);
}

void move(CardGroup& from, CardGroup& to, Card::Suit s, int v) { move(from,to,Card(s,v,nullptr,nullptr)); }

void moveAll(CardGroup& from, CardGroup& to) {
    int originalSize = from.cards.size();
    for(size_t i = 0; i < originalSize; ++i)
        move(from,to,from.cards.begin());
}

void moveAll(std::vector<CardGroup*> from, CardGroup& to) {
    for(std::vector<CardGroup*>::iterator it = from.begin(); it != from.end(); it++)
        moveAll(**it,to);
}

void deal(CardGroup& from, CardGroup& to, int cardNum) {
    if( cardNum > from.cards.size() )
    {
        // FIXME: throw an exception
    }
    else {
        for(size_t i = 0; i < cardNum; ++i)
        {
            move(from, to, from.cards.begin() );
        }
    }
}


void CardGroup::shuffle() {
    // FIXME: not so random, is deterministic
    std::random_shuffle(cards.begin(), cards.end());
}


std::pair<int,int> CardGroup::cardCoordinates(Card card) {
    int tmpx = x;
    int tmpy = y;
    
    if(!isPile) {
        for(cardIterator it = cards.end()-1; *it != card; it--) {
            tmpx += (it->w) * xShift;
            tmpy += (it->h) * yShift;
        }
    }
    return std::pair<int,int>(tmpx,tmpy);
}

void CardGroup::render() {
    int tmpx = x, tmpy = y;
    SDL_Texture* tex;
    
    if(cards.empty()) { return; }
    
    if(isPile) {
        Card card = cards[0];
        tex = isVisible? card.frontTex : card.backTex;
        renderTexture(tex, ren, tmpx, tmpy, card.w, card.h);
    }
    else {
        for(cardIterator it = cards.end()-1; it != cards.begin()-1; it--) {
            tex = isVisible? it->frontTex : it->backTex;
            renderTexture(tex, ren, tmpx, tmpy, it->w, it->h);
            tmpx += (it->w) * xShift;
            tmpy += (it->h) * yShift;
        }
    }
    
}