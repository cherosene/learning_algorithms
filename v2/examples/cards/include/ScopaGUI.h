#ifndef SCOPAGUI_H
#define SCOPAGUI_H

#include <SDL.h>
#include "Scopa.h"


class ScopaGUI : public Scopa {
public:
    enum GroupRole { DECK, TABLE, HAND_UP, HAND_DOWN, CAPTURED_PILE_UP, CAPTURED_PILE_DOWN, GROUP_ROLE_NUM};
    enum Key { VOID, UP, DOWN, LEFT, RIGHT, RETURN, ESCAPE };
    
    constexpr static const GroupRole PLAYER_POSITION[2] = { HAND_UP, HAND_DOWN };
    
    // graphic constants
    constexpr static const char* WINDOW_NAME    = "Scopa";
    static const unsigned int WINDOW_X          = 100;
    static const unsigned int WINDOW_Y          = 100;
    static const unsigned int WINDOW_WIDTH      = 960;
    static const unsigned int WINDOW_HEIGHT     = 640;
    
    static const unsigned int CARD_WIDTH    = 60;
    static const unsigned int CARD_HEIGHT   = 120;
    
    static const unsigned int CURSOR_WIDTH  = 20;
    static const unsigned int CURSOR_HEIGHT = 29;
    
    constexpr static const bool           GROUP_ROLE_IS_PILE      [GROUP_ROLE_NUM]    = { true,   false,  false,  false,  true,   true  };
    constexpr static const bool           GROUP_ROLE_IS_VISIBLE   [GROUP_ROLE_NUM]    = { false,  true,   true,   false,  true,   true  };
    constexpr static const unsigned int   GROUP_ROLE_X            [GROUP_ROLE_NUM]    = { 750,    150,    800,    100,    100,    800   };
    constexpr static const unsigned int   GROUP_ROLE_Y            [GROUP_ROLE_NUM]    = { 260,    260,    50,     470,    50,     470   };
    constexpr static const          int   GROUP_ROLE_SHIFT_X      [GROUP_ROLE_NUM]    = { 0,      60,     -60,    60,     0,      0     };
    constexpr static const          int   GROUP_ROLE_SHIFT_Y      [GROUP_ROLE_NUM]    = { 0,      0,      0,      0,      0,      0     };
    
    // FIXME: some images are jpg, some png
    constexpr static const char* CARD_IMAGE_NAMES[41] = {
        "Back.jpg",
    
        "01 - Bastoni.jpg", "02 - Bastoni.jpg", "03 - Bastoni.jpg", "04 - Bastoni.jpg", "05 - Bastoni.jpg",
        "06 - Bastoni.jpg", "07 - Bastoni.jpg", "08 - Bastoni.jpg", "09 - Bastoni.jpg", "10 - Bastoni.jpg",
    
        "01 - Coppe.jpg",   "02 - Coppe.jpg",   "03 - Coppe.jpg",   "04 - Coppe.jpg",   "05 - Coppe.jpg",
        "06 - Coppe.jpg",   "07 - Coppe.jpg",   "08 - Coppe.jpg",   "09 - Coppe.jpg",   "10 - Coppe.jpg",
    
        "01 - Spade.jpg",   "02 - Spade.jpg",   "03 - Spade.jpg",   "04 - Spade.jpg",   "05 - Spade.jpg",
        "06 - Spade.jpg",   "07 - Spade.jpg",   "08 - Spade.jpg",   "09 - Spade.jpg",   "10 - Spade.jpg",
    
        "01 - Denari.jpg",  "02 - Denari.jpg",  "03 - Denari.jpg",  "04 - Denari.jpg",  "05 - Denari.jpg",
        "06 - Denari.jpg",  "07 - Denari.jpg",  "08 - Denari.jpg",  "09 - Denari.jpg",  "10 - Denari.jpg"
    };
    constexpr static const char *CURSOR_IMAGE_NAME = "Cursor.png";
    constexpr static const char *BACKGROUND_IMAGE_NAME = "Background.png";

    constexpr static unsigned int BACK_IMAGE_CODE   = 0;
    constexpr static unsigned int CURSOR_IMAGE_CODE = 41;
    constexpr static unsigned int BACKGROUND_IMAGE_CODE = 42;
    
    
    
