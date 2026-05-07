// ============================================================
//  UNO_Game.h  —  Complete Game Backend (GUI-ready)
//
//  Changes from the original console version (UNO_Game_v5.cpp):
//    1. Removed #include <iostream> and ALL cout/cin statements
//    2. Removed runGame(), playerTurn(), displaycardsinhand()
//    3. Removed Wild_and_WildDrawFour() — GUI handles colour choice
//    4. Added TurnPhase enum for GUI state-machine
//    5. Added GUI action methods on GameManager
//    6. startGame() now deals 7 cards (was 10, debug value)
//    7. Added resetGame() for "New Game" button
//    8. Removed int main()
//
//  ALL game rules and logic are completely unchanged.
//
//  Companion file: UNO_Game.cpp  (all definitions + full comments)
// ============================================================
#pragma once    // Include guard -- ensures this header is only included once per compilation unit -- avoids redefinition errors

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// ============================================================
//  ENUMS
// ============================================================

enum class Color    { RED, BLUE, GREEN, YELLOW, WILD };
enum class CardType { NUMBER, SKIP, REVERSE, DRAW_TWO, WILD, WILD_DRAW_FOUR };

// TurnPhase tells main.cpp which screen/controls to show.
enum class TurnPhase
{
    CHOOSE_ACTION,          // Normal: pick a card to play, or draw
    AWAITING_DRAW_DECISION, // Just drew — choose to play it or keep it
    CHOOSE_COLOR,           // Wild/WD4 played — must pick a colour
    PLAY_WILD_CARD,         // Colour chosen — must play a "follow-up" card of that colour
    GAME_OVER               // A player emptied their hand
};

// ============================================================
//  HELPER FUNCTIONS
// ============================================================

string colorToString(Color c);
string cardTypeToString(CardType t);

// ============================================================
//  CARD  (abstract base)
// ============================================================

class Card
{
protected:
    Color    color;
    CardType type;

public:
    Card(Color color, CardType type);
    virtual ~Card() = default;

    Color    getColor() const;
    CardType getType()  const;

    virtual string getLabel() const = 0;
    virtual int    getValue() const;
    virtual bool   operator==(const Card& other) const = 0;
};

// ============================================================
//  NORMAL CARD  (0-9)
// ============================================================

class NormalCard : public Card
{
    int value;
public:
    NormalCard(Color color, int v);

    int    getValue() const override;
    string getLabel() const override;
    bool   operator==(const Card& other) const override;
};

// ============================================================
//  SPECIAL CARD  (Skip/Reverse/DrawTwo/Wild/WD4)
// ============================================================

class SpecialCard : public Card
{
public:
    SpecialCard(Color color, CardType type);

    string getLabel() const override;
    bool   operator==(const Card& other) const override;
};

// ============================================================
//  UNO DECK  (Factory — infinite procedural generator)
// ============================================================

class UnoDeck
{
    static bool seeded;

public:
    static Card*         GenerateOneCard();
    static vector<Card*> GenerateCards(int count);
};

// ============================================================
//  PLAYER
// ============================================================

class Player
{
    string        name;
    vector<Card*> hand;
    // bool          saidUno;  // REMOVED --- commented out

public:
    Player(const string& name);
    ~Player();

    string               getName()     const;
    // bool                 getSaidUno()  const;
    int                  getHandSize() const;
    const vector<Card*>& getHand()     const;

    // void setSaidUno(bool val);
    void addCard(Card* card);
    void removeCard(Card* card);   // removes exact pointer; does NOT delete
};

// ============================================================
//  GAME MANAGER  (Singleton)
// ============================================================

class GameManager
{
    static GameManager* instance;

    // Core state
    Card*           topCard          = nullptr;
    vector<Player*> players;
    int             currentPlayerIdx = 0;

    // GUI state-machine variables
    TurnPhase phase            = TurnPhase::CHOOSE_ACTION;
    Color     pendingWildColor = Color::RED;    // Only used during CHOOSE_WILD_CARD phase, set by GUI colour picker
    Card*     lastDrawnCard    = nullptr;   // Only used during AWAITING_DRAW_DECISION phase, holds the card just drawn from the deck
    Player*   winner           = nullptr;

    GameManager();

    void internalPlayCard(Player* p, Card* card);
    void applyEffect(Card* card);
    bool checkWin(Player* p);

public:
    static GameManager* getInstance();
    ~GameManager();

    // ---- Setup ----
    void addPlayer(Player* p);
    void startGame();
    void resetGame();

    // ---- Navigation ----
    Player* getCurrentPlayer();
    Player* getOtherPlayer();
    void    advanceTurn();
    int     getCurrentPlayerIdx() const;

    // ---- State accessors (read-only, called by GUI each frame) ----
    TurnPhase getTurnPhase()        const;
    Card*     getTopCard()          const;
    Color     getPendingWildColor() const;
    Card*     getLastDrawnCard()    const;
    bool      isGameOver()          const;
    Player*   getWinner()           const;
    bool      hasCardsOfColor(Color c) const;

    // ---- Playability check ----
    bool isPlayable(const Card* card) const;

    // ---- Turn actions (called by GUI on player interaction) ----
    bool tryPlayCard(int handIndex);
    void drawCard();
    void playDrawnCard();
    void keepDrawnCard();
    void setChosenColor(Color c);
    bool tryPlayWildFollowUp(int handIndex);
    void skipWildFollowUp();
};
