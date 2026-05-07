// ============================================================
//  main.cpp  —  SFML 3.0.2 Frontend for UNO Card Game
//
//  HOW TO COMPILE (Windows, MinGW g++ with SFML 3.0.2):
//    g++ -std=c++17 main.cpp -o UnoGame
//        -I<SFML_PATH>/include
//        -L<SFML_PATH>/lib
//        -lsfml-graphics -lsfml-window -lsfml-system
//
//  Files needed in the same folder as this file:
//    - UNO_Game_v5.h   (your backend header)
//    - arial.ttf        (copy from C:/Windows/Fonts/arial.ttf)
// ============================================================

#include <SFML/Graphics.hpp>
#include "backend.h"
#include <string>
#include <algorithm>

// ============================================================
//  CONSTANTS
// ============================================================
const unsigned WIN_W = 1100;
const unsigned WIN_H = 680;

// ============================================================
//  SCREEN STATE
// ============================================================
enum class Screen { MENU, RULES, GAME };

// ============================================================
//  CARD COLOUR -> sf::Color
// ============================================================
sf::Color toSFColor(Color c)
{
    switch (c)
    {
        case Color::RED:    return sf::Color(210, 45,  45);
        case Color::BLUE:   return sf::Color(40,  100, 210);
        case Color::GREEN:  return sf::Color(40,  170, 70);
        case Color::YELLOW: return sf::Color(220, 190, 20);
        default:            return sf::Color(55,  55,  55);  // WILD = dark grey
    }
}

// ============================================================
//  DRAW ONE UNO CARD  (coloured rect + centred label)
// ============================================================
void drawUnoCard(sf::RenderWindow& win, sf::Font& font,
                 const Card* card,
                 float x, float y,
                 float w = 75.f, float h = 108.f,
                 bool highlight = false)
{
    sf::RectangleShape rect({ w, h });
    rect.setPosition({ x, y });
    rect.setFillColor(toSFColor(card->getColor()));
    rect.setOutlineColor(highlight ? sf::Color::Yellow : sf::Color::White);
    rect.setOutlineThickness(highlight ? 3.5f : 1.5f);
    win.draw(rect);

    sf::Text lbl(font, card->getLabel(), 12);
    lbl.setFillColor(sf::Color::White);
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
    lbl.setPosition({ x + w / 2.f, y + h / 2.f });
    win.draw(lbl);
}

// ============================================================
//  DRAW A BUTTON  (filled rect + centred text)
//  Returns the FloatRect used, so the event handler can reuse
//  the same coordinates without duplicating numbers.
// ============================================================
sf::FloatRect drawButton(sf::RenderWindow& win, sf::Font& font,
                         const std::string& text,
                         float x, float y, float w, float h,
                         sf::Color fill, unsigned charSize = 20)
{
    sf::RectangleShape box({ w, h });
    box.setPosition({ x, y });
    box.setFillColor(fill);
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2.f);
    win.draw(box);

    sf::Text lbl(font, text, charSize);
    lbl.setFillColor(sf::Color::White);
    sf::FloatRect lb = lbl.getLocalBounds();
    lbl.setOrigin({ lb.size.x / 2.f, lb.size.y / 2.f });
    lbl.setPosition({ x + w / 2.f, y + h / 2.f });
    win.draw(lbl);

    return sf::FloatRect({ x, y }, { w, h });
}

