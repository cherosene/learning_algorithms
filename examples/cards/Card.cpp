#include "Card.h"
#include "utility.h"

Card::Card(Suit s, int v, SDL_Texture* nfrontTex, SDL_Texture* nbackTex, int nwidth, int nheight) : suit(s), value(v), frontTex(nfrontTex), backTex(nbackTex), w(nwidth), h(nheight) {}


bool operator==(const Card& lc, const Card& rc) {
    return ( lc.suit == rc.suit && lc.value == rc.value );
}

bool operator!=(const Card& lc, const Card& rc) {
    return ( lc.suit != rc.suit || lc.value != rc.value );
}

bool operator<(const Card& lc, const Card& rc) {
    return lc.suit < rc.suit || (lc.suit == rc.suit && lc.value < rc.value);
}


bool cvcmp(std::vector<Card> v1, std::vector<Card> v2) {
    std::sort(v1.begin(),v1.end());
    std::sort(v2.begin(),v2.end());
    return v1 == v2;
}