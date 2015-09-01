#ifndef CARDGROUP_H
#define CARDGROUP_H

#include <set>
#include <vector>
#include <string>
#include "Card.h"

class CardGroup {

public:
    typedef std::set<Card> group;
    typedef group::iterator iterator;
    typedef group::const_iterator const_iterator;
    typedef group::difference_type difference_type;
    typedef group::value_type value_type;
    typedef group::pointer pointer;
    typedef group::reference reference;
    
    // subclass to throw exceptions
    class exception : public std::exception {
    public:
        constexpr static const char* exceptionName = "CardGroup::Exception";
    	enum errcode {
            ELEMENT_ALREADY_INSERTED,
            CARD_NOT_PRESENT,
            GROUP_IS_EMPTY,
            TOO_FEW_CARDS
        };
    private:
    	enum errcode type;
        static const char *Reason[4];
    public:
    	exception( enum errcode x ) : type(x) {};
        const char *what() const throw() { return (std::string(exceptionName) + ": " + Reason[type]).c_str(); }
    };
    
    
    group cards;
    
    
public:
    // iterators and container methods
    iterator        begin()             const noexcept;
    iterator        end()               const noexcept;
    iterator        find(Card& card);
    Card            first();
    
    // control and membership methods methods
    //// mem: check if [card] is an element of the group
    bool mem(const Card& card);
    bool mem(Card::Suit suit, unsigned int value);
    //// includes:  (1) check if [big] includes [small];
    ////            (2) check if all elements in [beginSmall,endSmall) are included in [big]
    friend bool includes(CardGroup& big, CardGroup& small);
    template <class It> friend bool includes(CardGroup& big, It beginSmall, It endSmall);
    //// empty: check if group is empty
    bool empty();
    //// size: return the number of cards
    size_t size();
    //// filter: return a CardGroup containing only the elements for which the [condition] is true
    CardGroup filter(std::function<bool(Card)> condition);
    //// choose, chooseRandom: select a card from the group (without extraction)
    Card choose();
    Card chooseRandom();
    //// extractRandom: extract a card choosen randomly in the group
    Card extractRandom();
    // insert, erase, move
    //// insert: insert a card (if not already present)
    void insert(const Card& card);
    void insert(Card::Suit suit, unsigned int value);
    //// insertAll: insert all cards present in [cg] (without deleting the original)
    void insertAll(const CardGroup& cg);
    //// erase: erase a card
    void erase(const Card& card);
    void erase(Card::Suit suit, unsigned int value);
    //// move: move a card from [from] to [to]
    friend void move(CardGroup& from, CardGroup& to, const Card& card);
    friend void move(CardGroup& from, CardGroup& to, Card::Suit suit, unsigned int value);
    //// moveall: move all the cards in [from] to [to]
    friend void moveAll(CardGroup& from, CardGroup& to);
    // game related methods
    //// deal: choose randomly [cardNumber] cards from [from] and move them to [to]
    friend void deal(CardGroup& from, CardGroup& to, unsigned int cardNumber);
    //// sumValue: give back the sum of the value of the cards
    unsigned int sumValue();
    // encoding and deconding
    static unsigned int encode(const CardGroup& cg);
    static unsigned int encode(const std::vector<Card>& cv);
    static std::vector<Card> decode(unsigned int val);
    static std::vector<Card> encodingVector(const CardGroup& cg);
    static std::vector<Card> encodingVector(const std::vector<Card>& cv);

};


// template methods
// TODO: must I put thoose methods in a .tpp file? Decide it

template <class It>
bool includes(CardGroup& big, It beginSmall, It endSmall) {
    return std::includes(big.cards.begin(),big.cards.end(),beginSmall,endSmall);
}


#endif // CARDGROUP_H