#include "gamestate.h"
#include <stdlib.h>
#include <ctime>
#include "../store/gamexlostorage.h"
#include "../store/gamesfmlstorage.h"

namespace AWE {
    GameState::GameState() : _step(GameStateStep::BEGINNING) {}

    GameStateType GameState::stateType() const { return GameStateType::STOP; }
    GameStateStep GameState::step() const { return _step; }

    bool GameState::Begin() {
        if (_step == GameStateStep::BEGINNING) {
            _step = GameStateStep::PROCESSING;
        }
        return true;
    }

    bool GameState::Process() {
        if (_step == GameStateStep::PROCESSING) {
            _step = GameStateStep::ENDING;
        }
        return true;
    }

    bool GameState::End() {
        if (_step == GameStateStep::ENDING) {
            _step = GameStateStep::DONE;
        }
        return true;
    }

    bool GameState::ResetSteps() {
        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* ---- Derived ---- */


    /* -- Scene -- */


    /* Fade In */

    GameState_Scene_FadeIn::GameState_Scene_FadeIn() : GameState(), _fadebox(nullptr), _speed(0) {}
    GameState_Scene_FadeIn::GameState_Scene_FadeIn(sf::RectangleShape& fadebox, short speed) : GameState(), _fadebox(&fadebox), _speed(speed) {}

    const unsigned long GameState_Scene_FadeIn::FADE_TICK = 31250UL;

    bool GameState_Scene_FadeIn::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_fadebox) {
            _timer.reset();
            return false;
        }

