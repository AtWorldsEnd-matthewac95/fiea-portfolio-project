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

    sf::Sound currentSound;
    AWE::GameStateMachine states(xlo, scenes, battle, sfmls, currentSound);
    states.ConfigureForBeginning();

    sf::RenderWindow window(sf::VideoMode(AWE::AWESprite::WIDTH_BACKGROUND, AWE::AWESprite::HEIGHT_BACKGROUND), "Matthew Cummings FIEA Portfolio Project", sf::Style::Titlebar | sf::Style::Close);
    window.setSize(sf::Vector2u(window.getSize().x * 4, window.getSize().y * 4));
 
    InputType inputType = InputType::NONE;
    bool waitingForInput = true;
    bool anyInput = false;

    while (window.isOpen()) {
        sf::Event ev;
        while (window.pollEvent(ev)) {
            switch (ev.type) {

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

        AWE::GameState* currentState = states.current();

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

        if (waitingForInput) {
            inputType = InputType::NONE;
        }

        waitingForInput = true;

        AWE::GameState_LevelUp_Select* levelupState = nullptr;
        if (currentState->stateType() == AWE::GameStateType::LEVELUP_SELECT) {
            levelupState = states.GetState<AWE::GameState_LevelUp_Select>(AWE::GameStateType::LEVELUP_SELECT);
        }

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

        if (currentState->stateType() != AWE::GameStateType::CLOSE) {
            if (currentState->stateType() != AWE::GameStateType::STOP) {
                states.Update();
            }

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

    return 0;
}