// ============================================================
//  MAIN
// ============================================================
int main()
{
    // ---- Window ------------------------------------------------
    sf::RenderWindow window(
        sf::VideoMode({ WIN_W, WIN_H }),
        "Card Game Arena",
        sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    // ---- Font --------------------------------------------------
    sf::Font font;
    if (!font.openFromFile("D:\\SFML-3.0.2\\arial.ttf"))
        if (!font.openFromFile("D:\\SFML-3.0.2\\arial.ttf"))
            return -1;

    // ---- Game manager (singleton) ------------------------------
    GameManager* gm = GameManager::getInstance();
    gm->addPlayer(new Player("David"));
    gm->addPlayer(new Player("Sarah"));
    gm->startGame();

    Screen screen = Screen::MENU;

    // ============================================================
    //  MAIN LOOP
    // ============================================================
    while (window.isOpen())
    {
        // ========================================================
        //  EVENTS
        // ========================================================
        while (auto evOpt = window.pollEvent())
        {
            if (evOpt->is<sf::Event::Closed>())
            {
                window.close();
                break;
            }

            auto* mp = evOpt->getIf<sf::Event::MouseButtonPressed>();
            if (!mp || mp->button != sf::Mouse::Button::Left)
                continue;

            sf::Vector2f mouse = window.mapPixelToCoords(
                { mp->position.x, mp->position.y });

            // ---- MENU ------------------------------------------
            if (screen == Screen::MENU)
            {
                if (sf::FloatRect({350, 270}, {300, 52}).contains(mouse))
                {
                    gm->resetGame();
                    screen = Screen::GAME;
                }
                else if (sf::FloatRect({350, 348}, {300, 52}).contains(mouse))
                    screen = Screen::RULES;
                else if (sf::FloatRect({350, 426}, {300, 52}).contains(mouse))
                    window.close();
            }

            // ---- RULES -----------------------------------------
            else if (screen == Screen::RULES)
            {
                if (sf::FloatRect({420, 555}, {200, 46}).contains(mouse))
                    screen = Screen::MENU;
            }

            // ---- GAME ------------------------------------------
            else if (screen == Screen::GAME)
            {
                TurnPhase phase = gm->getTurnPhase();

                // Win screen: only "New Game" is clickable
                if (phase == TurnPhase::GAME_OVER)
                {
                    if (sf::FloatRect({370, 390}, {280, 52}).contains(mouse))
                        gm->resetGame();
                    continue;
                }

                Player* cur    = gm->getCurrentPlayer();
                int     handSz = cur->getHandSize();

                // DRAW CARD button
                if (phase == TurnPhase::CHOOSE_ACTION &&
                    sf::FloatRect({875, 200}, {130, 110}).contains(mouse))
                {
                    gm->drawCard();
                    continue;
                }

                // Play It / Keep It after drawing
                if (phase == TurnPhase::AWAITING_DRAW_DECISION)
                {
                    if (sf::FloatRect({330, 490}, {140, 40}).contains(mouse))
                        gm->playDrawnCard();
                    else if (sf::FloatRect({490, 490}, {140, 40}).contains(mouse))
                        gm->keepDrawnCard();
                    continue;
                }

                // Colour picker after Wild
                if (phase == TurnPhase::CHOOSE_COLOR)
                {
                    Color cols[4] = { Color::RED, Color::BLUE,
                                      Color::GREEN, Color::YELLOW };
                    float cx = 330.f, cy = 265.f,
                          bw =  88.f, bh =  88.f, bgap = 12.f;
                    for (int i = 0; i < 4; i++)
                    {
                        if (sf::FloatRect({cx + i*(bw+bgap), cy}, {bw, bh})
                                .contains(mouse))
                        {
                            gm->setChosenColor(cols[i]);
                            if (!gm->hasCardsOfColor(cols[i]))
                                gm->skipWildFollowUp();
                            break;
                        }
                    }
                    continue;
                }

                // Hand card clicks
                if (phase == TurnPhase::CHOOSE_ACTION ||
                    phase == TurnPhase::Play_Choosen_Card)
                {
                    // Replicate the same layout logic used in the draw section
                    float cw = 75.f, gap = 6.f;
                    float totalW = handSz * (cw + gap) - gap;
                    float availW = float(WIN_W) - 100.f;

                    if (totalW > availW)
                    {
                        cw    = (availW - gap * (handSz - 1)) / handSz;
                        gap   = (handSz > 1)
                                ? (availW - cw * handSz) / (handSz - 1)
                                : 0.f;
                        totalW = handSz * (cw + gap) - gap;
                    }

                    float startX = std::max(50.f,
                                   (float(WIN_W) - totalW) / 2.f);
                    float cardY  = 500.f;

                    for (int i = 0; i < handSz; i++)
                    {
                        if (sf::FloatRect({startX + i*(cw+gap), cardY}, {cw, 108.f}).contains(mouse))
                        {
                            if (phase == TurnPhase::CHOOSE_ACTION)
                                gm->tryPlayCard(i);
                            else
                                gm->tryPlayWildFollowUp(i);
                            break;
                        }
                    }
                }
            }
        } // end event loop

        // ========================================================
        //  DRAW
        // ========================================================
        window.clear(sf::Color(18, 18, 38));

        // ---- MENU SCREEN ----------------------------------------
        if (screen == Screen::MENU)
        {
            // Panel
            sf::RectangleShape panel({ 520.f, 340.f });
            panel.setPosition({ 290.f, 160.f });
            panel.setFillColor(sf::Color(28, 28, 68));
            panel.setOutlineColor(sf::Color(110, 110, 210));
            panel.setOutlineThickness(3.f);
            window.draw(panel);

            // Heading
            sf::Text heading(font, "CARD GAME ARENA", 36);
            heading.setFillColor(sf::Color(190, 190, 255));
            heading.setStyle(sf::Text::Bold);
            sf::FloatRect hb = heading.getLocalBounds();
            heading.setOrigin({ hb.size.x / 2.f, 0.f });
            heading.setPosition({ float(WIN_W) / 2.f, 178.f });
            window.draw(heading);

            sf::Text tag(font, "Ready to Dive in?", 18);
            tag.setFillColor(sf::Color(160, 160, 220));
            sf::FloatRect tb = tag.getLocalBounds();
            tag.setOrigin({ tb.size.x / 2.f, 0.f });
            tag.setPosition({ float(WIN_W) / 2.f, 228.f });
            window.draw(tag);

            // Three buttons — x/y/w/h MUST match the hit-rects above
            drawButton(window, font, "Start Game",
                       350, 270, 300, 52, sf::Color(60, 60, 140));
            drawButton(window, font, "Rules",
                       350, 348, 300, 52, sf::Color(40, 100, 100));
            drawButton(window, font, "Quit Game",
                       350, 426, 300, 52, sf::Color(130, 40, 40));
        }

        // ---- RULES SCREEN ---------------------------------------
        else if (screen == Screen::RULES)
        {
            sf::RectangleShape panel({ 700.f, 490.f });
            panel.setPosition({ 150.f, 70.f });
            panel.setFillColor(sf::Color(22, 22, 58));
            panel.setOutlineColor(sf::Color(110, 110, 210));
            panel.setOutlineThickness(3.f);
            window.draw(panel);

            sf::Text title(font, "UNO  RULES", 32);
            title.setFillColor(sf::Color(200, 200, 255));
            title.setStyle(sf::Text::Bold);
            sf::FloatRect tb = title.getLocalBounds();
            title.setOrigin({ tb.size.x / 2.f, 0.f });
            title.setPosition({ float(WIN_W) / 2.f, 90.f });
            window.draw(title);

            const char* rules[] = {
                "1.  Each player starts with 7 cards.",
                "2.  Match the top card by COLOR or NUMBER.",
                "3.  SKIP        : Next player loses their turn.",
                "4.  REVERSE     : Acts as Skip in a 2-player game.",
                "5.  DRAW TWO    : Next player draws 2 cards.",
                "6.  WILD        : Choose any color to continue play.",
                "7.  WILD DRAW 4 : Choose a color; next player draws 4.",
                "8.  If you cannot play, draw one card from the deck.",
                "9.  First player to empty their hand WINS!"
            };

            float ry = 158.f;
            for (auto& r : rules)
            {
                sf::Text rt(font, r, 18);
                rt.setFillColor(sf::Color(215, 215, 215));
                rt.setPosition({ 180.f, ry });
                window.draw(rt);
                ry += 36.f;
            }

            // Back button — x/y/w/h MUST match hit-rect above
            drawButton(window, font, "Back",
                       420, 555, 200, 46, sf::Color(70, 70, 140));
        }

        // ---- GAME SCREEN ----------------------------------------
        else if (screen == Screen::GAME)
        {
            TurnPhase phase = gm->getTurnPhase();

            // ---- WIN OVERLAY ------------------------------------
            if (phase == TurnPhase::GAME_OVER)
            {
                // Dark full-screen tint
                sf::RectangleShape tint({ float(WIN_W), float(WIN_H) });
                tint.setFillColor(sf::Color(0, 0, 0, 185));
                window.draw(tint);

                // Winner panel
                sf::RectangleShape panel({ 480.f, 220.f });
                panel.setPosition({ 310.f, 205.f });
                panel.setFillColor(sf::Color(22, 55, 22));
                panel.setOutlineColor(sf::Color::Yellow);
                panel.setOutlineThickness(4.f);
                window.draw(panel);

                std::string winMsg = gm->getWinner()->getName() + "  WINS!";
                sf::Text wt(font, winMsg, 44);
                wt.setFillColor(sf::Color::Yellow);
                wt.setStyle(sf::Text::Bold);
                sf::FloatRect wb = wt.getLocalBounds();
                wt.setOrigin({ wb.size.x / 2.f, 0.f });
                wt.setPosition({ float(WIN_W) / 2.f, 228.f });
                window.draw(wt);

                sf::Text sub(font, "Congratulations!", 22);
                sub.setFillColor(sf::Color::White);
                sf::FloatRect sb = sub.getLocalBounds();
                sub.setOrigin({ sb.size.x / 2.f, 0.f });
                sub.setPosition({ float(WIN_W) / 2.f, 298.f });
                window.draw(sub);

                sf::Text hint(font, "Press  'New Game'  to play again.", 16);
                hint.setFillColor(sf::Color(180, 180, 180));
                sf::FloatRect hb = hint.getLocalBounds();
                hint.setOrigin({ hb.size.x / 2.f, 0.f });
                hint.setPosition({ float(WIN_W) / 2.f, 336.f });
                window.draw(hint);

                // New Game button — x/y/w/h MUST match hit-rect above
                drawButton(window, font, "New Game",
                           370, 390, 280, 52, sf::Color(45, 110, 45));

                window.display();
                continue;   // skip the normal game draw below
            }

            // ---- NORMAL GAME DRAW -------------------------------
            Player* cur   = gm->getCurrentPlayer();
            Player* other = gm->getOtherPlayer();

            // Green felt background
            sf::RectangleShape felt({ float(WIN_W) - 80.f, float(WIN_H) - 60.f });
            felt.setPosition({ 40.f, 30.f });
            felt.setFillColor(sf::Color(22, 50, 22));
            felt.setOutlineColor(sf::Color(70, 130, 70));
            felt.setOutlineThickness(3.f);
            window.draw(felt);

            // Title
            sf::Text title(font, "GAME IN SESSION", 24);
            title.setFillColor(sf::Color(170, 255, 170));
            title.setStyle(sf::Text::Bold);
            sf::FloatRect titleB = title.getLocalBounds();
            title.setOrigin({ titleB.size.x / 2.f, 0.f });
            title.setPosition({ float(WIN_W) / 2.f, 40.f });
            window.draw(title);

            // Turn indicator
            sf::Text turnTxt(font, cur->getName() + "'s  Turn", 17);
            turnTxt.setFillColor(sf::Color(255, 215, 80));
            sf::FloatRect ttb = turnTxt.getLocalBounds();
            turnTxt.setOrigin({ ttb.size.x / 2.f, 0.f });
            turnTxt.setPosition({ float(WIN_W) / 2.f, 72.f });
            window.draw(turnTxt);

            // ---- LEFT: Player name buttons ----------------------
            // p0 = player added first (index 0), always at y=160
            // p1 = player added second (index 1), always at y=255
            // Active player gets purple highlight + bold name
            {
                int     ci = gm->getCurrentPlayerIdx();
                Player* p0 = (ci == 0) ? cur   : other;
                Player* p1 = (ci == 0) ? other : cur;

                auto drawPBtn = [&](Player* p, float y)
                {
                    bool active = (p == cur);

                    sf::RectangleShape pb({ 160.f, 52.f });
                    pb.setPosition({ 55.f, y });
                    pb.setFillColor(active
                        ? sf::Color(75, 35, 115)
                        : sf::Color(35, 35, 75));
                    pb.setOutlineColor(active
                        ? sf::Color(175, 95, 255)
                        : sf::Color(75, 75, 120));
                    pb.setOutlineThickness(active ? 3.f : 1.5f);
                    window.draw(pb);

                    sf::Text pt(font, p->getName(), 17);
                    if (active) pt.setStyle(sf::Text::Bold);
                    pt.setFillColor(active
                        ? sf::Color(230, 175, 255)
                        : sf::Color(175, 175, 175));
                    sf::FloatRect ptb = pt.getLocalBounds();
                    pt.setOrigin({ ptb.size.x / 2.f, ptb.size.y / 2.f });
                    pt.setPosition({ 135.f, y + 26.f });
                    window.draw(pt);

                    sf::Text cnt(font,
                        "Cards: " + std::to_string(p->getHandSize()), 13);
                    cnt.setFillColor(sf::Color(190, 190, 190));
                    cnt.setPosition({ 57.f, y + 57.f });
                    window.draw(cnt);
                };

                drawPBtn(p0, 160.f);   // index-0 player always top
                drawPBtn(p1, 255.f);   // index-1 player always bottom
            }

            // ---- CENTRE: Top Card -------------------------------
            sf::Text tcLbl(font, "TOP  CARD", 14);
            tcLbl.setFillColor(sf::Color(190, 255, 190));
            tcLbl.setPosition({ 468.f, 158.f });
            window.draw(tcLbl);

            if (gm->getTopCard())
                drawUnoCard(window, font, gm->getTopCard(),
                            452.f, 178.f, 100.f, 135.f);

            // ---- RIGHT: Draw Card button ------------------------
            // x=875 y=200 w=130 h=110  — matches hit-rect above
            {
                sf::RectangleShape db({ 130.f, 110.f });
                db.setPosition({ 875.f, 200.f });
                db.setFillColor(sf::Color(80, 28, 28));
                db.setOutlineColor(sf::Color(200, 75, 75));
                db.setOutlineThickness(2.f);
                window.draw(db);

                sf::Text dt(font, "DRAW\nCARD", 20);
                dt.setFillColor(sf::Color::White);
                dt.setStyle(sf::Text::Bold);
                sf::FloatRect dtb = dt.getLocalBounds();
                dt.setOrigin({ dtb.size.x / 2.f, dtb.size.y / 2.f });
                dt.setPosition({ 940.f, 255.f });
                window.draw(dt);
            }

            // ---- BOTTOM: Current player's hand ------------------
            {
                const auto& hand = cur->getHand();
                int   sz    = (int)hand.size();
                float cw    = 75.f;
                float gap   = 6.f;
                float totalW = sz * (cw + gap) - gap;
                float availW = float(WIN_W) - 100.f;

                // Shrink cards proportionally if hand is too wide
                if (totalW > availW)
                {
                    cw    = (availW - gap * (sz - 1)) / sz;
                    gap   = (sz > 1)
                            ? (availW - cw * sz) / (sz - 1)
                            : 0.f;
                    totalW = sz * (cw + gap) - gap;
                }

                float startX = std::max(50.f,
                               (float(WIN_W) - totalW) / 2.f);
                float cardY  = 500.f;

                sf::Text handLbl(font, cur->getName() + "'s  Hand:", 15);
                handLbl.setFillColor(sf::Color(170, 255, 170));
                handLbl.setPosition({ startX, 482.f });
                window.draw(handLbl);

                for (int i = 0; i < sz; i++)
                {
                    bool hi = (hand[i] == gm->getLastDrawnCard());
                    drawUnoCard(window, font, hand[i],
                                startX + i*(cw+gap), cardY,
                                cw, 108.f, hi);
                }
            }

            // ---- PHASE OVERLAYS ---------------------------------

            // AWAITING_DRAW_DECISION
            if (phase == TurnPhase::AWAITING_DRAW_DECISION)
            {
                sf::RectangleShape ov({ 380.f, 75.f });
                ov.setPosition({ 320.f, 455.f });
                ov.setFillColor(sf::Color(12, 12, 52, 238));
                ov.setOutlineColor(sf::Color::White);
                ov.setOutlineThickness(1.5f);
                window.draw(ov);

                sf::Text msg(font, "You drew a playable card!", 16);
                msg.setFillColor(sf::Color::White);
                sf::FloatRect mb = msg.getLocalBounds();
                msg.setOrigin({ mb.size.x / 2.f, 0.f });
                msg.setPosition({ 510.f, 461.f });
                window.draw(msg);

                // Buttons — x/y/w/h MUST match hit-rects above
                drawButton(window, font, "Play It",
                           330, 490, 140, 40, sf::Color(35, 100, 35));
                drawButton(window, font, "Keep It",
                           490, 490, 140, 40, sf::Color(110, 60, 15));
            }

            // CHOOSE_COLOR
            if (phase == TurnPhase::CHOOSE_COLOR)
            {
                sf::RectangleShape ovBg({ 440.f, 178.f });
                ovBg.setPosition({ 320.f, 218.f });
                ovBg.setFillColor(sf::Color(8, 8, 38, 242));
                ovBg.setOutlineColor(sf::Color::White);
                ovBg.setOutlineThickness(2.f);
                window.draw(ovBg);

                sf::Text pick(font, "Choose a Color:", 21);
                pick.setFillColor(sf::Color::White);
                sf::FloatRect pb = pick.getLocalBounds();
                pick.setOrigin({ pb.size.x / 2.f, 0.f });
                pick.setPosition({ float(WIN_W) / 2.f, 228.f });
                window.draw(pick);

                // Colour squares — x/y/w/h MUST match hit-rects above
                Color       cols[4] = { Color::RED, Color::BLUE,
                                        Color::GREEN, Color::YELLOW };
                const char* nms[4]  = { "Red", "Blue", "Green", "Yellow" };
                float cx = 330.f, cy = 265.f,
                      bw =  88.f, bh =  88.f, bgap = 12.f;

                for (int i = 0; i < 4; i++)
                {
                    sf::RectangleShape cb({ bw, bh });
                    cb.setPosition({ cx + i*(bw+bgap), cy });
                    cb.setFillColor(toSFColor(cols[i]));
                    cb.setOutlineColor(sf::Color::White);
                    cb.setOutlineThickness(2.f);
                    window.draw(cb);

                    sf::Text nl(font, nms[i], 15);
                    nl.setFillColor(sf::Color::White);
                    sf::FloatRect nb = nl.getLocalBounds();
                    nl.setOrigin({ nb.size.x / 2.f, nb.size.y / 2.f });
                    nl.setPosition({ cx + i*(bw+bgap) + bw/2.f,
                                     cy + bh/2.f });
                    window.draw(nl);
                }
            }

            // Play_Choosen_Card: colour hint
            if (phase == TurnPhase::Play_Choosen_Card)
            {
                std::string hint = "Play a  " +
                    colorToString(gm->getPendingWildColor()) +
                    "  card  (or any Wild)";

                sf::Text info(font, hint, 17);
                info.setFillColor(toSFColor(gm->getPendingWildColor()));
                info.setStyle(sf::Text::Bold);
                sf::FloatRect ib = info.getLocalBounds();
                info.setOrigin({ ib.size.x / 2.f, 0.f });
                info.setPosition({ float(WIN_W) / 2.f, 462.f });
                window.draw(info);
            }

        } // end GAME screen draw

        window.display();

    } // end main loop

    return 0;
}
