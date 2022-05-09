#pragma once
#include <memory>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "awedrawable.h"
#include "textbox.h"
#include "../models/battler.h"
#include "../models/skill.h"

namespace AWE {
    class BattleSelector : public AWEDrawable {
    private:
        sf::RectangleShape _rect;

    public:
        BattleSelector();
        BattleSelector(const sf::Color&);

        static const float SHAPE_OUTLINE_THICKNESS;

        const sf::RectangleShape& rect() const;

        sf::Vector2f SetRectSize(const sf::Vector2f&);
        sf::Vector2f SetRectPosition(const sf::Vector2f&);
        sf::Color SetRectColor(const sf::Color&);
    };

    class BattleMenu : public AWEDrawable {
    private:
        sf::RectangleShape _background;
        TextBox _skillTextBox;
        TextBox _characterTextBox;
        const std::vector<BattlerInstance_shptr>* _characters;
        unsigned int _skillIndex;
        unsigned int _characterIndex;
        unsigned int _margin;
        BattleSelector _skillSelector;
        BattleSelector _characterSelector;
        bool _isSelected;
        bool _isLocked;
        sf::Sound _cursorSound;

        std::string PadSpaces(const std::string& str, unsigned int len) const;
        std::string PadSpaces(const BattlerStatValue& num, unsigned int len) const;
        bool IncrementCharacterSingle(bool refreshTextAndSelectorPositions = true);
        bool DecrementCharacterSingle(bool refreshTextAndSelectorPositions = true);
        bool IncrementSkillSingle(bool refreshSelectorPositions = true);
        bool DecrementSkillSingle(bool refreshSelectorPositions = true);

    public:
        BattleMenu(
            const sf::Color& backgroundColor,
            unsigned int backgroundWidth,
            unsigned int backgroundHeight,
            unsigned int margin,
            const TextBoxVisuals& skillboxVisuals,
            const TextBoxVisuals& charboxVisuals,
            const sf::Color& skillSelectorColor,
            const sf::Color& charSelectorColor
        );
        BattleMenu(
            const sf::Color& backgroundColor,
            unsigned int backgroundWidth,
            unsigned int backgroundHeight,
            unsigned int margin,
            const TextBoxVisuals& skillboxVisuals,
            const TextBoxVisuals& charboxVisuals,
            const sf::Color& skillSelectorColor,
            const sf::Color& charSelectorColor,
            const sf::SoundBuffer& cursorSound
        );

        static const float SELECT_SAVE_SOUND_VOLUME;

        void RefreshText();
        void RefreshSelectorPositions();

        const sf::RectangleShape& background() const;
        const TextBox& skillTextBox() const;
        const TextBox& characterTextBox() const;
        unsigned int margin() const;
        const std::vector<BattlerInstance_shptr>* characters() const;
        const BattleSelector& skillSelector() const;
        const BattleSelector& characterSelector() const;
        bool isSelected() const;
        bool isLocked() const;
        const sf::Sound& cursorSound() const;

        unsigned int margin(unsigned int);
        const std::vector<BattlerInstance_shptr>* characters(const std::vector<BattlerInstance_shptr>&);

        sf::Vector2f SetPosition(const sf::Vector2f&);

        Skill_shptr GetCurrentSkill() const;
        bool GoToSkill(unsigned int);
        bool IncrementSkill(unsigned int delta = 1);
        bool DecrementSkill(unsigned int delta = 1);
        BattlerInstance_shptr GetCurrentCharacter() const;
        bool GoToCharacter(unsigned int);
        bool IncrementCharacter(unsigned int delta = 1);
        bool DecrementCharacter(unsigned int delta = 1);

        void Select();
        void ReleaseSelection();
    };
}
