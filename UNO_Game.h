#pragma once

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

using namespace std;

// Card ka color store karta hai. WILD = no fixed color (Wild cards ke liye)
enum class Color    { RED, BLUE, GREEN, YELLOW, WILD };

// Card ka type store karta hai
enum class CardType { NUMBER, SKIP, REVERSE, DRAW_TWO, WILD, WILD_DRAW_FOUR };

// GUI ko batata hai ab game kis state mein hai — har action method pehle yahi check karta hai
enum class TurnPhase
{
    CHOOSE_ACTION,          // Normal turn — card khelo ya draw karo
    AWAITING_DRAW_DECISION, // Draw kiya — ab play karo ya rakh lo
    CHOOSE_COLOR,           // Wild khela — color choose karo
    Play_Choosen_Card,         // Color choose ho gaya — us color ka follow-up card khelo
    GAME_OVER               // Koi player jeet gaya
};

// Color enum ko readable string mein convert karta hai — e.g. RED → "Red"
inline string colorToString(Color c)
{
    switch (c) {
        case Color::RED:    return "Red";
        case Color::BLUE:   return "Blue";
        case Color::GREEN:  return "Green";
        case Color::YELLOW: return "Yellow";
        case Color::WILD:   return "Wild";
        default:            return "Unknown";
    }
}

// CardType enum ko readable string mein convert karta hai — e.g. SKIP → "Skip"
inline string cardTypeToString(CardType t)
{
    switch (t) {
        case CardType::NUMBER:         return "Number";
        case CardType::SKIP:           return "Skip";
        case CardType::REVERSE:        return "Reverse";
        case CardType::DRAW_TWO:       return "Draw Two";
        case CardType::WILD:           return "Wild";
        case CardType::WILD_DRAW_FOUR: return "Wild Draw Four";
        default:                       return "Unknown";
    }
}

// Abstract base class — seedha Card object nahi ban sakta, sirf NormalCard/SpecialCard banta hai
class Card
{
protected:
    Color    color;
    CardType type;

public:
    // Color aur type store karo
    Card(Color color, CardType type) : color(color), type(type) {}
    virtual ~Card() = default;

    // Card ka color return karo
    Color    getColor() const { return color; }

    // Card ka type return karo
    CardType getType()  const { return type;  }

    // Subclass batayegi display name — e.g. "Blue 5", "Red Skip"
    virtual string getLabel() const = 0;

    // Number cards ke liye value return karta hai — baaki ke liye default -1
    virtual int    getValue() const { return -1; }

    // Subclass batayegi match logic — color ya type/value se compare karo
    virtual bool   operator==(const Card& other) const = 0;
};

// 0-9 number cards ke liye
class NormalCard : public Card
{
    int value;
public:
    // Color aur number value store karo
    NormalCard(Color color, int v) : Card(color, CardType::NUMBER), value(v) {}

    // Card ka number return karo (0-9)
    int    getValue() const override { return value; }

    // e.g. "Blue 5"
    string getLabel() const override
    {
        return colorToString(color) + " " + to_string(value);
    }

    // Same color YA same number ho toh match
    bool operator==(const Card& other) const override
    {
        return color == other.getColor() || value == other.getValue();
    }
};

// Skip, Reverse, Draw Two, Wild, Wild Draw Four ke liye
class SpecialCard : public Card
{
public:
    // Color aur type store karo
    SpecialCard(Color color, CardType type) : Card(color, type) {}

    // e.g. "Red Skip", "Wild", "Wild Draw Four"
    string getLabel() const override
    {
        switch (type) {
            case CardType::SKIP:           return colorToString(color) + " Skip";
            case CardType::REVERSE:        return colorToString(color) + " Reverse";
            case CardType::DRAW_TWO:       return colorToString(color) + " Draw Two";
            case CardType::WILD:           return "Wild";
            case CardType::WILD_DRAW_FOUR: return "Wild Draw Four";
            default:                       return "Unknown Special";
        }
    }

    // Same color YA same type ho toh match
    bool operator==(const Card& other) const override
    {
        return color == other.getColor() || type == other.getType();
    }
};

