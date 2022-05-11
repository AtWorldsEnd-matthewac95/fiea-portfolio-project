#pragma once
#include <unordered_map>
#include <vector>
#include <SFML/Audio.hpp>
#include "battlerdecision.h"
#include "gamebattleinfo.h"
#include "gamescene.h"
#include "../misc/damage.h"
#include "../sfml/battlemenu.h"
#include "../sfml/awesprite.h"
#include "../sfml/textbox.h"
#include "../store/gamesfmlstorage.h"

namespace AWE {
    enum class GameStateStep : unsigned short {
        DONE,
        BEGINNING,
        PROCESSING,
        ENDING
    };

    enum class GameStateTransition : unsigned short {
        FORWARD,
        BACKWARD,
        LOOP
    };

    enum class GameStateType : unsigned short {
        STOP,
        CLOSE,
        SCENE_FADEIN,
        SCENE_FADEOUT,
        SCENE_MUSIC,
        MACHINESCENE_TRANSITION,
        BATTLE_MONOLOGUE,
        BATTLE_AISKILLSELECT,
        BATTLE_PLAYERSKILLSELECT,
        BATTLE_SKILLNAME,
        BATTLE_SKILLANIMATION,
        BATTLE_DAMAGECALCULATION,
        BATTLE_PLAYERWON,
        BATTLE_PLAYERLOST,
        MACHINEBATTLE_PLAYERLOOP,
        MACHINEBATTLE_SKILLINIT,
        MACHINEBATTLE_SKILLLOOP,
        MACHINEBATTLE_BATTLELOOP,
        LEVELUP_LOADCHARACTERS,
        LEVELUP_SELECT,
        MACHINELEVELUP_SETSELECT,
        MACHINELEVELUP_ADVANCE,
        MACHINE
    };

    class GameState {
    protected:
        GameStateStep _step;

    public:
        GameState();

        virtual GameStateType stateType() const;
        GameStateStep step() const;
        bool isDone() const { return _step == GameStateStep::DONE; }

        virtual bool Begin();
        virtual bool Process();
        virtual bool End();

        bool Update() {
            switch (_step) {
            case GameStateStep::BEGINNING:
                return Begin();
                break;

            case GameStateStep::PROCESSING:
                return Process();
                break;

            case GameStateStep::ENDING:
                return End();
                break;
            }

            return true;
        }

        virtual bool ResetSteps();
    };


    /* ---- Derived ---- */


    /* Close */

    class GameState_Close : public GameState {
    public:
        GameStateType stateType() const override { return GameStateType::CLOSE; }
    };


    /* -- Scene -- */


    /* Fade In */

    class GameState_Scene_FadeIn : public GameState {
    private:
        sf::RectangleShape* _fadebox;
        short _speed;
        std::unique_ptr<sf::Clock> _timer;

    public:
        GameState_Scene_FadeIn();
        GameState_Scene_FadeIn(sf::RectangleShape&, short);

        static const unsigned long FADE_TICK;

        const sf::RectangleShape* fadebox() const { return _fadebox; }
        short speed() const { return _speed; }

        sf::RectangleShape* fadebox() { return _fadebox; }

        sf::RectangleShape* fadebox(sf::RectangleShape& newval) { sf::RectangleShape* oldval = _fadebox; _fadebox = &newval; return oldval; }
        short speed(short newval) { short oldval = _speed; _speed = newval; return oldval; }

        GameStateType stateType() const override { return GameStateType::SCENE_FADEIN; }

        bool Begin() override;
        bool Process() override;
        bool ResetSteps() override;
    };


    /* Fade Out */

    class GameState_Scene_FadeOut : public GameState {
    private:
        sf::RectangleShape* _fadebox;
        short _speed;
        std::unique_ptr<sf::Clock> _timer;

    public:
        GameState_Scene_FadeOut();
        GameState_Scene_FadeOut(sf::RectangleShape&, short);

        static const unsigned long FADE_TICK;

        const sf::RectangleShape* fadebox() const { return _fadebox; }
        short speed() const { return _speed; }

        sf::RectangleShape* fadebox() { return _fadebox; }

        sf::RectangleShape* fadebox(sf::RectangleShape& newval) { sf::RectangleShape* oldval = _fadebox; _fadebox = &newval; return oldval; }
        short speed(short newval) { short oldval = _speed; _speed = newval; return oldval; }

        GameStateType stateType() const override { return GameStateType::SCENE_FADEOUT; }

        bool Begin() override;
        bool Process() override;
        bool ResetSteps() override;
    };


    /* Music */

    class GameState_Scene_Music : public GameState {
    private:
        GameSFMLStorage* _sfmls;
        GameSceneTransitioner* _scenes;

    public:
        GameState_Scene_Music();
        GameState_Scene_Music(GameSFMLStorage&, GameSceneTransitioner&);

        const GameSFMLStorage* sfmls() const { return _sfmls; }
        const GameSceneTransitioner* scenes() const { return _scenes; }

