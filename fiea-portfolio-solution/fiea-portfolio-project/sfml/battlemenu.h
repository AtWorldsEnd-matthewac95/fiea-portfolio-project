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
    /// <summary>
    /// Wraps a SFML rectangle shape instance. Used to indicate which position on the menu the player is currently selecting.
    /// </summary>
    class BattleSelector : public AWEDrawable {
    private:
        sf::RectangleShape _rect;

    public:
        BattleSelector();
        BattleSelector(const sf::Color&);

        /// <summary>
        /// Thickness in pixels of the shape outline.
        /// </summary>
        static const float SHAPE_OUTLINE_THICKNESS;

        /// <returns>const reference to the internal SFML RectangleShape element.</returns>
        const sf::RectangleShape& rect() const;

        /// <param name="">New size of the internal RectangleShape.</param>
        /// <returns>Old size of the internal RectangleShape.</returns>
        sf::Vector2f SetRectSize(const sf::Vector2f&);
        /// <param name="">New position of the internal RectangleShape.</param>
        /// <returns>Old position of the internal RectangleShape.</returns>
        sf::Vector2f SetRectPosition(const sf::Vector2f&);
        /// <param name="">New color of the internal RectangleShape.</param>
        /// <returns>Old color of the internal RectangleShape.</returns>
        sf::Color SetRectColor(const sf::Color&);
    };

    /// <summary>
    /// Made of several custom and SFML components. Comprises the menu seen at the bottom of the screen during a battle.
    /// </summary>
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

        /// <summary>
        /// Appends spaces to the end of the string until the resultant string is of the given size.
        /// </summary>
        /// <param name="str">String to append spaces to. Note this string itself is not modified, a copy with the modifications is returned.</param>
        /// <param name="len">Length of the resultant string.</param>
        /// <returns>Resultant string. Note if the given string's length was already equal to or greater than the given size, no changes will have been made.</returns>
        std::string PadSpaces(const std::string& str, unsigned int len) const;
        /// <summary>
        /// Appends spaces to the end of the number until the resultant string is of the given size.
        /// </summary>
        /// <param name="str">Number to append spaces to. Note this number itself is not modified, a string with the modifications is returned.</param>
        /// <param name="len">Length of the resultant string.</param>
        /// <returns>Resultant string. Note if the given number already had equal or more digits than the given size, no spaces will have been added.</returns>
        std::string PadSpaces(const BattlerStatValue& num, unsigned int len) const;
        /// <summary>
        /// Shift the focus of the menu to the next character. This will change the contents of the skill side of the menu and set the skill index to 0,
        /// unless the current character is the last in which case this will do nothing.
        /// </summary>
        /// <param name="refreshTextAndSelectorPositions">If true, updates the text and selector positions. Would only be false if the caller is handling this operation instead.</param>
        /// <returns>true if the character changes, false otherwise.</returns>
        bool IncrementCharacterSingle(bool refreshTextAndSelectorPositions = true);
        /// <summary>
        /// Shift the focus of the menu to the previous character. This will change the contents of the skill side of the menu and set the skill index to 0,
        /// unless the current character is the first in which case this will do nothing.
        /// </summary>
        /// <param name="refreshTextAndSelectorPositions">If true, updates the text and selector positions. Would only be false if the caller is handling this operation instead.</param>
        /// <returns>true if the character changes, false otherwise.</returns>
        bool DecrementCharacterSingle(bool refreshTextAndSelectorPositions = true);
        /// <summary>
        /// Shift the skill selector to the next skill, unless the current skill is the last in which case this will do nothing.
        /// </summary>
        /// <param name="refreshSelectorPositions">If true, updates the selector position. Would only be false if the caller is handling this operation instead.</param>
        /// <returns>true if the skill changes, false otherwise.</returns>
        bool IncrementSkillSingle(bool refreshSelectorPositions = true);
        /// <summary>
        /// Shift the skill selector to the previous skill, unless the current skill is the first in which case this will do nothing.
        /// </summary>
        /// <param name="refreshSelectorPositions">If true, updates the selector position. Would only be false if the caller is handling this operation instead.</param>
        /// <returns>true if the skill changes, false otherwise.</returns>
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

        /// <summary>
        /// Volume between 0.f and 100.f of the sounds this menu makes. Keep in mind that one volume level controls both sounds.
        /// </summary>
        static const float SELECT_SAVE_SOUND_VOLUME;

        /// <summary>
        /// Updates the text on the textboxes to be whatever their current value should be.
        /// </summary>
        void RefreshText();
        /// <summary>
        /// Updates the positions of the selectors to be whatever their current positions should be.
        /// </summary>
        void RefreshSelectorPositions();

        /// <returns>const reference to the background shape element.</returns>
        const sf::RectangleShape& background() const;
        /// <returns>const reference to the textbox which lists the skills of the current character.</returns>
        const TextBox& skillTextBox() const;
        /// <returns>const reference to the textbox which lists the characters.</returns>
        const TextBox& characterTextBox() const;
        /// <returns>Space in pixels between the two textboxes and between those textboxes and the edges of the menu.</returns>
        unsigned int margin() const;
        /// <returns>const pointer to the list of characters this menu tracks.</returns>
        const std::vector<BattlerInstance_shptr>* characters() const;
        /// <returns>const reference to the selector for skills.</returns>
        const BattleSelector& skillSelector() const;
        /// <returns>const reference to the selector for characters.</returns>
        const BattleSelector& characterSelector() const;
        /// <returns>Has the skill been selected?</returns>
        bool isSelected() const;
        /// <returns>If true, the positions of the selectors and the focus of the menu can't change.</returns>
        bool isLocked() const;
        /// <returns>const reference to the internal SFML Sound element.</returns>
        const sf::Sound& cursorSound() const;

        /// <param name="">New value for the margin.</param>
        /// <returns>Old value for the margin.</returns>
        unsigned int margin(unsigned int);
        /// <param name="">New value for the character list.</param>
        /// <returns>Old value of the character list as a pointer.</returns>
        const std::vector<BattlerInstance_shptr>* characters(const std::vector<BattlerInstance_shptr>&);

        /// <param name="">New position of the menu. This will update the position of all of the menu's child elements as well.</param>
        /// <returns>Old position of the menu.</returns>
        sf::Vector2f SetPosition(const sf::Vector2f&);

        /// <returns>shared_ptr to the skill the selector is currently over.</returns>
        Skill_shptr GetCurrentSkill() const;
        /// <summary>
        /// Attempts to go to the skill with the given index, or simply goes to the last skill if no skill with that index exists.
        /// </summary>
        /// <param name="">Index to go to.</param>
        /// <returns>True if a skill with the given index was found, false otherwise.</returns>
        bool GoToSkill(unsigned int);
        /// <summary>
        /// Shift the skill selector to a skill further down the list, unless the current skill is the last in which case this will do nothing.
        /// </summary>
        /// <param name="delta">How many times the skill should be incremented.</param>
        /// <returns>true if the skill selector successfully incremented the full amount, false otherwise.</returns>
        bool IncrementSkill(unsigned int delta = 1);
        /// <summary>
        /// Shift the skill selector to a skill back up the list, unless the current skill is the first in which case this will do nothing.
        /// </summary>
        /// <param name="delta">How many times the skill should be decremented.</param>
        /// <returns>true if the skill selector successfully decremented the full amount, false otherwise.</returns>
        bool DecrementSkill(unsigned int delta = 1);
        /// <returns>shared_ptr to the character the selector is currently over.</returns>
        BattlerInstance_shptr GetCurrentCharacter() const;
        /// <summary>
        /// Attempts to go to the character with the given index, or simply goes to the last character if no character with that index exists.
        /// If the character changes, the skill side of the menu will change and the skill index will be set to 0.
        /// </summary>
        /// <param name="">Index to go to.</param>
        /// <returns>True if a character with the given index was found, false otherwise.</returns>
        bool GoToCharacter(unsigned int);
        /// <summary>
        /// Shift the focus of the menu to a character further down the list. This will change the contents of the skill side of the menu and set the skill index to 0,
        /// unless the current character is the last in which case this will do nothing.
        /// </summary>
        /// <param name="delta">How many times the character should be incremented.</param>
        /// <returns>true if the character was successfully incremented the full amount, false otherwise.</returns>
        bool IncrementCharacter(unsigned int delta = 1);
        /// <summary>
        /// Shift the focus of the menu to a character back up the list. This will change the contents of the skill side of the menu and set the skill index to 0,
        /// unless the current character is the last in which case this will do nothing.
        /// </summary>
        /// <param name="delta">How many times the character should be decremented.</param>
        /// <returns>true if the character was successfully decremented the full amount, false otherwise.</returns>
        bool DecrementCharacter(unsigned int delta = 1);

        /// <summary>
        /// Designates the current selector positions as the player's selection. This only works if the menu and both selectors are visible, and if the current skill
        /// and character are both valid.
        /// </summary>
        void Select();
        /// <summary>
        /// Releases the lock on the selection. The current selector positions are no longer considered to be the player's selection.
        /// </summary>
        void ReleaseSelection();
    };
}