// Static factory — koi object nahi banta, seedha card generate hota hai
class UnoDeck
{
    static inline bool seeded = false;

public:
    // 1-56 random id se ek card new karke return karta hai
    static Card* GenerateOneCard()
    {
        if (!seeded) { srand((unsigned)time(nullptr)); seeded = true; }
        int id = rand() % 56 + 1;

        if (id == 1)  return new NormalCard(Color::BLUE, 0);
        if (id == 2)  return new NormalCard(Color::BLUE, 1);
        if (id == 3)  return new NormalCard(Color::BLUE, 2);
        if (id == 4)  return new NormalCard(Color::BLUE, 3);
        if (id == 5)  return new NormalCard(Color::BLUE, 4);
        if (id == 6)  return new NormalCard(Color::BLUE, 5);
        if (id == 7)  return new NormalCard(Color::BLUE, 6);
        if (id == 8)  return new NormalCard(Color::BLUE, 7);
        if (id == 9)  return new NormalCard(Color::BLUE, 8);
        if (id == 10) return new NormalCard(Color::BLUE, 9);

        if (id == 11) return new NormalCard(Color::GREEN, 0);
        if (id == 12) return new NormalCard(Color::GREEN, 1);
        if (id == 13) return new NormalCard(Color::GREEN, 2);
        if (id == 14) return new NormalCard(Color::GREEN, 3);
        if (id == 15) return new NormalCard(Color::GREEN, 4);
        if (id == 16) return new NormalCard(Color::GREEN, 5);
        if (id == 17) return new NormalCard(Color::GREEN, 6);
        if (id == 18) return new NormalCard(Color::GREEN, 7);
        if (id == 19) return new NormalCard(Color::GREEN, 8);
        if (id == 20) return new NormalCard(Color::GREEN, 9);

        if (id == 21) return new NormalCard(Color::YELLOW, 0);
        if (id == 22) return new NormalCard(Color::YELLOW, 1);
        if (id == 23) return new NormalCard(Color::YELLOW, 2);
        if (id == 24) return new NormalCard(Color::YELLOW, 3);
        if (id == 25) return new NormalCard(Color::YELLOW, 4);
        if (id == 26) return new NormalCard(Color::YELLOW, 5);
        if (id == 27) return new NormalCard(Color::YELLOW, 6);
        if (id == 28) return new NormalCard(Color::YELLOW, 7);
        if (id == 29) return new NormalCard(Color::YELLOW, 8);
        if (id == 30) return new NormalCard(Color::YELLOW, 9);

        if (id == 31) return new NormalCard(Color::RED, 0);
        if (id == 32) return new NormalCard(Color::RED, 1);
        if (id == 33) return new NormalCard(Color::RED, 2);
        if (id == 34) return new NormalCard(Color::RED, 3);
        if (id == 35) return new NormalCard(Color::RED, 4);
        if (id == 36) return new NormalCard(Color::RED, 5);
        if (id == 37) return new NormalCard(Color::RED, 6);
        if (id == 38) return new NormalCard(Color::RED, 7);
        if (id == 39) return new NormalCard(Color::RED, 8);
        if (id == 40) return new NormalCard(Color::RED, 9);

        if (id == 41) return new SpecialCard(Color::BLUE,   CardType::SKIP);
        if (id == 42) return new SpecialCard(Color::GREEN,  CardType::SKIP);
        if (id == 43) return new SpecialCard(Color::YELLOW, CardType::SKIP);
        if (id == 44) return new SpecialCard(Color::RED,    CardType::SKIP);

        if (id == 45) return new SpecialCard(Color::BLUE,   CardType::REVERSE);
        if (id == 46) return new SpecialCard(Color::GREEN,  CardType::REVERSE);
        if (id == 47) return new SpecialCard(Color::YELLOW, CardType::REVERSE);
        if (id == 48) return new SpecialCard(Color::RED,    CardType::REVERSE);

        if (id == 49) return new SpecialCard(Color::BLUE,   CardType::DRAW_TWO);
        if (id == 50) return new SpecialCard(Color::GREEN,  CardType::DRAW_TWO);
        if (id == 51) return new SpecialCard(Color::YELLOW, CardType::DRAW_TWO);
        if (id == 52) return new SpecialCard(Color::RED,    CardType::DRAW_TWO);

        if (id <= 54) return new SpecialCard(Color::WILD, CardType::WILD);

        return new SpecialCard(Color::WILD, CardType::WILD_DRAW_FOUR);
    }

    // count baar GenerateOneCard() call karo, vector return karo
    static vector<Card*> GenerateCards(int count)
    {
        vector<Card*> cards;
        for (int i = 0; i < count; i++)
            cards.push_back(GenerateOneCard());
        return cards;
    }
};

// Ek player — naam, hand (cards), aur UNO flag store karta hai
class Player
{
    string        name;
    vector<Card*> hand;
    bool          saidUno;

public:
    // Naam set karo, saidUno false rakho
    Player(const string& name) : name(name), saidUno(false) {}

    // Apne saare cards delete karo
    ~Player() { for (Card* c : hand) delete c; }

    // Player ka naam return karo
    string               getName()     const { return name;             }

    // UNO bola ya nahi
    bool                 getSaidUno()  const { return saidUno;          }