    // subclass to throw exceptions
    class exception : public std::exception {
    public:
        constexpr static const char* exceptionName = "ScopaGUI::exception";
    	enum errcode {
            SDL_INIT,
            SDL_CREATE_WINDOW,
            SDL_CREATE_RENDERER,
            SDL_CREATE_TEXTURE_SURFACE,
            SDL_LOAD_IMAGE,
            VOID_SUIT_NOT_ALLOWED,
            GROUP_ROLE_NOT_VALID,
            NO_FOCUS,
            FOCUS_NOT_ON_HAND,
            WRONG_FOCUS,
            FOCUS_ON_WRONG_HAND,
            CARD_NOT_FOUND
        };
    private:
    	enum errcode type;
        static const char *Reason[12];
    public:
        exception( enum errcode x ) : type(x) {};
        const char *what() const throw() { return (std::string(exceptionName) + ": " + Reason[type]).c_str(); }
    };
    
    // subclass: graphical pointers to the cards
    class Cursor {
    public:
        // FIXME: it would be good to this with references
        Card card;
        GroupRole groupRole;
    private:
        ScopaGUI& out;
        
    public:
        Cursor(const Card& ncard, GroupRole gr, ScopaGUI& nout);
        void setFocus(const Card& ncard, GroupRole gr);
        unsigned int index();
        void render();
    };
    
    
    // graphic properties
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool showOnChange = true;
    //// cursors
    Cursor* focus;
    std::vector<Cursor> cursorsToRender;
    
private:
    SDL_Texture* images[43];
    
    
    
    
public:
    ScopaGUI(const char* cardImagesDirectory);
    ~ScopaGUI();
    // graphic methods
    //// showAll: render all elements on table and show them
    void setAutomaticShowOnChange(bool autoShow);
    void showAll();
    // Cursor methods
    //// setFocus:  (1) set the focus (a Cursor pointer) on Card [card] present in the CardGroup with role [gr]
    ////            (2) set the focus (a Cursor pointer) on Card [card] present in the player [player] hand
    ////            (3) set the focus (a Cursor pointer) on the first Cart present in the player [player] hand
    void setFocus(Card ncard, GroupRole gr);
    void setFocus(Card ncard, Who player);
    void setFocus(Who player);
    //// looseFocus: set the focus pointer to nullptr
    void looseFocus(bool showHere = true);
    //// moveFocus: move the focus in the hand of [distance] steps (with sign)
    void moveFocus(int distance);
    //// moveLeft, moveRight: move the focus to the left or to the right
    void moveFocusLeft();
    void moveFocusRight();
    //// addCursor: add a Cursor to render pointing at Card [ncard] in the CardGroup with role [gr]
    void addCursor(Card ncard, GroupRole gr, bool showHere = true);
    //// addCursors: add Cursors to be rendered for the GroupRole [gr]
    void addCursors(CardGroup cg, GroupRole gr, bool showHere = true);
    //// clearCursors: delete all Cursors stored for rendering (focus not included)
    void clearCursors(bool showHere = true);
    // game methods
    //// playOnFocus: play the card pointed by focus and take from the table the set of cards identified by [cg] or [capt]
    unsigned int playOnFocus(const CardGroup& cg);
    unsigned int playOnFocus(capture capt);
    // keyboard input methods
    //// readKey: wait for keyboard input and return the corresponding Key
    Key readKey();
    // user interaction
    //// userChooseCapture: let the user choose the capture from [captures]
    CardGroup userChooseCapture(std::vector<CardGroup> captures);
    //// userPlay: let the user play its turn
    unsigned int userPlay();
    
protected:
    // GUI methods
    void startGUI();
    void stopGUI();
    void renderCardGroup(GroupRole cgPosition, CardGroup& cg);
    CardGroup& groupByRole(GroupRole gr);
    // encoding and decoding
    static unsigned int imageCode(Card& card);
    // texture and images handling
    SDL_Texture* loadTexture(const char* filename);
    void renderTexture(SDL_Texture *tex, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    void renderCard(bool isVisible, Card card, unsigned int x, unsigned int y);
    void renderBackground();
    
};



#endif // SCOPAGUI_H