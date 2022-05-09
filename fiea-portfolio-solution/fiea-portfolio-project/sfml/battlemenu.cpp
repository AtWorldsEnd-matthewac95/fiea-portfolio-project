#include "battlemenu.h"
#include <iterator>
#include <sstream>

namespace AWE {

    /* BattleSelector */

    const float BattleSelector::SHAPE_OUTLINE_THICKNESS = 1.f;

    BattleSelector::BattleSelector() : AWEDrawable() { _rect.setOutlineThickness(SHAPE_OUTLINE_THICKNESS); _rect.setFillColor(sf::Color(0, 0, 0, 0)); }
    BattleSelector::BattleSelector(const sf::Color& color) : AWEDrawable() { _rect.setOutlineThickness(SHAPE_OUTLINE_THICKNESS); _rect.setFillColor(sf::Color(0, 0, 0, 0)); _rect.setOutlineColor(color); }

    const sf::RectangleShape& BattleSelector::rect() const { return _rect; }

    sf::Vector2f BattleSelector::SetRectSize(const sf::Vector2f& size) { sf::Vector2f oldval = _rect.getSize(); _rect.setSize(size); return oldval; }
    sf::Vector2f BattleSelector::SetRectPosition(const sf::Vector2f& position) { sf::Vector2f oldval = _rect.getPosition(); _rect.setPosition(position); return oldval; }
    sf::Color BattleSelector::SetRectColor(const sf::Color& color) { sf::Color oldval = _rect.getOutlineColor(); _rect.setOutlineColor(color); return oldval; }


    /* BattleMenu */

    BattleMenu::BattleMenu(
        const sf::Color& backgroundColor,
        unsigned int backgroundWidth,
        unsigned int backgroundHeight,
        unsigned int margin,
        const TextBoxVisuals& skillboxVisuals,
        const TextBoxVisuals& charboxVisuals,
        const sf::Color& skillSelectorColor,
        const sf::Color& charSelectorColor,
        const sf::SoundBuffer& cursorSound
    ) : BattleMenu(backgroundColor, backgroundWidth, backgroundHeight, margin, skillboxVisuals, charboxVisuals, skillSelectorColor, charSelectorColor) {
        _cursorSound.setBuffer(cursorSound);
    }

    BattleMenu::BattleMenu(
        const sf::Color& backgroundColor,
        unsigned int backgroundWidth,
        unsigned int backgroundHeight,
        unsigned int margin,
        const TextBoxVisuals& skillboxVisuals,
        const TextBoxVisuals& charboxVisuals,
        const sf::Color& skillSelectorColor,
        const sf::Color& charSelectorColor
    ) : AWEDrawable(), _margin(margin), _characters(nullptr), _skillTextBox(TextBox(skillboxVisuals)), _characterTextBox(TextBox(charboxVisuals)) {
        _isSelected = false;
        _isLocked = false;
        _cursorSound.setVolume(SELECT_SAVE_SOUND_VOLUME);

        _background.setFillColor(backgroundColor);
        _background.setSize(sf::Vector2f(backgroundWidth, backgroundHeight));

        _skillTextBox.alignment(TextAlignment::LEFT);
        _characterTextBox.alignment(TextAlignment::RIGHT);

        if (skillboxVisuals.margin < 1) {
            _skillTextBox.margin(margin);
        }

        if (charboxVisuals.margin < 1) {
            _characterTextBox.margin(margin);
        }

        unsigned int innerWidth = ((backgroundWidth - margin) / 2) - margin;
        unsigned int innerHeight = backgroundHeight - (margin * 2);
        _skillTextBox.fixedWidth(innerWidth - (_skillTextBox.margin() * 2));
        _skillTextBox.fixedHeight(innerHeight - (_skillTextBox.margin() * 2));
        _characterTextBox.fixedWidth(innerWidth - (_characterTextBox.margin() * 2));
        _characterTextBox.fixedHeight(innerHeight - (_characterTextBox.margin() * 2));

        _skillIndex = 0;
        _characterIndex = 0;

        _skillSelector.SetRectColor(skillSelectorColor);
        _skillSelector.SetRectSize(sf::Vector2f(_skillTextBox.fixedWidth(), skillboxVisuals.letterHeight));
        _characterSelector.SetRectColor(charSelectorColor);
        _characterSelector.SetRectSize(sf::Vector2f(_characterTextBox.fixedWidth(), charboxVisuals.letterHeight));

        RefreshText();
        SetPosition(_background.getPosition());
        RefreshSelectorPositions();
    }

    const float BattleMenu::SELECT_SAVE_SOUND_VOLUME = 40.f;

    std::string BattleMenu::PadSpaces(const std::string& str, unsigned int len) const {
        std::string spaces = "";

        unsigned int loop = len - str.length();
        for (unsigned int i = 0; i < loop; i++) {
            spaces += " ";
        }

        return spaces;
    }

    std::string BattleMenu::PadSpaces(const BattlerStatValue& num, unsigned int len) const {
        std::string spaces = "";

        unsigned int copy = static_cast<unsigned int>(num);
        do {
            len--;
            copy /= 10;
        } while (copy > 0);

        for (unsigned int i = 0; i < len; i++) {
            spaces += " ";
        }

        return spaces;
    }

