#include "CardGroup.h"
#include <cstdlib>


// exception messagges
const char *CardGroup::exception::Reason[4] = {
    "The element is already present in the container.",
    "The card is not present in the group.",
    "The group is empty.",
    "The group has too few cards."
};


// public methods
CardGroup::iterator CardGroup::begin()          const noexcept  { return cards.begin();     }
CardGroup::iterator CardGroup::end()            const noexcept  { return cards.end();       }
CardGroup::iterator CardGroup::find(Card& card)                 { return cards.find(card);  }
Card                CardGroup::first()                          { return *begin();          }


bool CardGroup::mem(const Card& card) {
    group::iterator it = cards.find(card);
    return ( it != cards.end() );
}
bool CardGroup::mem(Card::Suit suit, unsigned int value) {
    return mem(Card(suit,value));
}

bool includes(CardGroup& big, CardGroup& small) {
    return std::includes(big.cards.begin(),big.cards.end(),small.cards.begin(),small.cards.end());
}

// template <class It> bool includes(CardGroup& big, It beginSmall, It endSmall)
//// As a template method, this one has to be put directly into the header

bool CardGroup::empty() {
    return cards.size() == 0;
}

size_t CardGroup::size() {
    return cards.size();
}

CardGroup CardGroup::filter(std::function<bool(Card)> condition) {
    CardGroup result;
    for(group::iterator it = cards.begin(); it != cards.end(); it++) {
        if(condition(*it)) { result.insert(*it); }
    }
    return result;
}

Card CardGroup::choose() {
    if(empty()) { throw exception(exception::GROUP_IS_EMPTY); }
    return *cards.begin();
}
Card CardGroup::chooseRandom() {
    if(empty()) { throw exception(exception::GROUP_IS_EMPTY); }
    int range = cards.size();
    group::iterator it = cards.begin();
    std::advance(it, rand()%range);
    return *it;
}

Card CardGroup::extractRandom() {
    Card tmpCard = chooseRandom();
    erase(tmpCard);
    return tmpCard;
}

void CardGroup::insert(const Card& card) {
    std::pair<group::iterator,bool> insertionResult = cards.insert(card);
    if(!insertionResult.second) { throw exception(exception::ELEMENT_ALREADY_INSERTED); }
}
void CardGroup::insert(Card::Suit suit, unsigned int value) {
    insert(Card(suit,value));
}

void CardGroup::insertAll(const CardGroup& cg) {
    for(CardGroup::iterator it = cg.begin(); it != cg.end(); it++) { insert(*it); }
}

void CardGroup::erase(const Card& card) {
    size_t erasedElementsNum = cards.erase(card);
    if(erasedElementsNum == 0) { throw exception(exception::CARD_NOT_PRESENT); }
}
void CardGroup::erase(Card::Suit suit, unsigned int value) {
    erase(Card(suit,value));
}

void move(CardGroup& from, CardGroup& to, const Card& card) {
    from.erase(card);
    to.insert(card);
}
void move(CardGroup& from, CardGroup& to, Card::Suit suit, unsigned int value) {
    from.erase(suit,value);
    to.insert(suit,value);
}

void moveAll(CardGroup& from, CardGroup& to) {
    while(!from.empty()) {
        move(from,to,from.choose());
    }
}

void deal(CardGroup& from, CardGroup& to, unsigned int cardNumber = 1) {
    if(from.cards.size() < cardNumber) { throw CardGroup::exception(CardGroup::exception::TOO_FEW_CARDS); }
    for(size_t i = 0; i < cardNumber; ++i) {
        move(from,to,from.chooseRandom());
    }
}

unsigned int CardGroup::sumValue() {
    unsigned int result = 0;
    for(group::iterator it = cards.begin(); it != cards.end(); it++) {
        result += it->value;
    }
    return result;
}

unsigned int CardGroup::encode(const CardGroup& cg) {
    unsigned int result = 0;
    std::vector<Card> toEncode = encodingVector(cg);
    for(std::vector<Card>::iterator it = toEncode.begin(); it != toEncode.end(); it++) {
        result += Card::encode(*it);
        result *= 21;
    }
    result /= 21;
    return result;
}

unsigned int encode(const std::vector<Card>& cv) {
    unsigned int result = 0;
    std::vector<Card> toEncode = CardGroup::encodingVector(cv);
    for(std::vector<Card>::iterator it = toEncode.begin(); it != toEncode.end(); it++) {
        result += Card::encode(*it);
        result *= 21;
    }
    result /= 21;
    return result;
}

std::vector<Card> CardGroup::decode(unsigned int val) {
    std::vector<Card> result = std::vector<Card>();
    while(val != 0) {
        unsigned int r = val % 21;
        result.push_back(Card::decode(r));
        val /= 21;
    }
    return result;
}

std::vector<Card> CardGroup::encodingVector(const CardGroup& cg) {
    std::vector<Card> result;
    for(CardGroup::iterator it = cg.begin(); it != cg.end(); it++) {
        if(it->suit == Card::DENARI) { result.push_back(*it); }
        else { result.push_back(Card(Card::VOID,it->value)); }
    }
    std::sort(result.begin(),result.end());
    return result;
}

std::vector<Card> CardGroup::encodingVector(const std::vector<Card>& cv) {
    std::vector<Card> result;
    for(std::vector<Card>::const_iterator it = cv.begin(); it != cv.end(); it++) {
        if(it->suit == Card::DENARI) { result.push_back(*it); }
        else { result.push_back(Card(Card::VOID,it->value)); }
    }
    std::sort(result.begin(),result.end());
    return result;
}
