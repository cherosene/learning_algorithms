#include "Scopa.h"

// exception messagges
const char *Scopa::exception::Reason[8] = {
    "The deck has too few cards for this operation.",
    "Match has not ended yet.",
    "Hands are not empty.",
    "Hand is empty.",
    "Invalid capture.",
    "The card was not there.",
    "This array is not a score array (wrong size).",
    "Wrong turn to play."
};


constexpr Card::Suit Scopa::suits[];


// public methods

Scopa::Scopa() {
    // put the cards in the deck
    for(size_t suitIndex = 0; suitIndex < sizeof(suits)/sizeof(Card::Suit); ++suitIndex) {
        for(size_t valueIndex = 1; valueIndex <= MAX_CARD_VALUE; ++valueIndex) {
            deck.insert(suits[suitIndex],valueIndex);
        }
    }
}

bool Scopa::matchHasEnded() {
    return deck.empty() && hand[PLAYER_ZERO].empty() && hand[PLAYER_ONE].empty();
}

bool Scopa::roundHasEnded() {
    return hand[PLAYER_ZERO].empty() && hand[PLAYER_ONE].empty();
}

void Scopa::startMatch(bool jumpControl) {
    if(!jumpControl && !matchHasEnded()) { throw exception(exception::MATCH_HAS_NOT_ENDED); }
    pointsMatch[PLAYER_ZERO] = 0;
    pointsMatch[PLAYER_ONE]  = 0;

    moveAll(table,deck);
    moveAll(hand[PLAYER_ZERO],deck);
    moveAll(hand[PLAYER_ONE],deck);
    moveAll(capturedPile[PLAYER_ZERO],deck);
    moveAll(capturedPile[PLAYER_ONE],deck);

    dealTable();
    dealHand(PLAYER_ZERO);
    dealHand(PLAYER_ONE);
}

void Scopa::startRound() {
    if(!hand[PLAYER_ZERO].empty() || !hand[PLAYER_ONE].empty()) { throw exception(exception::HANDS_ARE_NOT_EMPTY); }
    
    dealHand(PLAYER_ZERO);
    dealHand(PLAYER_ONE);
}

void Scopa::swapTurn() {
    currentTurn = currentTurn == PLAYER_ZERO? PLAYER_ONE : PLAYER_ZERO;
}

void Scopa::evaluateScore() {
    if(!matchHasEnded()) { throw exception(exception::MATCH_HAS_NOT_ENDED); }
    
    moveAll(table,capturedPile[lastCapturePlayer]);
    // cards
    if(capturedPile[PLAYER_ZERO].size() > capturedPile[PLAYER_ONE].size()) { pointsMatch[PLAYER_ZERO]++; }
    else if(capturedPile[PLAYER_ZERO].size() < capturedPile[PLAYER_ONE].size()) { pointsMatch[PLAYER_ONE]++; }
    // denari
    std::function<bool(Card)> isOfDenari = [](Card card){ return card.suit == Card::DENARI; };
    CardGroup capturedDenari[PLAYER_NUMBER];
    capturedDenari[PLAYER_ZERO] = capturedPile[PLAYER_ZERO].filter(isOfDenari);
    capturedDenari[PLAYER_ONE]  = capturedPile[PLAYER_ONE].filter(isOfDenari);
    if(capturedDenari[PLAYER_ZERO].size() > capturedDenari[PLAYER_ONE].size()) { pointsMatch[PLAYER_ZERO]++; }
    else if(capturedDenari[PLAYER_ZERO].size() < capturedDenari[PLAYER_ONE].size()) { pointsMatch[PLAYER_ONE]++; }
}

CardGroup Scopa::chooseBestCapture(unsigned int value, std::function<float(CardGroup)> evaluationFun) {
    std::vector<CardGroup> captures = possibleCaptures(value);
    return chooseBestCapture(captures, evaluationFun);
}

CardGroup Scopa::chooseBestCapture(std::vector<CardGroup> captures, std::function<float(CardGroup)> evaluationFun) {
    if(captures.empty()) { return CardGroup(); }
    
    CardGroup* best = &captures.front();
    float bestValue = evaluationFun(*best);
    for(std::vector<CardGroup>::iterator it = captures.begin() + 1; it != captures.end(); it++) {
        float currentValue = evaluationFun(*it);
        if(currentValue > bestValue) {
            bestValue = currentValue;
            best = &*it;
        }
    }
    return *best;
}

float Scopa::standardCaptureEvaluationFunction(CardGroup cg) {
    float score = 0.;
    for(CardGroup::iterator it = cg.begin(); it != cg.end(); it++) {
        if(isPointCard(*it)) { score++; }
    }
    if(cg.size() == table.size()) { score++; }
    return score;
}