    void BattleMenu::RefreshText() {
        if (!_characters || _characters->size() < 1) {
            _skillSelector.isVisible(false);
            _characterSelector.isVisible(false);
            return;
        }

        std::stringstream ss;
        for (const BattlerInstance_shptr& character : *_characters) {
            BattlerStatValue mxhp = character->GetStat(BattlerStat::MXHP.AsLong());
            ss << character->name() << PadSpaces(character->name(), 8) << ' ' << PadSpaces(character->hp(), 4) << character->hp() << '/' << mxhp << PadSpaces(mxhp, 4) << '\n';
        }
        _characterTextBox.SetString(ss.str());
        _characterSelector.isVisible(true);

        const SkillMap& skills = _characters->at(_characterIndex)->skills();

        if (skills.empty()) {
            _skillSelector.isVisible(false);
            return;
        }

        ss.str("");
        for (const SkillMap::value_type& skill : skills) {
            ss << skill.second->name() << "\n";
        }
        _skillTextBox.SetString(ss.str());
        _skillSelector.isVisible(true);
    }

    sf::Vector2f BattleMenu::SetPosition(const sf::Vector2f& position) {
        sf::Vector2f oldval = _background.getPosition();

        _background.setPosition(position);
        _skillTextBox.SetPosition(sf::Vector2f(position.x + _margin, position.y + _margin));
        _characterTextBox.SetPosition(sf::Vector2f(position.x + (2 * _margin) + _skillTextBox.box().getSize().x, position.y + _margin));

        RefreshSelectorPositions();

        return oldval;
    }

    void BattleMenu::RefreshSelectorPositions() {
        _skillSelector.SetRectPosition(sf::Vector2f(
            _skillTextBox.box().getPosition().x + _skillTextBox.margin(),
            _skillTextBox.box().getPosition().y + _skillTextBox.margin() + (_skillIndex * _skillTextBox.letterHeight())
        ));
        _characterSelector.SetRectPosition(sf::Vector2f(
            _characterTextBox.box().getPosition().x + _characterTextBox.margin(),
            _characterTextBox.box().getPosition().y + _characterTextBox.margin() + (_characterIndex * _characterTextBox.letterHeight())
        ));
    }

    bool BattleMenu::IncrementCharacterSingle(bool refreshTextAndSelectorPositions) {
        if (_isLocked) { return false; }

        if (_characters && _characters->size() > (_characterIndex + 1)) {
            _characterIndex++;
            _skillIndex = 0;
            if (refreshTextAndSelectorPositions) { RefreshText(); RefreshSelectorPositions(); }
            return true;
        }

        _characterIndex = _characters ? _characters->size() - 1 : 0;
        if (refreshTextAndSelectorPositions) { RefreshText(); RefreshSelectorPositions(); }
        return false;
    }

    bool BattleMenu::DecrementCharacterSingle(bool refreshTextAndSelectorPositions) {
        if (_isLocked) { return false; }

        if (_characters && _characterIndex > 0) {
            _characterIndex = (_characters->size() > _characterIndex ? _characterIndex : _characters->size()) - 1;
            _skillIndex = 0;
            if (refreshTextAndSelectorPositions) { RefreshText(); RefreshSelectorPositions(); }
            return true;
        }

        _characterIndex = 0;
        if (refreshTextAndSelectorPositions) { RefreshText(); RefreshSelectorPositions(); }
        return false;
    }

    bool BattleMenu::IncrementSkillSingle(bool refreshSelectorPositions) {
        if (_isLocked) { return false; }

        if (!_characters || _characters->empty() || _characterIndex >= _characters->size()) {
            _skillIndex = 0;
            if (refreshSelectorPositions) { RefreshSelectorPositions(); }
            return false;
        }

        const SkillMap& skills = _characters->at(_characterIndex)->skills();

        if (skills.size() > (_skillIndex + 1)) {
            _skillIndex++;
            if (refreshSelectorPositions) { RefreshSelectorPositions(); }
            return true;
        }

        _skillIndex = skills.empty() ? 0 : (skills.size() - 1);
        if (refreshSelectorPositions) { RefreshSelectorPositions(); }
        return false;
    }

    bool BattleMenu::DecrementSkillSingle(bool refreshSelectorPositions) {
        if (_isLocked) { return false; }

        if (!_characters || _characters->empty() || _characterIndex >= _characters->size()) {
            _skillIndex = 0;
            if (refreshSelectorPositions) { RefreshSelectorPositions(); }
            return false;
        }

        const SkillMap& skills = _characters->at(_characterIndex)->skills();

        if (_skillIndex > 0) {
            _skillIndex = (skills.size() > _skillIndex ? _skillIndex : skills.size()) - 1;
            if (refreshSelectorPositions) { RefreshSelectorPositions(); }
            return true;
        }

        _skillIndex = 0;
        if (refreshSelectorPositions) { RefreshSelectorPositions(); }
        return false;
    }