    // Hand mein kitne cards hain
    int                  getHandSize() const { return (int)hand.size(); }

    // Poori hand ka const reference return karo
    const vector<Card*>& getHand()     const { return hand;             }

    // UNO flag manually set karo
    void setSaidUno(bool val) { saidUno = val; }

    // Hand mein ek card daalo
    void addCard(Card* card)  { if (card) hand.push_back(card); }

    // Hand se card vector se nikalo — delete nahi karta, caller ki responsibility hai
    void removeCard(Card* card)
    {
        for (int i = 0; i < (int)hand.size(); i++) {
            if (hand[i] == card) {
                hand.erase(hand.begin() + i);
                if ((int)hand.size() != 1) saidUno = false;
                return;
            }
        }
    }
};

// Singleton — poore game ka brain, state aur logic yahan hai
class GameManager
{
    static inline GameManager* instance = nullptr;

    Card*           topCard          = nullptr;
    vector<Player*> players;
    int             currentPlayerIdx = 0;

    TurnPhase phase            = TurnPhase::CHOOSE_ACTION;
    Color     pendingWildColor = Color::RED;
    Card*     lastDrawnCard    = nullptr;
    Player*   winner           = nullptr;

    GameManager() = default;

    // Card hand se nikalo, purana topCard replace karo, applyEffect() trigger karo
    void internalPlayCard(Player* p, Card* card)
    {
        p->removeCard(card);
        delete topCard;
        topCard = card;
        phase = TurnPhase::CHOOSE_ACTION;
        applyEffect(card);
    }

    // Card type ke hisaab se side effects lagao — Skip/Reverse pre-advance, Draw Two/WD4 cards do, Wild phase change karo
    void applyEffect(Card* card)
    {
        switch (card->getType())
        {
        case CardType::NUMBER:
            break;

        case CardType::SKIP:
        case CardType::REVERSE:
            advanceTurn();
            break;

        case CardType::DRAW_TWO:
            getOtherPlayer()->addCard(UnoDeck::GenerateOneCard());
            getOtherPlayer()->addCard(UnoDeck::GenerateOneCard());
            break;

        case CardType::WILD:
            phase = TurnPhase::CHOOSE_COLOR;
            break;

        case CardType::WILD_DRAW_FOUR:
            for (int i = 0; i < 4; i++)
                getOtherPlayer()->addCard(UnoDeck::GenerateOneCard());
            phase = TurnPhase::CHOOSE_COLOR;
            break;

        default:
            break;
        }
    }

    // Hand empty hai toh winner set karo aur GAME_OVER phase karo
    bool checkWin(Player* p)
    {
        if (p->getHandSize() == 0) {
            winner = p;
            phase  = TurnPhase::GAME_OVER;
            return true;
        }
        return false;
    }

public:
    // Singleton instance return karo — nahi hai toh pehli baar banao
    static GameManager* getInstance()
    {
        if (!instance) instance = new GameManager();
        return instance;
    }

    ~GameManager()
    {
        delete topCard;
        for (Player* p : players) delete p;
    }

    // Players vector mein player daalo
    void addPlayer(Player* p) { players.push_back(p); }

    // Dono players ko 7-7 cards deal karo, non-Wild top card rakho, state reset karo
    void startGame()
    {
        for (Player* p : players) {
            vector<Card*> dealt = UnoDeck::GenerateCards(7);
            for (Card* c : dealt) p->addCard(c);
        }

        do {
            delete topCard;
            topCard = UnoDeck::GenerateOneCard();
        } while (topCard->getType() == CardType::WILD ||
                 topCard->getType() == CardType::WILD_DRAW_FOUR);

        phase            = TurnPhase::CHOOSE_ACTION;
        currentPlayerIdx = 0;
        winner           = nullptr;
        lastDrawnCard    = nullptr;
    }

    // Sab kuch delete karo, naye players banao, startGame() call karo — "New Game" button ke liye
    void resetGame()
    {
        delete topCard;
        topCard = nullptr;
        for (Player* p : players) delete p;
        players.clear();

        winner        = nullptr;
        lastDrawnCard = nullptr;

        players.push_back(new Player("David"));
        players.push_back(new Player("Sarah"));
        startGame();
    }

    // Jiska abhi turn hai woh player return karo
    Player* getCurrentPlayer()    { return players[currentPlayerIdx];     }

    // Dusra player return karo
    Player* getOtherPlayer()      { return players[1 - currentPlayerIdx]; }

    // currentPlayerIdx toggle karo — 0→1, 1→0
    void    advanceTurn()         { currentPlayerIdx = 1 - currentPlayerIdx; }