        GameSFMLStorage* sfmls() { return _sfmls; }
        GameSFMLStorage* sfmls(GameSFMLStorage& newval) { GameSFMLStorage* oldval = _sfmls; _sfmls = &newval; return oldval; }
        GameSceneTransitioner* scenes() { return _scenes; }
        GameSceneTransitioner* scenes(GameSceneTransitioner& newval) { GameSceneTransitioner* oldval = _scenes; _scenes = &newval; return oldval; }

        GameStateType stateType() const override { return GameStateType::SCENE_MUSIC; }

        bool Begin() override;
        bool ResetSteps() override;
    };


    /* -- Battle -- */


    /* AI Skill Select */

    class GameState_Battle_AISkillSelect : public GameState {
    private:
        GameBattleInfo* _battle;

    public:
        GameState_Battle_AISkillSelect();
        GameState_Battle_AISkillSelect(GameBattleInfo&);

        const GameBattleInfo* battle() const;

        GameBattleInfo* battle(GameBattleInfo&);

        GameStateType stateType() const override { return GameStateType::BATTLE_AISKILLSELECT; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };


    /* Skill Name */

    class GameState_Battle_SkillName : public GameState {
    private:
        TextBox* _textbox;
        Skill_shptr _skill;
        std::unique_ptr<sf::Clock> _timer;

    public:
        GameState_Battle_SkillName();
        GameState_Battle_SkillName(TextBox&);

        static const unsigned int DEFAULT_DISPLAY_MILLIS;

        const TextBox* textbox() const;
        const Skill_shptr& skill() const;
        const std::unique_ptr<sf::Clock>& timer() const;

        TextBox* textbox(TextBox&);
        Skill_shptr skill(Skill_shptr);

        GameStateType stateType() const override { return GameStateType::BATTLE_SKILLNAME; }

        bool Begin() override;
        bool Process() override;
        bool ResetSteps() override;
    };


    /* Damage Calculation */

    class GameState_Battle_DamageCalculation : public GameState {
    private:
        const DamageCalculator* _calc;
        std::unique_ptr<Damage> _result;
        AWESprite* _targetsprite;
        TextBox* _damagetext;
        TextBox* _prompttext;
        TextBox* _skilltext;
        const BattlerDecision* _decision;
        std::unique_ptr<sf::Clock> _timer;
        bool _isAcknowledged;

    public:
        GameState_Battle_DamageCalculation();
        GameState_Battle_DamageCalculation(const DamageCalculator&, TextBox& damagetext);
        GameState_Battle_DamageCalculation(const DamageCalculator&, TextBox& damagetext, TextBox& prompttext);

        static const unsigned int DEFAULT_PROMPT_WAIT_MILLIS;
        static const std::string DEFAULT_PROMPT_MESSAGE;

        const DamageCalculator* calc() const;
        const std::unique_ptr<Damage>& result() const;
        const AWESprite* targetsprite() const;
        const TextBox* damagetext() const;
        const TextBox* prompttext() const;
        const TextBox* skilltext() const;
        const BattlerDecision* decision() const;
        const std::unique_ptr<sf::Clock>& timer() const;
        bool isAcknowledged() const;

        const DamageCalculator* calc(const DamageCalculator&);
        AWESprite* targetsprite(AWESprite&);
        TextBox* damagetext(TextBox&);
        TextBox* prompttext(TextBox&);
        TextBox* skilltext(TextBox&);
        const BattlerDecision* decision(const BattlerDecision&);

        GameStateType stateType() const override { return GameStateType::BATTLE_DAMAGECALCULATION; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;

        bool Acknowledge();
    };


    /* Skill Animation */

    class GameState_Battle_SkillAnimation : public GameState {
    private:
        sf::Sound* _sound;
        TextBox* _skilltext;
        AWESprite* _skillsprite;
        AWESprite* _targetsprite;

    public:
        GameState_Battle_SkillAnimation();
        GameState_Battle_SkillAnimation(sf::Sound&, AWESprite& skill);

        const sf::Sound* sound() const;
        const TextBox* skilltext() const;
        const AWESprite* skillsprite() const;
        const AWESprite* targetsprite() const;

        sf::Sound* sound();
        AWESprite* skillsprite();

        sf::Sound* sound(sf::Sound&);
        TextBox* skilltext(TextBox&);
        AWESprite* skillsprite(AWESprite&);
        AWESprite* targetsprite(AWESprite&);

        GameStateType stateType() const override { return GameStateType::BATTLE_SKILLANIMATION; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };


    /* Player Skill Select */

    class GameState_Battle_PlayerSkillSelect : public GameState {
    private:
        BattleMenu* _menu;
        int _characterIndex;
        GameBattleInfo* _battle;
        std::unique_ptr<sf::Sound> _selectSound;

    public:
        GameState_Battle_PlayerSkillSelect();
        GameState_Battle_PlayerSkillSelect(BattleMenu&, GameBattleInfo&);
        GameState_Battle_PlayerSkillSelect(BattleMenu&, GameBattleInfo&, const sf::SoundBuffer&);