        sf::Color color = _fadebox->getFillColor();
        _fadebox->setFillColor(sf::Color(color.r, color.g, color.b, 255U));
        _timer = std::make_unique<sf::Clock>(sf::Clock());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Scene_FadeIn::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_fadebox || _speed <= 0) {
            _timer.reset();
            return false;
        }

        if (!_timer) {
            _timer = std::make_unique<sf::Clock>(sf::Clock());
        }

        if (_timer->getElapsedTime().asMicroseconds() >= FADE_TICK) {
            _timer->restart();
            sf::Color color = _fadebox->getFillColor();
            short alpha = (_speed > color.a) ? 0 : (color.a - _speed);
            _fadebox->setFillColor(sf::Color(color.r, color.g, color.b, alpha));
        }

        if (_fadebox->getFillColor().a <= 0) {
            _timer.reset();
            _step = GameStateStep::DONE;
        }
        return true;
    }

    bool GameState_Scene_FadeIn::ResetSteps() { _timer.reset(); _step = GameStateStep::BEGINNING; return true; }


    /* Fade Out */

    GameState_Scene_FadeOut::GameState_Scene_FadeOut() : GameState(), _fadebox(nullptr), _speed(0) {}
    GameState_Scene_FadeOut::GameState_Scene_FadeOut(sf::RectangleShape& fadebox, short speed) : GameState(), _fadebox(&fadebox), _speed(speed) {}

    const unsigned long GameState_Scene_FadeOut::FADE_TICK = 31250UL;

    bool GameState_Scene_FadeOut::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_fadebox) {
            _timer.reset();
            return false;
        }

        sf::Color color = _fadebox->getFillColor();
        _fadebox->setFillColor(sf::Color(color.r, color.g, color.b, 0U));
        _timer = std::make_unique<sf::Clock>(sf::Clock());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Scene_FadeOut::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_fadebox || _speed <= 0) {
            return false;
        }

        if (!_timer) {
            _timer = std::make_unique<sf::Clock>(sf::Clock());
        }

        if (_timer->getElapsedTime().asMicroseconds() >= FADE_TICK) {
            _timer->restart();
            sf::Color color = _fadebox->getFillColor();
            short alpha = ((color.a + static_cast<unsigned int>(_speed)) > 255U) ? 255U : (color.a + _speed);
            _fadebox->setFillColor(sf::Color(color.r, color.g, color.b, alpha));
        }

        if (_fadebox->getFillColor().a >= 255U) {
            _timer.reset();
            _step = GameStateStep::DONE;
        }
        return true;
    }

    bool GameState_Scene_FadeOut::ResetSteps() { _timer.reset(); _step = GameStateStep::BEGINNING; return true; }


    /* Music */

    GameState_Scene_Music::GameState_Scene_Music() : GameState(), _sfmls(nullptr), _scenes(nullptr) {}
    GameState_Scene_Music::GameState_Scene_Music(GameSFMLStorage& sfmls, GameSceneTransitioner& scenes) : GameState(), _sfmls(&sfmls), _scenes(&scenes) {}

    bool GameState_Scene_Music::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_sfmls || !_scenes || !_scenes->GetCurrent()) {
            return false;
        }

        AWEMusicInfo music = _scenes->GetCurrent()->music();

        _sfmls->StopMusic();
        if (_sfmls->PlayMusic(music.filename(), music.loopstart(), music.loopdur(), music.startoffset())) {
            _step = GameStateStep::DONE;
            return true;
        }

        return false;
    }

    bool GameState_Scene_Music::ResetSteps() {
        if (!_sfmls) {
            return false;
        }

        _sfmls->StopMusic();
        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* -- Battle -- */


    /* AI Skill Select */

    GameState_Battle_AISkillSelect::GameState_Battle_AISkillSelect() : GameState(), _battle(nullptr) {}
    GameState_Battle_AISkillSelect::GameState_Battle_AISkillSelect(GameBattleInfo& battle) : GameState(), _battle(&battle) {}

    const GameBattleInfo* GameState_Battle_AISkillSelect::battle() const { return _battle; }

    GameBattleInfo* GameState_Battle_AISkillSelect::battle(GameBattleInfo& newval) { GameBattleInfo* oldval = _battle; _battle = &newval; return oldval; }

    bool GameState_Battle_AISkillSelect::ResetSteps() {
        if (!_battle) {
            return false;
        }

        RemoveBattlerDecision(*_battle->decisions(), _battle->enemy());
        _step = GameStateStep::BEGINNING;
        return true;
    }

    bool GameState_Battle_AISkillSelect::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_battle) {
            return false;
        }

        RemoveBattlerDecision(*_battle->decisions(), _battle->enemy());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_AISkillSelect::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_battle) {
            return false;
        }

        BattlerInstance_shptr enemy = _battle->enemy();
        BattlerDecisionOrdering* decisions = _battle->decisions();

        BattlerDecisionOrdering::iterator orderitr;
        BattlerDecisionList::iterator _;
        bool foundOrderitr = false, foundDecision = false;

        FindBattlerDecision(*decisions, enemy, foundOrderitr, orderitr, foundDecision, _);

        if (foundDecision || enemy->skills().empty()) {
            _step = GameStateStep::DONE;
            return true;
        }

        if (!foundOrderitr) {
            orderitr = decisions->insert(decisions->begin(), std::make_pair(enemy->priority(), std::vector<BattlerDecision>()));
        }

        srand(time(nullptr));
        auto skill = enemy->skills().begin();
        std::advance(skill, (rand() % enemy->skills().size()));
        auto size = _battle->characters().size();
        auto target = rand() % size;
        short delta = 1 - (2 * (rand() % 2));

        while (_battle->characters().at(target)->hp() <= 0) {
            target = (target + delta) % size;
        }

        orderitr->second.push_back(BattlerDecision(skill->second, enemy, _battle->characters().at(target)));

        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_AISkillSelect::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (!_battle) {
            _step = GameStateStep::PROCESSING;
            return false;
        }

        bool foundOrderitr = false, foundDecision = false;

        FindBattlerDecision(*_battle->decisions(), _battle->enemy(), foundOrderitr, foundDecision);
        if (foundOrderitr && foundDecision) {
            _step = GameStateStep::DONE;
            return true;
        }

        _step = GameStateStep::PROCESSING;
        return false;
    }


    /* Skill Name */

    GameState_Battle_SkillName::GameState_Battle_SkillName() : GameState(), _textbox(nullptr) {}
    GameState_Battle_SkillName::GameState_Battle_SkillName(TextBox& textbox) : GameState(), _textbox(&textbox) {}

    const unsigned int GameState_Battle_SkillName::DEFAULT_DISPLAY_MILLIS = 1000;

    const TextBox* GameState_Battle_SkillName::textbox() const { return _textbox; }
    const Skill_shptr& GameState_Battle_SkillName::skill() const { return _skill; }
    const std::unique_ptr<sf::Clock>& GameState_Battle_SkillName::timer() const { return _timer; }

    TextBox* GameState_Battle_SkillName::textbox(TextBox& newval) { TextBox* oldval = _textbox; _textbox = &newval; return oldval; }
    Skill_shptr GameState_Battle_SkillName::skill(Skill_shptr newval) { Skill_shptr oldval = std::move(_skill); _skill = std::move(newval); return oldval; }

    bool GameState_Battle_SkillName::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_textbox || !_skill) {
            return false;
        }

        _textbox->SetString(_skill->name());
        _textbox->isVisible(true);
        _timer = std::make_unique<sf::Clock>(sf::Clock());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_SkillName::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_timer) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (_timer->getElapsedTime().asMilliseconds() >= DEFAULT_DISPLAY_MILLIS) {
            _step = GameStateStep::DONE;
            _timer.reset();
        }

        return true;
    }

    bool GameState_Battle_SkillName::ResetSteps() {
        if (_textbox) {
            _textbox->SetString("");
            _textbox->isVisible(false);
            _timer.reset();
            _step = GameStateStep::BEGINNING;
            return true;
        }
        return false;
    }


    /* Damage Calculation */

    GameState_Battle_DamageCalculation::GameState_Battle_DamageCalculation()
        : GameState(), _isAcknowledged(false), _calc(nullptr), _targetsprite(nullptr), _damagetext(nullptr), _prompttext(nullptr), _skilltext(nullptr), _decision(nullptr) {}
    GameState_Battle_DamageCalculation::GameState_Battle_DamageCalculation(const DamageCalculator& calc, TextBox& damagetext)
        : GameState(), _isAcknowledged(false), _calc(&calc), _targetsprite(nullptr), _damagetext(&damagetext), _prompttext(nullptr), _skilltext(nullptr), _decision(nullptr) {}
    GameState_Battle_DamageCalculation::GameState_Battle_DamageCalculation(const DamageCalculator& calc, TextBox& damagetext, TextBox& prompttext)
        : GameState(), _isAcknowledged(false), _calc(&calc), _targetsprite(nullptr), _damagetext(&damagetext), _prompttext(&prompttext), _skilltext(nullptr), _decision(nullptr) {}

    const unsigned int GameState_Battle_DamageCalculation::DEFAULT_PROMPT_WAIT_MILLIS = 2500;
    const std::string GameState_Battle_DamageCalculation::DEFAULT_PROMPT_MESSAGE = "PRESS ANY\nKEY TO\nPROCEED";

    const DamageCalculator* GameState_Battle_DamageCalculation::calc() const { return _calc; }
    const std::unique_ptr<Damage>& GameState_Battle_DamageCalculation::result() const { return _result; }
    const AWESprite* GameState_Battle_DamageCalculation::targetsprite() const { return _targetsprite; }
    const TextBox* GameState_Battle_DamageCalculation::damagetext() const { return _damagetext; }
    const TextBox* GameState_Battle_DamageCalculation::prompttext() const { return _prompttext; }
    const TextBox* GameState_Battle_DamageCalculation::skilltext() const { return _skilltext; }
    const BattlerDecision* GameState_Battle_DamageCalculation::decision() const { return _decision; }
    const std::unique_ptr<sf::Clock>& GameState_Battle_DamageCalculation::timer() const { return _timer; }
    bool GameState_Battle_DamageCalculation::isAcknowledged() const { return _isAcknowledged; }

    const DamageCalculator* GameState_Battle_DamageCalculation::calc(const DamageCalculator& newval) { const DamageCalculator* oldval = _calc; _calc = &newval; return oldval; }
    AWESprite* GameState_Battle_DamageCalculation::targetsprite(AWESprite& newval) { AWESprite* oldval = _targetsprite; _targetsprite = &newval; return oldval; }
    TextBox* GameState_Battle_DamageCalculation::damagetext(TextBox& newval) { TextBox* oldval = _damagetext; _damagetext = &newval; return oldval; }
    TextBox* GameState_Battle_DamageCalculation::prompttext(TextBox& newval) { TextBox* oldval = _prompttext; _prompttext = &newval; return oldval; }
    TextBox* GameState_Battle_DamageCalculation::skilltext(TextBox& newval) { TextBox* oldval = _skilltext; _skilltext = &newval; return oldval; }
    const BattlerDecision* GameState_Battle_DamageCalculation::decision(const BattlerDecision& newval) { const BattlerDecision* oldval = _decision; _decision = &newval; return oldval; }

    bool GameState_Battle_DamageCalculation::Acknowledge() {
        if (_isAcknowledged) {
            return true;
        }

        if (_step != GameStateStep::PROCESSING) {
            return false;
        }

        _isAcknowledged = true;
        return true;
    }

    bool GameState_Battle_DamageCalculation::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_calc || !_targetsprite || !_damagetext || !_decision) {
            return false;
        }

        _result = std::make_unique<Damage>(_calc->CalculateDamage(_decision->skill(), _decision->source(), _decision->target()));
        _damagetext->SetString(_result->ToString(_calc->lov()->damageTypes()));
        _damagetext->SetPosition(_targetsprite->GetSkillPosition());
        _damagetext->isVisible(true);
        _isAcknowledged = false;
        _timer = std::make_unique<sf::Clock>(sf::Clock());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_DamageCalculation::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (_isAcknowledged) {
            _step = GameStateStep::ENDING;
            return true;
        }

        if (_timer && _timer->getElapsedTime().asMilliseconds() >= DEFAULT_PROMPT_WAIT_MILLIS) {
            _timer.reset();

            if (_prompttext) {
                _prompttext->SetString(DEFAULT_PROMPT_MESSAGE);
                _prompttext->isVisible(true);
            }
        }

        return true;
    }

    bool GameState_Battle_DamageCalculation::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (!_damagetext) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (_skilltext) {
            _skilltext->isVisible(false);
        }

        if (_prompttext) {
            _prompttext->isVisible(false);
        }

        _timer.reset();
        _damagetext->isVisible(false);
        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_DamageCalculation::ResetSteps() {
        _timer.reset();

        if (!_damagetext) {
            return false;
        }

        if (_prompttext) {
            _prompttext->isVisible(false);
        }

        _result.reset();
        _damagetext->isVisible(false);
        _isAcknowledged = false;
        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* Skill Animation */

    GameState_Battle_SkillAnimation::GameState_Battle_SkillAnimation() : GameState(), _sound(nullptr), _skilltext(nullptr), _skillsprite(nullptr), _targetsprite(nullptr) {}
    GameState_Battle_SkillAnimation::GameState_Battle_SkillAnimation(sf::Sound& sound, AWESprite& skillsprite)
        : GameState(), _sound(&sound), _skilltext(nullptr), _skillsprite(&skillsprite), _targetsprite(nullptr) {}

    const sf::Sound* GameState_Battle_SkillAnimation::sound() const { return _sound; }
    const TextBox* GameState_Battle_SkillAnimation::skilltext() const { return _skilltext; }
    const AWESprite* GameState_Battle_SkillAnimation::skillsprite() const { return _skillsprite; }
    const AWESprite* GameState_Battle_SkillAnimation::targetsprite() const { return _targetsprite; }
    sf::Sound* GameState_Battle_SkillAnimation::sound() { return _sound; }
    AWESprite* GameState_Battle_SkillAnimation::skillsprite() { return _skillsprite; }

    sf::Sound* GameState_Battle_SkillAnimation::sound(sf::Sound& newval) { sf::Sound* oldval = _sound; _sound = &newval; return oldval; }
    TextBox* GameState_Battle_SkillAnimation::skilltext(TextBox& newval) { TextBox* oldval = _skilltext; _skilltext = &newval; return oldval; }
    AWESprite* GameState_Battle_SkillAnimation::skillsprite(AWESprite& newval) { AWESprite* oldval = _skillsprite; _skillsprite = &newval; return oldval; }
    AWESprite* GameState_Battle_SkillAnimation::targetsprite(AWESprite& newval) { AWESprite* oldval = _targetsprite; _targetsprite = &newval; return oldval; }

    bool GameState_Battle_SkillAnimation::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_skillsprite) {
            _step = GameStateStep::ENDING;
            return true;
        }

        if (!_sound) {
            _step = GameStateStep::ENDING;
            return true;
        }

        if (_sound->getLoop() || !_targetsprite) {
            return false;
        }

        sf::Vector2f skillpos = _targetsprite->GetSkillPosition();
        _skillsprite->SetPosition(skillpos.x, skillpos.y);
        _skillsprite->isVisible(true);
        _sound->play();
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_SkillAnimation::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_sound) {
            _step = GameStateStep::ENDING;
            return true;
        }

        if (_sound->getLoop()) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (_sound->getStatus() == sf::SoundStream::Stopped) {
            _step = GameStateStep::ENDING;
        }

        return true;
    }

    bool GameState_Battle_SkillAnimation::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (_skilltext) {
            _skilltext->isVisible(false);
        }

        if (_sound) {
            _sound->stop();
        }

        if (_skillsprite) {
            _skillsprite->isVisible(false);
        }

        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_SkillAnimation::ResetSteps() {
        if (_skilltext) {
            _skilltext->isVisible(false);
        }

        if (_sound) {
            _sound->stop();
        }

        if (_skillsprite) {
            _skillsprite->isVisible(false);
        }

        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* Player Skill Select */

    GameState_Battle_PlayerSkillSelect::GameState_Battle_PlayerSkillSelect() : GameState(), _menu(nullptr), _characterIndex(-1), _battle(nullptr) {}
    GameState_Battle_PlayerSkillSelect::GameState_Battle_PlayerSkillSelect(BattleMenu& menu, GameBattleInfo& battle) : GameState(), _menu(&menu), _characterIndex(-1), _battle(&battle) {}
    GameState_Battle_PlayerSkillSelect::GameState_Battle_PlayerSkillSelect(BattleMenu& menu, GameBattleInfo& battle, const sf::SoundBuffer& select)
            : GameState(), _menu(&menu), _characterIndex(-1), _battle(&battle) {
        _selectSound = std::make_unique<sf::Sound>(sf::Sound());
        _selectSound->setBuffer(select);
    }

    const BattleMenu* GameState_Battle_PlayerSkillSelect::menu() const { return _menu; }
    int GameState_Battle_PlayerSkillSelect::characterIndex() const { return _characterIndex; }
    const GameBattleInfo* GameState_Battle_PlayerSkillSelect::battle() const { return _battle; }
    const sf::Sound* GameState_Battle_PlayerSkillSelect::selectSound() const { return _selectSound.get(); }

    BattleMenu* GameState_Battle_PlayerSkillSelect::menu(BattleMenu& newval) { BattleMenu* oldval = _menu; _menu = &newval; return oldval; }
    int GameState_Battle_PlayerSkillSelect::characterIndex(int newval) { int oldval = _characterIndex; _characterIndex = newval; return oldval; }
    GameBattleInfo* GameState_Battle_PlayerSkillSelect::battle(GameBattleInfo& newval) { GameBattleInfo* oldval = _battle; _battle = &newval; return oldval; }
    void GameState_Battle_PlayerSkillSelect::selectSound(const sf::SoundBuffer& newbuffer) {
        if (!_selectSound) {
            _selectSound = std::make_unique<sf::Sound>(sf::Sound());
        }
        _selectSound->setBuffer(newbuffer);
    }

    bool GameState_Battle_PlayerSkillSelect::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_menu) {
            return false;
        }

        _menu->isVisible(true);

        if (_selectSound) { _selectSound->setVolume(BattleMenu::SELECT_SAVE_SOUND_VOLUME); }

        if (_characterIndex >= 0) {
            _menu->GoToCharacter(static_cast<unsigned int>(_characterIndex));
        }

        BattlerInstance_shptr character = _menu->GetCurrentCharacter();
        if (!character) {
            return false;
        }

        if (character->hp() <= 0) {
            _menu->isVisible(false);
            _step = GameStateStep::DONE;
            return true;
        }

        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_PlayerSkillSelect::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_menu) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        if (_menu->isLocked() && _menu->isSelected()) {
            if (!_battle) {
                return false;
            }

            BattlerDecisionOrdering* decisions = _battle->decisions();
            BattlerInstance_shptr source = _menu->GetCurrentCharacter();
            BattlerDecisionOrdering::iterator orderitr;
            BattlerDecisionList::iterator decision;
            bool foundOrderitr, foundDecision;

            FindBattlerDecision(*decisions, source, foundOrderitr, orderitr, foundDecision, decision);

            if (!foundOrderitr) {
                orderitr = decisions->insert(decisions->begin(), std::make_pair(source->priority(), BattlerDecisionList()));
            }

            if (foundDecision) {
                orderitr->second.erase(decision);
            }

            orderitr->second.push_back(BattlerDecision(_menu->GetCurrentSkill(), std::move(source), _battle->enemy()));
            _menu->ReleaseSelection();

            if (_selectSound && _selectSound->getBuffer()) {
                _selectSound->play();
            }

            _step = GameStateStep::ENDING;
        }

        return true;
    }

    bool GameState_Battle_PlayerSkillSelect::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (!_menu) {
            return false;
        }

        _menu->isVisible(false);
        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_PlayerSkillSelect::ResetSteps() {
        if (!_menu || !_battle) {
            return false;
        }

        _menu->ReleaseSelection();
        _menu->isVisible(false);

        if (_characterIndex >= 0 && _menu->GoToCharacter(_characterIndex)) {
            RemoveBattlerDecision(*_battle->decisions(), _menu->GetCurrentCharacter());
        }

        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* Monologue */

    GameState_Battle_Monologue::GameState_Battle_Monologue() : GameState(), _thomas(false), _prompt(nullptr) {}
    GameState_Battle_Monologue::GameState_Battle_Monologue(TextBox& prompt) : GameState(), _thomas(false), _prompt(&prompt) {}

    bool GameState_Battle_Monologue::thomas() const { return _thomas; }
    const TextBox* GameState_Battle_Monologue::prompt() const { return _prompt; }

    bool GameState_Battle_Monologue::thomas(bool newval) { bool oldval = _thomas; _thomas = newval; return oldval; }
    TextBox* GameState_Battle_Monologue::prompt(TextBox& newval) { TextBox* oldval = _prompt; _prompt = &newval; return oldval; }

    void GameState_Battle_Monologue::Reset() {
        _timer.reset();

        if (_prompt) {
            _prompt->isVisible(false);
            _prompt->fixedWidth(0U);
            _prompt->fixedHeight(0U);
            _prompt->alignment(TextAlignment::CENTER);
            _prompt->SetString("");
        }
    }

    bool GameState_Battle_Monologue::Skip() {
        if (_step != GameStateStep::PROCESSING) {
            return false;
        }

        _step = GameStateStep::ENDING;
        return true;
    }

    bool GameState_Battle_Monologue::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_thomas) {
            _step = GameStateStep::DONE;
            return true;
        }

        if (!_prompt) {
            Reset();
            return false;
        }

        _prompt->fixedWidth(_prompt->letterWidth() * 20);
        _prompt->fixedHeight(_prompt->letterHeight() * 7);
        _prompt->alignment(TextAlignment::LEFT);
        _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"");
        _prompt->isVisible(true);
        _timer = std::make_unique<sf::Clock>(sf::Clock());
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_Monologue::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_timer) {
            _timer = std::make_unique<sf::Clock>(sf::Clock());
        }

        if (_timer->getElapsedTime().asMilliseconds() > 21000U) {
            _step = GameStateStep::ENDING;
            return true;
        }

        if (_timer->getElapsedTime().asMilliseconds() > 0U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"Long I have fought");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 3000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"Long I have fought\n to honor Guardiana.");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 6000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"Long I have fought\n to honor Guardiana.\n Now, she is lost");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 9000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"Long I have fought\n to honor Guardiana.\n Now, she is lost\n to corrupted men.");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 12000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"I cannot abide her");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 15000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"I cannot abide her\n continued disgrace.");
        }
        if (_timer->getElapsedTime().asMilliseconds() > 18000U) {
            _prompt->SetString("press any to skip\n\nLORD THOMAS:\n\"I cannot abide her\n continued disgrace.\n Engarde!\"");
        }
    }

    bool GameState_Battle_Monologue::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        Reset();
        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_Monologue::ResetSteps() { Reset(); _step = GameStateStep::BEGINNING; return true; }


    /* Player Won */

    GameState_Battle_PlayerWon::GameState_Battle_PlayerWon() : GameState(), _enemySprite(nullptr), _speed(0u) {}
    GameState_Battle_PlayerWon::GameState_Battle_PlayerWon(AWESprite& enemy, const sf::SoundBuffer& enemyDefeat, sf::Color fadeColor, unsigned short speed)
            : GameState(), _enemySprite(&enemy), _fadeColor(fadeColor), _speed(speed) {
        _defeatSound = std::make_unique<sf::Sound>(sf::Sound());
        _defeatSound->setBuffer(enemyDefeat);
    }

    const unsigned long GameState_Battle_PlayerWon::FADE_TICK = 31250UL;

    const AWESprite* GameState_Battle_PlayerWon::enemySprite() const { return _enemySprite; }
    const sf::Sound* GameState_Battle_PlayerWon::defeatSound() const { return _defeatSound.get(); }
    const sf::Color& GameState_Battle_PlayerWon::fadeColor() const { return _fadeColor; }
    unsigned short GameState_Battle_PlayerWon::speed() const { return _speed; }

    AWESprite* GameState_Battle_PlayerWon::enemySprite(AWESprite& newval) { AWESprite* oldval = _enemySprite; _enemySprite = &newval; return oldval; }
    sf::Color GameState_Battle_PlayerWon::fadeColor(sf::Color newval) { sf::Color oldval = _fadeColor; _fadeColor = newval; return oldval; }
    unsigned short GameState_Battle_PlayerWon::speed(unsigned short newval) { unsigned short oldval = _speed; _speed = newval; return oldval; }
    void GameState_Battle_PlayerWon::defeatSound(const sf::SoundBuffer& newval) {
        if (!_defeatSound) {
            _defeatSound = std::make_unique<sf::Sound>(sf::Sound());
        }
        _defeatSound->setBuffer(newval);
    }

    bool GameState_Battle_PlayerWon::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_enemySprite) {
            return false;
        }

        _originalColor = _enemySprite->SetSpriteColor(_fadeColor);
        _timer = std::make_unique<sf::Clock>(sf::Clock());

        if (_defeatSound && _defeatSound->getBuffer()) {
            _defeatSound->play();
        }

        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_PlayerWon::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (!_enemySprite || _speed <= 0u) {
            return false;
        }

        if (!_timer) {
            _timer = std::make_unique<sf::Clock>(sf::Clock());
        }

        if (_timer->getElapsedTime().asMicroseconds() >= FADE_TICK) {
            _timer->restart();
            sf::Color color = _enemySprite->sprite().getColor();
            short alpha = (_speed > color.a) ? 0 : (color.a - _speed);
            _enemySprite->SetSpriteColor(sf::Color(color.r, color.g, color.b, alpha));
        }

        if (_enemySprite->sprite().getColor().a <= 0U) {
            _timer.reset();
            _step = GameStateStep::ENDING;
        }

        return true;
    }

    bool GameState_Battle_PlayerWon::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (!_enemySprite) {
            return false;
        }

        _timer.reset();
        _enemySprite->isVisible(false);
        _enemySprite->SetSpriteColor(_originalColor);
        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_Battle_PlayerWon::ResetSteps() {
        if (!_enemySprite) {
            return false;
        }

        _enemySprite->SetSpriteColor(_originalColor.a > 0u ? _originalColor : sf::Color::White);
        _enemySprite->isVisible(true);
        _timer.reset();
        _step = GameStateStep::BEGINNING;
        return true;
    }


    /* Player Lost */

    GameState_Battle_PlayerLost::GameState_Battle_PlayerLost() : GameState(), _prompt(nullptr), _isAcknowledged(false) {}
    GameState_Battle_PlayerLost::GameState_Battle_PlayerLost(TextBox& prompt) : GameState(), _prompt(&prompt), _isAcknowledged(false) {}

    bool GameState_Battle_PlayerLost::isAcknowledged() const { return _isAcknowledged; }
    const TextBox* GameState_Battle_PlayerLost::prompt() const { return _prompt; }
    TextBox* GameState_Battle_PlayerLost::prompt(TextBox& newval) { TextBox* oldval = _prompt; _prompt = &newval; return oldval; }

    void GameState_Battle_PlayerLost::Reset() {
        _isAcknowledged = false;

        if (_prompt) {
            _prompt->isVisible(false);
            _prompt->alignment(TextAlignment::CENTER);
        }
    }

    bool GameState_Battle_PlayerLost::Acknowledge() {
        if (_step != GameStateStep::PROCESSING) {
            return false;
        }

        _isAcknowledged = true;
        return _isAcknowledged;
    }

    bool GameState_Battle_PlayerLost::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_prompt) {
            return false;
        }

        _prompt->alignment(TextAlignment::LEFT);
        _prompt->SetString("YOU LOST\n\nPress any\nkey to exit.");
        _prompt->isVisible(true);
        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_Battle_PlayerLost::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (_isAcknowledged) {
            if (!_prompt) {
                return false;
            }

            Reset();
            _step = GameStateStep::DONE;
        }

        return true;
    }

    bool GameState_Battle_PlayerLost::ResetSteps() { Reset(); _step = GameStateStep::BEGINNING; return true; }


    /* -- Level Up -- */


    /* Load Characters */

    GameState_LevelUp_LoadCharacters::GameState_LevelUp_LoadCharacters() : GameState(), _xlo(nullptr) {}
    GameState_LevelUp_LoadCharacters::GameState_LevelUp_LoadCharacters(GameXLOStorage& xlo) : GameState(), _xlo(&xlo) {}

    const GameXLOStorage* GameState_LevelUp_LoadCharacters::xlo() const { return _xlo; }
    const std::unordered_map<GameTextureType, Battler_shptr>& GameState_LevelUp_LoadCharacters::characters() const { return _characters; }

    GameXLOStorage* GameState_LevelUp_LoadCharacters::xlo(GameXLOStorage& newval) { GameXLOStorage* oldval = _xlo; _xlo = &newval; return oldval; }

    bool GameState_LevelUp_LoadCharacters::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (RefreshCharacters()) {
            _step = GameStateStep::DONE;
        }
        return true;
    }

    bool GameState_LevelUp_LoadCharacters::RefreshCharacters() {
        if (!_xlo) {
            return false;
        }

        const BattlerMap& battlers = _xlo->battlers();
        for (const BattlerMap::value_type& battler : battlers) {
            if (battler.second->isCharacter()) {
                if (battler.second->textureType() == static_cast<unsigned int>(GameTextureType::EPPLER)) {
                    _characters.insert(std::make_pair(GameTextureType::LEVELUP_EPPLER, _xlo->GetBattler(battler.first)));
                } else if (battler.second->textureType() == static_cast<unsigned int>(GameTextureType::REMI)) {
                    _characters.insert(std::make_pair(GameTextureType::LEVELUP_REMI, _xlo->GetBattler(battler.first)));
                }
            }
        }

        return true;
    }


    /* Select */

    GameState_LevelUp_Select::GameState_LevelUp_Select() : GameState(), _menu(nullptr), _isSelectionConfirmed(false) {}
    GameState_LevelUp_Select::GameState_LevelUp_Select(AWESprite& menu) : GameState(), _menu(&menu), _isSelectionConfirmed(false) {}
    GameState_LevelUp_Select::GameState_LevelUp_Select(AWESprite& menu, const sf::SoundBuffer& cursor, const sf::SoundBuffer& select) : GameState(), _menu(&menu), _isSelectionConfirmed(false) {
        _cursorSound = std::make_unique<sf::Sound>(sf::Sound());
        _cursorSound->setBuffer(cursor);
        _selectSound = std::make_unique<sf::Sound>(sf::Sound());
        _selectSound->setBuffer(select);
    }

    const unsigned int GameState_LevelUp_Select::MAX_MENU_INDEX = 2U;

    const AWESprite* GameState_LevelUp_Select::menu() const { return _menu; }
    const Battler_shptr& GameState_LevelUp_Select::battler() const { return _battler; }
    bool GameState_LevelUp_Select::isSelectionConfirmed() const { return _isSelectionConfirmed; }
    const sf::Sound* GameState_LevelUp_Select::cursorSound() const { return _cursorSound.get(); }
    const sf::Sound* GameState_LevelUp_Select::selectSound() const { return _selectSound.get(); }

    AWESprite* GameState_LevelUp_Select::menu() { return _menu; }

    AWESprite* GameState_LevelUp_Select::menu(AWESprite& newval) { AWESprite* oldval = _menu; _menu = &newval; return oldval; }
    Battler_shptr GameState_LevelUp_Select::battler(Battler_shptr newval) { Battler_shptr oldval = std::move(_battler); _battler = std::move(newval); return oldval; }
    void GameState_LevelUp_Select::cursorSound(const sf::SoundBuffer& newbuffer) {
        if (!_cursorSound) {
            _cursorSound = std::make_unique<sf::Sound>(sf::Sound());
        }
        _cursorSound->setBuffer(newbuffer);
    }
    void GameState_LevelUp_Select::selectSound(const sf::SoundBuffer& newbuffer) {
        if (!_selectSound) {
            _selectSound = std::make_unique<sf::Sound>(sf::Sound());
        }
        _selectSound->setBuffer(newbuffer);
    }

    bool GameState_LevelUp_Select::IncrementMenu() {
        if (_step != GameStateStep::PROCESSING || _isSelectionConfirmed || !_menu) {
            return false;
        }

        unsigned int tx = _menu->textureIndex();
        _menu->textureIndex((tx < MAX_MENU_INDEX) ? (tx + 1) : MAX_MENU_INDEX);

        if (_cursorSound && _cursorSound->getBuffer()) {
            _cursorSound->play();
        }

        return true;
    }

    bool GameState_LevelUp_Select::DecrementMenu() {
        if (_step != GameStateStep::PROCESSING || _isSelectionConfirmed || !_menu) {
            return false;
        }

        unsigned int tx = _menu->textureIndex();
        _menu->textureIndex((tx > 0) ? (tx - 1) : 0);

        if (_cursorSound && _cursorSound->getBuffer()) {
            _cursorSound->play();
        }

        return true;
    }

    bool GameState_LevelUp_Select::ConfirmSelection() {
        if (_step != GameStateStep::PROCESSING || _isSelectionConfirmed || !_menu || !_menu->isVisible()) {
            return false;
        }

        _isSelectionConfirmed = true;
        return true;
    }

    bool GameState_LevelUp_Select::ReleaseSelection() {
        if (_step == GameStateStep::ENDING) {
            return false;
        }

        _isSelectionConfirmed = false;
        return true;
    }

    bool GameState_LevelUp_Select::Begin() {
        if (_step != GameStateStep::BEGINNING) {
            return true;
        }

        if (!_menu) {
            return false;
        }

        _isSelectionConfirmed = false;
        _menu->textureIndex(0U);
        _menu->isVisible(true);

        if (_cursorSound) { _cursorSound->setVolume(BattleMenu::SELECT_SAVE_SOUND_VOLUME); }
        if (_selectSound) { _selectSound->setVolume(BattleMenu::SELECT_SAVE_SOUND_VOLUME); }

        _step = GameStateStep::PROCESSING;
        return true;
    }

    bool GameState_LevelUp_Select::Process() {
        if (_step != GameStateStep::PROCESSING) {
            return true;
        }

        if (_isSelectionConfirmed) {
            _step = GameStateStep::ENDING;
        }
        return true;
    }

    bool GameState_LevelUp_Select::ExtractElement(SkillElementGroupKey groupkey, DamageTypeKey dtypekey, ElementalAffinityKey& output) {
        if (!_battler) {
            return false;
        }

        ABRV weapon('W', 'E', 'P', 'N');
        const Equipment_shptr& equipment = _battler->currentEquipment().GetEquipment(weapon.AsLong(), 1);

        if (!equipment) {
            return false;
        }

        const SkillElement_shptr& element = equipment->GetConversion(groupkey);
        if (element) {
            output = std::make_pair(element->abrvlong(), dtypekey);
            return true;
        }

        return false;
    }

    bool GameState_LevelUp_Select::End() {
        if (_step != GameStateStep::ENDING) {
            return true;
        }

        if (!_menu || !_menu->isVisible() || !_battler || !_isSelectionConfirmed) {
            _step = GameStateStep::BEGINNING;
            return false;
        }

        int hpIncrease = 150;
        ElementalAffinityKey affinkey;
        ABRV maxhp('M', 'X', 'H', 'P');
        ABRV piercing('P', 'I', 'E', 'R');
        if (_battler->textureType() == static_cast<unsigned int>(GameTextureType::EPPLER)) {
            ABRV strength('S', 'T', 'R', 'N');
            ABRV finesse('F', 'I', 'N', 'S');
            ABRV pyretic('F', 'I', 'R', 'E');
            ABRV slashing('S', 'L', 'S', 'H');
            ABRV swords('S', 'W', 'D', 'S');

            switch (_menu->textureIndex()) {
            case 0:
                if (ExtractElement(swords.AsLong(), pyretic.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(finesse.AsLong(), 10);
                    _battler->AdjustAffinity(affinkey, 20);
                }
                break;

            case 1:
                if (ExtractElement(swords.AsLong(), slashing.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(strength.AsLong(), 5);
                    _battler->AdjustAffinity(affinkey, 10);
                }
                break;

            case 2:
                if (ExtractElement(swords.AsLong(), piercing.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(strength.AsLong(), 5);
                    _battler->AdjustStat(finesse.AsLong(), 5);
                    _battler->AdjustAffinity(affinkey, 5);
                }
                break;
            }
        } else if (_battler->textureType() == static_cast<unsigned int>(GameTextureType::REMI)) {
            ABRV mysticism('M', 'Y', 'S', 'T');
            ABRV will('W', 'I', 'L', 'L');
            ABRV aetheric('A', 'E', 'T', 'H');
            ABRV arcane('A', 'R', 'C', 'N');
            ABRV aestival('A', 'S', 'V', 'L');
            ABRV wands('W', 'A', 'N', 'S');

            switch (_menu->textureIndex()) {
            case 0:
                if (ExtractElement(aestival.AsLong(), aetheric.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(will.AsLong(), 10);
                    _battler->AdjustAffinity(affinkey, 20);
                }
                break;

            case 1:
                if (ExtractElement(wands.AsLong(), arcane.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(mysticism.AsLong(), 10);
                    _battler->AdjustAffinity(affinkey, 20);
                }
                break;

            case 2:
                if (ExtractElement(aestival.AsLong(), piercing.AsLong(), affinkey)) {
                    _battler->AdjustStat(maxhp.AsLong(), hpIncrease);
                    _battler->AdjustStat(will.AsLong(), 7);
                    _battler->AdjustStat(mysticism.AsLong(), 7);
                    _battler->AdjustAffinity(affinkey, 10);
                }
                break;
            }
        }

        _menu->isVisible(false);
        _isSelectionConfirmed = false;

        if (_selectSound && _selectSound->getBuffer()) {
            _selectSound->play();
        }

        _step = GameStateStep::DONE;
        return true;
    }

    bool GameState_LevelUp_Select::ResetSteps() {
        if (!ReleaseSelection()) {
            return false;
        }

        _step = GameStateStep::BEGINNING;
        return true;
    }
}