    const sf::RectangleShape& BattleMenu::background() const { return _background; }
    const TextBox& BattleMenu::skillTextBox() const { return _skillTextBox; }
    const TextBox& BattleMenu::characterTextBox() const { return _characterTextBox; }
    unsigned int BattleMenu::margin() const { return _margin; }
    const std::vector<BattlerInstance_shptr>* BattleMenu::characters() const { return _characters; }
    const BattleSelector& BattleMenu::skillSelector() const { return _skillSelector; }
    const BattleSelector& BattleMenu::characterSelector() const { return _characterSelector; }
    bool BattleMenu::isSelected() const { return _isSelected; }
    bool BattleMenu::isLocked() const { return _isLocked; }
    const sf::Sound& BattleMenu::cursorSound() const { return _cursorSound; }

    unsigned int BattleMenu::margin(unsigned int newval) { unsigned int oldval = _margin; _margin = newval; return oldval; }

    const std::vector<BattlerInstance_shptr>* BattleMenu::characters(const std::vector<BattlerInstance_shptr>& newval) {
        const std::vector<BattlerInstance_shptr>* oldval = _characters;
        _characters = &newval;
        _characterIndex = 0;
        _skillIndex = 0;
        RefreshText();
        RefreshSelectorPositions();
        return oldval;
    }

    Skill_shptr BattleMenu::GetCurrentSkill() const {
        Skill_shptr ptr;
        BattlerInstance_shptr character = GetCurrentCharacter();

        if (!character) {
            return ptr;
        }

        const SkillMap& skills = character->skills();

        if (!skills.empty() && _skillIndex < skills.size()) {
            SkillMap::const_iterator itr = skills.begin();
            std::advance(itr, _skillIndex);
            ptr = itr->second;
        }

        return ptr;
    }

    bool BattleMenu::GoToSkill(unsigned int index) {
        if (_isLocked) { return false; }

        BattlerInstance_shptr character = GetCurrentCharacter();

        if (!character || character->skills().empty()) {
            _skillIndex = 0;
            return false;
        }

        if (_cursorSound.getBuffer()) {
            _cursorSound.play();
        }

        size_t skillsize = character->skills().size();
        if (index < skillsize) {
            _skillIndex = index;
            RefreshSelectorPositions();
            return true;
        }

        _skillIndex = skillsize - 1;
        RefreshSelectorPositions();
        return false;
    }

    bool BattleMenu::IncrementSkill(unsigned int delta) {
        if (_isLocked) { return false; }

        bool loop = true;

        for (int i = 0; loop && i < delta; i++) {
            loop &= IncrementSkillSingle(false);
        }

        if (delta > 0) {
            if (_cursorSound.getBuffer()) {
                _cursorSound.play();
            }
            RefreshSelectorPositions();
        } else {
            loop = false;
        }

        return loop;
    }

    bool BattleMenu::DecrementSkill(unsigned int delta) {
        if (_isLocked) { return false; }

        bool loop = true;

        for (int i = 0; loop && i < delta; i++) {
            loop &= DecrementSkillSingle(false);
        }

        if (delta > 0) {
            if (_cursorSound.getBuffer()) {
                _cursorSound.play();
            }
            RefreshSelectorPositions();
        } else {
            loop = false;
        }

        return loop;
    }

    BattlerInstance_shptr BattleMenu::GetCurrentCharacter() const {
        BattlerInstance_shptr ptr;

        if (_characters && _characterIndex < _characters->size()) {
            ptr = _characters->at(_characterIndex);
        }

        return ptr;
    }

    bool BattleMenu::GoToCharacter(unsigned int index) {
        if (_isLocked) { return false; }

        if (_characters && index < _characters->size()) {
            _characterIndex = index;
            _skillIndex = 0;
            RefreshText();
            RefreshSelectorPositions();
            return true;
        }

        _characterIndex = _characters->size() - 1;
        RefreshSelectorPositions();
        return false;
    }

    bool BattleMenu::IncrementCharacter(unsigned int delta) {
        if (_isLocked) { return false; }

        bool loop = true;

        for (int i = 0; loop && i < delta; i++) {
            loop &= IncrementCharacterSingle(false);
        }

        if (delta > 0) {
            RefreshText();
            RefreshSelectorPositions();
        } else {
            loop = false;
        }

        return loop;
    }

    bool BattleMenu::DecrementCharacter(unsigned int delta) {
        if (_isLocked) { return false; }

        bool loop = true;

        for (int i = 0; loop && i < delta; i++) {
            loop &= DecrementCharacterSingle(false);
        }

        if (delta > 0) {
            RefreshText();
            RefreshSelectorPositions();
        } else {
            loop = false;
        }

        return loop;
    }

    void BattleMenu::Select() {
        if (_isVisible && _characterSelector.isVisible() && _skillSelector.isVisible()) {
            BattlerInstance_shptr character = GetCurrentCharacter();
            Skill_shptr skill = GetCurrentSkill();
            if (character && skill) {
                _isSelected = true;
                _isLocked = true;
            }
        }
    }

    void BattleMenu::ReleaseSelection() {
        _isSelected = false;
        _isLocked = false;
    }
}