    // Current player ka index return karo (0 ya 1)
    int     getCurrentPlayerIdx() const { return currentPlayerIdx; }

    // Current TurnPhase return karo
    TurnPhase getTurnPhase()        const { return phase;            }

    // Discard pile ka top card return karo
    Card*     getTopCard()          const { return topCard;          }

    // Wild ke baad player ne jo color choose kiya woh return karo
    Color     getPendingWildColor() const { return pendingWildColor; }

    // Abhi draw kiya hua card return karo
    Card*     getLastDrawnCard()    const { return lastDrawnCard;    }

    // Phase GAME_OVER hai ya nahi
    bool      isGameOver()          const { return phase == TurnPhase::GAME_OVER; }

    // Winner player return karo — game khatam nahi toh nullptr
    Player*   getWinner()           const { return winner;           }

    // Current player ke paas us color ka koi card hai ya nahi — Wild cards count nahi hote
    bool hasCardsOfColor(Color c) const
    {
        for (const Card* card : players[currentPlayerIdx]->getHand())
            if (card->getColor() == c) return true;
        return false;
    }

    // Card top card pe legal hai ya nahi — Wild hamesha true, warna color ya type/value match check karo
    bool isPlayable(const Card* card) const
    {
        if (card->getType() == CardType::WILD ||
            card->getType() == CardType::WILD_DRAW_FOUR) return true;
        if (card->getColor() == topCard->getColor())      return true;
        if (card->getType() == topCard->getType()) {
            if (card->getType() == CardType::NUMBER)
                return card->getValue() == topCard->getValue();
            return true;
        }
        return false;
    }

    // Normal turn mein hand se card khelo — phase/index/playability check karo, phir internalPlayCard()
    bool tryPlayCard(int handIndex)
    {
        if (phase != TurnPhase::CHOOSE_ACTION) return false;

        Player* p = getCurrentPlayer();
        if (handIndex < 0 || handIndex >= p->getHandSize()) return false;

        Card* card = p->getHand()[handIndex];
        if (!isPlayable(card)) return false;

        internalPlayCard(p, card);
        if (checkWin(p)) return true;

        if (phase == TurnPhase::CHOOSE_ACTION)
            advanceTurn();

        return true;
    }

    // Deck se ek card lo — playable hai toh AWAITING_DRAW_DECISION, nahi toh auto-pass
    void drawCard()
    {
        if (phase != TurnPhase::CHOOSE_ACTION) return;

        Player* p     = getCurrentPlayer();
        Card*   drawn = UnoDeck::GenerateOneCard();
        p->addCard(drawn);
        lastDrawnCard = drawn;

        if (isPlayable(drawn))
            phase = TurnPhase::AWAITING_DRAW_DECISION;
        else {
            lastDrawnCard = nullptr;
            advanceTurn();
        }
    }

    // Abhi draw kiya hua card khelo
    void playDrawnCard()
    {
        if (phase != TurnPhase::AWAITING_DRAW_DECISION || !lastDrawnCard) return;

        Player* p    = getCurrentPlayer();
        Card*   card = lastDrawnCard;
        lastDrawnCard = nullptr;

        internalPlayCard(p, card);
        if (checkWin(p)) return;

        if (phase == TurnPhase::CHOOSE_ACTION)
            advanceTurn();
    }

    // Drawn card rakh lo, turn end karo
    void keepDrawnCard()
    {
        if (phase != TurnPhase::AWAITING_DRAW_DECISION) return;
        lastDrawnCard = nullptr;
        phase = TurnPhase::CHOOSE_ACTION;
        advanceTurn();
    }

    // Wild ke baad player ka chosen color set karo, phase Play_Choosen_Card karo
    void setChosenColor(Color c)
    {
        if (phase != TurnPhase::CHOOSE_COLOR) return;
        pendingWildColor = c;
        phase = TurnPhase::Play_Choosen_Card;
    }

    // Wild ke baad chosen color ka follow-up card khelo
    bool tryPlayWildFollowUp(int handIndex)
    {
        if (phase != TurnPhase::Play_Choosen_Card) return false;

        Player* p = getCurrentPlayer();
        if (handIndex < 0 || handIndex >= p->getHandSize()) return false;

        Card* card = p->getHand()[handIndex];
        if (card->getColor() != pendingWildColor) return false;

        internalPlayCard(p, card);
        if (checkWin(p)) return true;

        if (phase == TurnPhase::CHOOSE_ACTION)
            advanceTurn();

        return true;
    }

    // Koi matching color card nahi — turn end karo
    void skipWildFollowUp()
    {
        if (phase != TurnPhase::Play_Choosen_Card) return;
        phase = TurnPhase::CHOOSE_ACTION;
        advanceTurn();
    }
};