        const BattleMenu* menu() const;
        int characterIndex() const;
        const GameBattleInfo* battle() const;
        const sf::Sound* selectSound() const;

        BattleMenu* menu(BattleMenu&);
        int characterIndex(int);
        GameBattleInfo* battle(GameBattleInfo&);
        void selectSound(const sf::SoundBuffer&);

        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERSKILLSELECT; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };


    /* Monologue */

    class GameState_Battle_Monologue : public GameState {
    private:
        bool _thomas;
        TextBox* _prompt;
        std::unique_ptr<sf::Clock> _timer;
        bool _isReadyToSkip;

        void Reset();

    public:
        GameState_Battle_Monologue();
        GameState_Battle_Monologue(TextBox&);

        static const std::string SKIP_STRING;

        bool thomas() const;
        const TextBox* prompt() const;
        bool isReadyToSkip() const;

        bool thomas(bool);
        TextBox* prompt(TextBox&);
        bool isReadyToSkip(bool);

        bool Skip();

        GameStateType stateType() const override { return GameStateType::BATTLE_MONOLOGUE; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };


    /* Player Won */

    class GameState_Battle_PlayerWon : public GameState {
    private:
        AWESprite* _enemySprite;
        std::unique_ptr<sf::Clock> _timer;
        std::unique_ptr<sf::Sound> _defeatSound;
        sf::Color _fadeColor;
        sf::Color _originalColor;
        unsigned short _speed;

    public:
        GameState_Battle_PlayerWon();
        GameState_Battle_PlayerWon(AWESprite&, const sf::SoundBuffer&, sf::Color, unsigned short);

        static const unsigned long FADE_TICK;

        const AWESprite* enemySprite() const;
        const sf::Sound* defeatSound() const;
        const sf::Color& fadeColor() const;
        unsigned short speed() const;

        AWESprite* enemySprite(AWESprite&);
        void defeatSound(const sf::SoundBuffer&);
        sf::Color fadeColor(sf::Color);
        unsigned short speed(unsigned short);

        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERWON; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };


    /* Player Lost */

    class GameState_Battle_PlayerLost : public GameState {
    private:
        bool _isAcknowledged;
        TextBox* _prompt;

        void Reset();

    public:
        GameState_Battle_PlayerLost();
        GameState_Battle_PlayerLost(TextBox&);

        const TextBox* prompt() const;
        bool isAcknowledged() const;

        TextBox* prompt(TextBox&);

        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERLOST; }

        bool Acknowledge();

        bool Begin() override;
        bool Process() override;
        bool ResetSteps() override;
    };


    /* -- Level Up -- */


    /* Load Characters */

    // Forward declaration, defined in gamestatemachine.h
    class GameState_MachineLevelUp_SetSelect;

    class GameState_LevelUp_LoadCharacters : public GameState {
    private:
        GameXLOStorage* _xlo;
        std::unordered_map<GameTextureType, Battler_shptr> _characters;

        bool RefreshCharacters();

        friend class GameState_MachineLevelUp_SetSelect;

    public:
        GameState_LevelUp_LoadCharacters();
        GameState_LevelUp_LoadCharacters(GameXLOStorage&);

        const GameXLOStorage* xlo() const;
        const std::unordered_map<GameTextureType, Battler_shptr>& characters() const;

        GameXLOStorage* xlo(GameXLOStorage&);

        GameStateType stateType() const override { return GameStateType::LEVELUP_LOADCHARACTERS; }

        bool Begin() override;
    };


    /* Select */

    class GameState_LevelUp_Select : public GameState {
    private:
        AWESprite* _menu;
        Battler_shptr _battler;
        bool _isSelectionConfirmed;
        std::unique_ptr<sf::Sound> _cursorSound;
        std::unique_ptr<sf::Sound> _selectSound;

        bool ExtractElement(SkillElementGroupKey, DamageTypeKey, ElementalAffinityKey& output);

    public:
        GameState_LevelUp_Select();
        GameState_LevelUp_Select(AWESprite& menu);
        GameState_LevelUp_Select(AWESprite& menu, const sf::SoundBuffer& cursor, const sf::SoundBuffer& select);

        static const unsigned int MAX_MENU_INDEX;

        const AWESprite* menu() const;
        const Battler_shptr& battler() const;
        bool isSelectionConfirmed() const;
        const sf::Sound* cursorSound() const;
        const sf::Sound* selectSound() const;

        AWESprite* menu();

        AWESprite* menu(AWESprite&);
        Battler_shptr battler(Battler_shptr);
        void cursorSound(const sf::SoundBuffer&);
        void selectSound(const sf::SoundBuffer&);

        GameStateType stateType() const override { return GameStateType::LEVELUP_SELECT; }

        bool IncrementMenu();
        bool DecrementMenu();
        bool ConfirmSelection();
        bool ReleaseSelection();

        bool Begin() override;
        bool Process() override;
        bool End() override;
        bool ResetSteps() override;
    };
}