unsigned int Scopa::playCard(Who player, Card card, const CardGroup& capt) {
    return playCard(player,card,capt.cards);
}

unsigned int Scopa::playCard(Who player, Card card, capture capt) {
    if(currentTurn != player) { throw exception(exception::WRONG_TURN); }
    if(!hand[player].mem(card)) { throw exception(exception::CARD_NOT_FOUND); }
    if(!includes(table,capt.begin(),capt.end())) { throw exception(exception::NOT_A_VALID_CAPTURE); }
    
    unsigned int pointsEarned = 0;
    
    if(capt.empty()) {
        // hand -> table
        move(hand[player],table,card);
    }
    else {
        lastCapturePlayer = player;
        // hand -> capturedPile
        move(hand[player],capturedPile[player],card);
        if(Scopa::isPointCard(card)) {
            pointsEarned++;
            pointsMatch[player]++;
        }
        // table -> capturedPile
        for(capture::iterator it = capt.begin(); it != capt.end(); it++) {
            move(table,capturedPile[player],*it);
            if(Scopa::isPointCard(*it)) {
                pointsEarned++;
                pointsMatch[player]++;
            }
        }
        // is scopa?
        if(table.empty()) {
            pointsEarned++;
            pointsMatch[player]++;
        }
    }
    
    swapTurn();
    return pointsEarned;
}

std::vector<CardGroup> Scopa::possibleCaptures(unsigned int val) {
    std::vector<CardGroup> partialResult, result;
    std::vector<CardGroup> tmpSet = possibleCapturesAux(val);
    bool onlySingleCardCaptures = false;
    for(std::vector<CardGroup>::iterator it = tmpSet.begin(); it != tmpSet.end(); it++) {
        if(it->sumValue() == val) {
            if(it->size() == 1) { onlySingleCardCaptures = true; }
            partialResult.push_back(*it);
        }
    }
    if(onlySingleCardCaptures) {
        for(std::vector<CardGroup>::iterator it = partialResult.begin(); it != partialResult.end(); it++) {
            if(it->size() == 1) { result.push_back(*it); }
        }
        return result;
    }
    else { return partialResult; }
}

unsigned int Scopa::howManyCaptures(std::vector<CardGroup> capturesVec, unsigned int val) {
    unsigned int result = 0;
    for(std::vector<CardGroup>::iterator it = capturesVec.begin(); it != capturesVec.end(); it++) {
        if(it->sumValue() == val) { result++; }
    }
    return result;
}

unsigned int Scopa::strategyPlay(std::function<Card()> strategyCard, std::function<CardGroup(std::vector<CardGroup>)> strategyCapture) {
    Card cardToPlay = strategyCard();
    std::vector<CardGroup> captures = possibleCaptures(cardToPlay.value);
    if( captures.size() == 0 ) { return playCard(currentTurn, cardToPlay, CardGroup()); }
    else if( captures.size() == 1 ) { return playCard(currentTurn, cardToPlay, captures.front()); }
    else { return playCard(currentTurn, cardToPlay, strategyCapture(captures)); }
}


// protected and private methods

void Scopa::dealHand(Who player) {
    if(deck.size() < 3) { throw exception(exception::DECK_HAS_TOO_FEW_CARDS); }
    deal(deck,hand[player],3);
}

void Scopa::dealTable() {
    if(deck.size() < 4) { throw exception(exception::DECK_HAS_TOO_FEW_CARDS); }
    deal(deck,table,4);
}

bool Scopa::isPointCard(const Card& card) {
    return card == Card(Card::DENARI,7) || card == Card(Card::DENARI,10);
}

std::vector<CardGroup> Scopa::possibleCapturesAux(unsigned int val) {
    std::vector<CardGroup> result;
    
    for(CardGroup::iterator cardIt = std::prev(table.end()); cardIt != std::prev(table.begin()); cardIt--) {
        std::vector<CardGroup> cgToAdd;
        for(std::vector<CardGroup>::iterator resIt = result.begin(); resIt != result.end(); resIt++) {
            if(resIt->sumValue() + cardIt->value <= val) {
                CardGroup tmpCg = *resIt;
                tmpCg.insert(*cardIt);
                cgToAdd.push_back(tmpCg);
            }
        }
        result.insert(result.end(),cgToAdd.begin(),cgToAdd.end());
        CardGroup tmpCg = CardGroup();
        tmpCg.insert(*cardIt);
        result.push_back(tmpCg);
    }
    
    return result;
}