#include "Card.h"


// exception messagges
const char *Card::exception::Reason[1] = {
    "No problem at all."
};


// public methods
Card::Card(Suit nsuit, unsigned int nvalue) : suit(nsuit), value(nvalue) {}

// comparison operators
bool operator==(const Card& lc, const Card& rc) {
    return ( lc.value == rc.value && lc.suit == rc.suit );
}

bool operator!=(const Card& lc, const Card& rc) {
    return !(lc == rc);
}

bool operator< (const Card& lc, const Card& rc) {
    return lc.value < rc.value || (lc.value == rc.value && lc.suit < rc.suit);
}

bool operator<=(const Card& lc, const Card& rc) {
    return lc < rc || lc == rc;
}

bool operator> (const Card& lc, const Card& rc) {
    return lc.value > rc.value || (lc.value == rc.value && lc.suit > rc.suit);
}

bool operator>=(const Card& lc, const Card& rc) {
    return lc > rc || lc == rc;
}

// FIXME: encode and decode have magic numbers
unsigned int Card::encode(Card card) {
    unsigned int result = card.value;
    if(card.suit == Card::DENARI) { result += 10; }
    return result;
}

Card Card::decode(unsigned int val) {
    if(val <= 10) { return Card(Card::VOID,val); }
    else { return Card(Card::DENARI,val-10); }
}