#include <iostream>
#include <memory>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "state/battlerdecision.h"
#include "state/gamebattleinfo.h"
#include "state/gamestate.h"
#include "state/gamestatemachine.h"
#include "store/gamelovstorage.h"
#include "store/gamesfmlstorage.h"
#include "store/gamexlostorage.h"
#include "sfml/battlemenu.h"
#include "sfml/awesprite.h"
#include "sfml/textbox.h"

enum class InputType : unsigned short {
    NONE,
    SELECT,
    CANCEL,
    UP,
    DOWN
};

int main() {

    // Storage initialization. All "storage" objects are loaded and initialized.

    AWE::GameLOVStorage lov;
    if (!lov.Initialize("res")) {
        std::cout << "List of values failed to initialize.\n";
        return 1;
    }

    AWE::ABRV physabrv('P', 'H', 'Y', 'S');
    AWE::DamageInclination_shptr phys = lov.GetDamageInclination(physabrv.AsLong());
    if (!phys) {
        std::cout << "PHYS damage inclination was not loaded!\n";
        return 1;
    }

    AWE::GameXLOStorage xlo;
    if (!xlo.Initialize(lov, phys, "res/data.xml")) {
        std::cout << "XML-loaded objects failed to initialize.\n";
        return 1;
    }

    AWE::GameBattleInfo battle(lov, xlo);
    AWE::GameSFMLStorage sfmls;
    if (sfmls.loadFailed()) {
        std::cout << "SFML-loaded objects failed to initialize.\n";
        return 1;
    }


    // Scene initialization. All scenes are created and inserted into the scene transitioner.

    AWE::GameSceneTransitioner scenes(sfmls);
    AWE::GameSceneInfo scene;

    scene = AWE::GameSceneInfo(AWE::GameTextureType::WOODS, AWE::AWEMusicInfo("fight2-export-comb.wav", 3300U, 61450U));
    scene.sprites()->push_back(AWE::GameTextureType::EPPLER);
    scene.sprites()->push_back(AWE::GameTextureType::REMI);
    scene.sprites()->push_back(AWE::GameTextureType::ENEMY);
    scenes.AddScene(scene);

    scene = AWE::GameSceneInfo(AWE::GameTextureType::TEMPLE, AWE::AWEMusicInfo("moments-peace-export.ogg", 3500U, 303900U, 1000U));
    scene.sprites()->push_back(AWE::GameTextureType::LEVELUP_EPPLER);
    scenes.AddScene(scene);

    scene = AWE::GameSceneInfo(AWE::GameTextureType::FIRETOWN, AWE::AWEMusicInfo("7heros-export.ogg", 26805U, 93074U));
    scene.sprites()->push_back(AWE::GameTextureType::EPPLER);
    scene.sprites()->push_back(AWE::GameTextureType::REMI);
    scene.sprites()->push_back(AWE::GameTextureType::ENEMY);
    scenes.AddScene(scene);


    // State machine initialization. State machine is configured for the beginning of the game, which due to a special state will proceed into the battle configuration automatically.

    sf::Sound currentSound;
    AWE::GameStateMachine states(xlo, scenes, battle, sfmls, currentSound);
    states.ConfigureForBeginning();


    // Window is initialized. Hey, it's my name!

    sf::RenderWindow window(sf::VideoMode(AWE::AWESprite::WIDTH_BACKGROUND, AWE::AWESprite::HEIGHT_BACKGROUND), "Matthew Cummings FIEA Portfolio Project", sf::Style::Titlebar | sf::Style::Close);
    window.setSize(sf::Vector2u(window.getSize().x * 4, window.getSize().y * 4));
 

    // Final helper variables created.

    InputType inputType = InputType::NONE;
    bool waitingForInput = true;
    bool anyInput = false;


    // Main game loop. Once this loop is exited, the game concludes.

    while (window.isOpen()) {


        // Standard SFML event polling.

        sf::Event ev;
        while (window.pollEvent(ev)) {
            switch (ev.type) {

            // The following state ends the game. The remaining loop will continue to process the game as normal, but because `window.isOpen()` will return false now, the game is as good as finished.
            // I could make this more clear by putting an `if (!window.isOpen()) { break; }` after the while loop, but it's cleaner if we just don't do that and the current behavior is harmless.

            case sf::Event::Closed:
                window.close();
                break;

            case sf::Event::KeyPressed:
                anyInput = true;

                if (waitingForInput) {
                    switch (ev.key.code) {

                    case sf::Keyboard::Z:
                    case sf::Keyboard::Space: // intentional fall-through
                    case sf::Keyboard::Enter: // intentional fall-through
                        inputType = InputType::SELECT;
                        waitingForInput = false;
                        break;

                    case sf::Keyboard::X:
                    case sf::Keyboard::Backspace: // intentional fall-through
                    case sf::Keyboard::Delete: // intentional fall-through
                        inputType = InputType::CANCEL;
                        waitingForInput = false;
                        break;

                    case sf::Keyboard::Up:
                    case sf::Keyboard::PageUp: // intentional fall-through
                    case sf::Keyboard::W: // intentional fall-through
                        inputType = InputType::UP;
                        waitingForInput = false;
                        break;

                    case sf::Keyboard::Down:
                    case sf::Keyboard::PageDown: // intentional fall-through
                    case sf::Keyboard::S: // intentional fall-through
                        inputType = InputType::DOWN;
                        waitingForInput = false;
                        break;

                    }
                    break;

                }
            }
        }


        // Ascertain the current state.

        AWE::GameState* currentState = states.current();


        // Certain states respond to any input. Handle such situations here.

        if (anyInput) {
            if (currentState->stateType() == AWE::GameStateType::BATTLE_DAMAGECALCULATION && currentState->step() == AWE::GameStateStep::PROCESSING) {
                auto calcState = states.GetState<AWE::GameState_Battle_DamageCalculation>(AWE::GameStateType::BATTLE_DAMAGECALCULATION);
                if (calcState) { calcState->Acknowledge(); }
            }

            if (currentState->stateType() == AWE::GameStateType::BATTLE_MONOLOGUE && currentState->step() == AWE::GameStateStep::PROCESSING) {
                auto monologueState = states.GetState<AWE::GameState_Battle_Monologue>(AWE::GameStateType::BATTLE_MONOLOGUE);
                if (monologueState) { monologueState->Skip(); }
            }

            if (currentState->stateType() == AWE::GameStateType::BATTLE_PLAYERLOST && currentState->step() == AWE::GameStateStep::PROCESSING) {
                auto lostState = states.GetState<AWE::GameState_Battle_PlayerLost>(AWE::GameStateType::BATTLE_PLAYERLOST);
                if (lostState) { lostState->Acknowledge(); }
            }

            anyInput = false;
        }


        // waitingForInput is set to false if any relevant input is received. Notably, this means that pressing an unrecognized key will NOT set waitingForInput to false,
        // while pressing any key (including an unrecognized one) WILL set anyInput to true.

        // If we didn't get any relevant input, we'll set inputType to NONE for this game loop iteration.

        if (waitingForInput) {
            inputType = InputType::NONE;
        }


        // We're done with the waitingForInput flag now that inputType has been set, so let's set waitingForInput back to true right away.

        waitingForInput = true;


        // Since the "menu" in the level up screen is really just a sprite without any behavior built into it, we pass input through the level up state itself.

        // Of course, we only retrieve the level up state if we're currently at that point in the game.

        AWE::GameState_LevelUp_Select* levelupState = nullptr;
        if (currentState->stateType() == AWE::GameStateType::LEVELUP_SELECT) {
            levelupState = states.GetState<AWE::GameState_LevelUp_Select>(AWE::GameStateType::LEVELUP_SELECT);
        }


        // Handle the input type here. Note if the input type is NONE, nothing happens.

        switch (inputType) {

        case InputType::SELECT:
            if (sfmls.battleMenu()->isVisible()) {
                sfmls.battleMenu()->Select();
            }

            if (levelupState) {
                levelupState->ConfirmSelection();
            }
            break;

        case InputType::UP:
            if (sfmls.battleMenu()->isVisible()) {
                sfmls.battleMenu()->DecrementSkill();
            }

            if (levelupState) {
                levelupState->DecrementMenu();
            }
            break;

        case InputType::DOWN:
            if (sfmls.battleMenu()->isVisible()) {
                sfmls.battleMenu()->IncrementSkill();
            }

            if (levelupState) {
                levelupState->IncrementMenu();
            }
            break;

        }


        // The CLOSE state type is not meant to be a normal state, but is instead a way for the state machine to indicate the game should conclude.

        if (currentState->stateType() != AWE::GameStateType::CLOSE) {


            // Yes, this does mean if we ever hit a STOP state, the game will just hang. This should never happen, and if it ever does it makes it easy to investigate what went wrong.

            if (currentState->stateType() != AWE::GameStateType::STOP) {
                states.Update();
            }


            // Very exciting and interesting draw commands. The order does matter, objects drawn later will appear on top of objects drawn earlier.

            window.clear();

            sfmls.DrawBackground(window, scenes.GetCurrent()->background());
            sfmls.DrawAWESprite(window, AWE::GameTextureType::ENEMY);
            sfmls.DrawAWESprite(window, AWE::GameTextureType::EPPLER);
            sfmls.DrawAWESprite(window, AWE::GameTextureType::REMI);
            sfmls.DrawAWESprite(window, AWE::GameTextureType::SKILL);
            sfmls.DrawAWESprite(window, AWE::GameTextureType::LEVELUP_EPPLER);
            sfmls.DrawAWESprite(window, AWE::GameTextureType::LEVELUP_REMI);
            sfmls.DrawTextBox(window, AWE::GameTextboxType::DAMAGE);
            sfmls.DrawTextBox(window, AWE::GameTextboxType::SKILL);
            sfmls.DrawTextBox(window, AWE::GameTextboxType::GENERIC);
            sfmls.DrawBattleMenu(window);
            window.draw(*sfmls.fadebox());

            window.display();
        } else {
            window.clear();
            window.close();
        }
    }


    // Thanks for playing!

    return 0;
}
