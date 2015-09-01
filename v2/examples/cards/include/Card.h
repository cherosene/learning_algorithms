#ifndef CARD_H
#define CARD_H

#include <map>
#include <exception>
#include <string>

class Card {
public:
    enum Suit { VOID, BASTONI, COPPE, SPADE, DENARI };
    
    // subclass to throw exceptions
    class exception : public std::exception {
    public:
        constexpr static const char* exceptionName = "Card::exception";
    	enum errcode {
            VOID
        };
    private:
    	enum errcode type;
        static const char *Reason[1];
    public:
    	exception( enum errcode x ) : type(x) {};
        const char *what() const throw() { return (std::string(exceptionName) + ": " + Reason[type]).c_str(); }
    };
    
    Suit suit;
    unsigned int value;
    
public:
    Card(Suit nsuit, unsigned int nvalue);
    
    // operators
    friend bool operator==(const Card& lc, const Card& rc);
    friend bool operator!=(const Card& lc, const Card& rc);
    friend bool operator< (const Card& lc, const Card& rc);
    friend bool operator<=(const Card& lc, const Card& rc);
    friend bool operator> (const Card& lc, const Card& rc);
    friend bool operator>=(const Card& lc, const Card& rc);
    
    // encoding and deconding
    static unsigned int encode(Card card);
    static Card decode(unsigned int val);
};


#endif // CARD_H