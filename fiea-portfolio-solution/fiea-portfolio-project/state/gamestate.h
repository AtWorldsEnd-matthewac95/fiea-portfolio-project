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
    /// <summary>
    /// Enumerates all the possible steps a game state can be in.
    /// </summary>
    enum class GameStateStep : unsigned short {
        DONE,
        BEGINNING,
        PROCESSING,
        ENDING
    };

    /// <summary>
    /// Enumerates all the possible types of game states. Each of these types should have a corresponding class implementing the behavior.
    /// Not all states are defined in the same file.
    /// </summary>
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

    /// <summary>
    /// Superclass for all game states. Implements very basic functionality for all steps, meant to be overriden. The basic procedure is to call Update, which is
    /// in turn hard-coded to call the correct step function, and then afterwards check if the state's step is equal to DONE (or to call the isDone function). If
    /// the state is done, then it's time to transition to the next state.
    /// </summary>
    class GameState {
    protected:
        GameStateStep _step;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState();

        /// <returns>State type.</returns>
        virtual GameStateType stateType() const;
        /// <returns>Current step.</returns>
        GameStateStep step() const;
        /// <returns>Is this state done with its work?</returns>
        bool isDone() const { return _step == GameStateStep::DONE; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        virtual bool Begin();
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        virtual bool Process();
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        virtual bool End();

        /// <summary>
        /// Calls the appropriate behavior based on the current step.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
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

        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        virtual bool ResetSteps();
    };


    /* ---- Derived ---- */


    /* Close */

    /// <summary>
    /// State meant to represent that the screen should be closed, concluding the game. No actual behavior is defined in this state.
    /// </summary>
    class GameState_Close : public GameState {
    public:
        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::CLOSE; }
    };


    /* -- Scene -- */


    /* Fade In */

    /// <summary>
    /// State used to make the game fade into view, normally from a black screen.
    /// </summary>
    class GameState_Scene_FadeIn : public GameState {
    private:
        sf::RectangleShape* _fadebox;
        short _speed;
        std::unique_ptr<sf::Clock> _timer;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Scene_FadeIn();
        GameState_Scene_FadeIn(sf::RectangleShape&, short);

        /// <summary>
        /// Time in microseconds between each increment of the fade box transperancy.
        /// </summary>
        static const unsigned long FADE_TICK;

        const sf::RectangleShape* fadebox() const { return _fadebox; }
        /// <returns>Amount of transperancy the fadebox gains each "tick" (See FADE_TICK). Note transperancy is a value between 0U and 255U.</returns>
        short speed() const { return _speed; }

        sf::RectangleShape* fadebox() { return _fadebox; }

        sf::RectangleShape* fadebox(sf::RectangleShape& newval) { sf::RectangleShape* oldval = _fadebox; _fadebox = &newval; return oldval; }
        short speed(short newval) { short oldval = _speed; _speed = newval; return oldval; }

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::SCENE_FADEIN; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Fade Out */

    /// <summary>
    /// State used to make the game fade into view, normally to a black screen.
    /// </summary>
    class GameState_Scene_FadeOut : public GameState {
    private:
        sf::RectangleShape* _fadebox;
        short _speed;
        std::unique_ptr<sf::Clock> _timer;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Scene_FadeOut();
        GameState_Scene_FadeOut(sf::RectangleShape&, short);

        /// <summary>
        /// Time in microseconds between each decrement of the fade box transperancy.
        /// </summary>
        static const unsigned long FADE_TICK;

        const sf::RectangleShape* fadebox() const { return _fadebox; }
        /// <returns>Amount of transperancy the fadebox loses each "tick" (See FADE_TICK). Note transperancy is a value between 0U and 255U.</returns>
        short speed() const { return _speed; }

        sf::RectangleShape* fadebox() { return _fadebox; }

        sf::RectangleShape* fadebox(sf::RectangleShape& newval) { sf::RectangleShape* oldval = _fadebox; _fadebox = &newval; return oldval; }
        short speed(short newval) { short oldval = _speed; _speed = newval; return oldval; }

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::SCENE_FADEOUT; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Music */

    /// <summary>
    /// State responsible for starting the music in the scene. Reseting this state will stop the music.
    /// </summary>
    class GameState_Scene_Music : public GameState {
    private:
        GameSFMLStorage* _sfmls;
        GameSceneTransitioner* _scenes;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Scene_Music();
        GameState_Scene_Music(GameSFMLStorage&, GameSceneTransitioner&);

        const GameSFMLStorage* sfmls() const { return _sfmls; }
        const GameSceneTransitioner* scenes() const { return _scenes; }

        GameSFMLStorage* sfmls() { return _sfmls; }
        GameSFMLStorage* sfmls(GameSFMLStorage& newval) { GameSFMLStorage* oldval = _sfmls; _sfmls = &newval; return oldval; }
        GameSceneTransitioner* scenes() { return _scenes; }
        GameSceneTransitioner* scenes(GameSceneTransitioner& newval) { GameSceneTransitioner* oldval = _scenes; _scenes = &newval; return oldval; }

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::SCENE_MUSIC; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* -- Battle -- */


    /* AI Skill Select */

    /// <summary>
    /// State responsible for selecting the AI's skill and target.
    /// </summary>
    class GameState_Battle_AISkillSelect : public GameState {
    private:
        GameBattleInfo* _battle;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_AISkillSelect();
        GameState_Battle_AISkillSelect(GameBattleInfo&);

        const GameBattleInfo* battle() const;

        GameBattleInfo* battle(GameBattleInfo&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_AISKILLSELECT; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Skill Name */

    /// <summary>
    /// State responsible for displaying the current skill's name to the player.
    /// </summary>
    class GameState_Battle_SkillName : public GameState {
    private:
        TextBox* _textbox;
        Skill_shptr _skill;
        std::unique_ptr<sf::Clock> _timer;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_SkillName();
        GameState_Battle_SkillName(TextBox&);

        /// <summary>
        /// Amount of time this state will wait before finishing.
        /// </summary>
        static const unsigned int DEFAULT_DISPLAY_MILLIS;

        const TextBox* textbox() const;
        const Skill_shptr& skill() const;
        const std::unique_ptr<sf::Clock>& timer() const;

        TextBox* textbox(TextBox&);
        Skill_shptr skill(Skill_shptr);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_SKILLNAME; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Damage Calculation */

    /// <summary>
    /// State responsible for calculating damage as a part of battler decision resolution.
    /// </summary>
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
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_DamageCalculation();
        GameState_Battle_DamageCalculation(const DamageCalculator&, TextBox& damagetext);
        GameState_Battle_DamageCalculation(const DamageCalculator&, TextBox& damagetext, TextBox& prompttext);

        /// <summary>
        /// Amount of time in milliseconds the state will wait before displaying the input prompt.
        /// </summary>
        static const unsigned int DEFAULT_PROMPT_WAIT_MILLIS;
        /// <summary>
        /// Message displayed by the input prompt. Should inform the player how to dismiss the damage display text.
        /// </summary>
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

        /// <summary>
        /// When the damage display text is being displayed, the player can press any key to dismiss it. This function is used to indicate the player has initiated this process.
        /// </summary>
        /// <returns>true if successful, false otherwise. If false is returned, the state may not be in the correct step for acknowledgement.</returns>
        bool Acknowledge();

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_DAMAGECALCULATION; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Skill Animation */

    /// <summary>
    /// State responsible for displaying the "animation" of the skill. For simplicity's sake, this is simply a still image coupled with a sound.
    /// </summary>
    class GameState_Battle_SkillAnimation : public GameState {
    private:
        sf::Sound* _sound;
        TextBox* _skilltext;
        AWESprite* _skillsprite;
        AWESprite* _targetsprite;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
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

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_SKILLANIMATION; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Player Skill Select */

    /// <summary>
    /// State responsible for stewarding the game through the player skill selection process.
    /// </summary>
    class GameState_Battle_PlayerSkillSelect : public GameState {
    private:
        BattleMenu* _menu;
        int _characterIndex;
        GameBattleInfo* _battle;
        std::unique_ptr<sf::Sound> _selectSound;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
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

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERSKILLSELECT; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Monologue */

    /// <summary>
    /// State responsible for displaying a character's monologue. Currently there's only one monologue in the game, and that's of Lord Thomas, the 2nd enemy.
    /// </summary>
    class GameState_Battle_Monologue : public GameState {
    private:
        bool _thomas;
        TextBox* _prompt;
        std::unique_ptr<sf::Clock> _timer;
        bool _isReadyToSkip;

        void Reset();

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_Monologue();
        GameState_Battle_Monologue(TextBox&);

        /// <summary>
        /// String used to inform the player the monologue can be skipped.
        /// </summary>
        static const std::string SKIP_STRING;

        bool thomas() const;
        const TextBox* prompt() const;
        bool isReadyToSkip() const;

        bool thomas(bool);
        TextBox* prompt(TextBox&);
        bool isReadyToSkip(bool);

        /// <summary>
        /// Used to indicate the player may want to skip the dialogue.
        /// </summary>
        /// <returns>true if the input was received, false otherwise. If false is returned, the state may not be in the correct step for skipping.</returns>
        bool Skip();

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_MONOLOGUE; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Player Won */

    /// <summary>
    /// Game responsible for controlling what happens immediately after the player wins the battle.
    /// </summary>
    class GameState_Battle_PlayerWon : public GameState {
    private:
        AWESprite* _enemySprite;
        std::unique_ptr<sf::Clock> _timer;
        std::unique_ptr<sf::Sound> _defeatSound;
        sf::Color _fadeColor;
        sf::Color _originalColor;
        unsigned short _speed;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_PlayerWon();
        GameState_Battle_PlayerWon(AWESprite&, const sf::SoundBuffer&, sf::Color, unsigned short);

        /// <summary>
        /// Amount of time in microseconds between the enemy sprite gaining transperancy.
        /// </summary>
        static const unsigned long FADE_TICK;

        const AWESprite* enemySprite() const;
        const sf::Sound* defeatSound() const;
        const sf::Color& fadeColor() const;
        /// <returns>Amount of transperancy the fadebox gains each "tick" (See FADE_TICK). Note transperancy is a value between 0U and 255U.</returns>
        unsigned short speed() const;

        AWESprite* enemySprite(AWESprite&);
        void defeatSound(const sf::SoundBuffer&);
        sf::Color fadeColor(sf::Color);
        unsigned short speed(unsigned short);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERWON; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* Player Lost */

    /// <summary>
    /// State responsible for controlling what happens immediately after the player losses the battle.
    /// </summary>
    class GameState_Battle_PlayerLost : public GameState {
    private:
        bool _isAcknowledged;
        TextBox* _prompt;

        /// <summary>
        /// Backs up the reset behavior. Used because more than one step function can initiate this reset behavior.
        /// </summary>
        void Reset();

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_Battle_PlayerLost();
        GameState_Battle_PlayerLost(TextBox&);

        const TextBox* prompt() const;
        bool isAcknowledged() const;

        TextBox* prompt(TextBox&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::BATTLE_PLAYERLOST; }

        /// <summary>
        /// Used to indicate the player has acknowledged the defeat message.
        /// </summary>
        /// <returns>true if successful, false otherwise. If false is returned, the state may not be in the correct step for acknowledgement.</returns>
        bool Acknowledge();

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };


    /* -- Level Up -- */


    /* Load Characters */

    // Forward declaration, defined in gamestatemachine.h
    class GameState_MachineLevelUp_SetSelect;

    /// <summary>
    /// State responsible for finding the characters who are to be leveled up.
    /// </summary>
    class GameState_LevelUp_LoadCharacters : public GameState {
    private:
        GameXLOStorage* _xlo;
        std::unordered_map<GameTextureType, Battler_shptr> _characters;

        /// <summary>
        /// Used to get the characters from the XLO storage.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool RefreshCharacters();

        friend class GameState_MachineLevelUp_SetSelect;

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
        GameState_LevelUp_LoadCharacters();
        GameState_LevelUp_LoadCharacters(GameXLOStorage&);

        const GameXLOStorage* xlo() const;
        const std::unordered_map<GameTextureType, Battler_shptr>& characters() const;

        GameXLOStorage* xlo(GameXLOStorage&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::LEVELUP_LOADCHARACTERS; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
    };


    /* Select */

    /// <summary>
    /// State responsible for stewarding the game through the player level up selection process.
    /// </summary>
    class GameState_LevelUp_Select : public GameState {
    private:
        AWESprite* _menu;
        Battler_shptr _battler;
        bool _isSelectionConfirmed;
        std::unique_ptr<sf::Sound> _cursorSound;
        std::unique_ptr<sf::Sound> _selectSound;

        /// <summary>
        /// Extracts the appropriate elemental affinity from the current battler using the group key and damage type given.
        /// </summary>
        /// <param name="output">Output parameter. If successful, this will be set to the extracted elemental affinity.</param>
        /// <returns>true if successful, false otherwise.</returns>
        bool ExtractElement(SkillElementGroupKey, DamageTypeKey, ElementalAffinityKey& output);

    public:
        /// <summary>
        /// Default constructor. Initializes current step to BEGINNING.
        /// </summary>
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

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::LEVELUP_SELECT; }

        /// <summary>
        /// Moves the skill selection cursor down.
        /// </summary>
        /// <returns>true if successful, false otherwise. If false is returned, the cursor may already be over the bottom element.</returns>
        bool IncrementMenu();
        /// <summary>
        /// Moves the skill selection cursor up.
        /// </summary>
        /// <returns>true if successful, false otherwise. If false is returned, the cursor may already be over the top element.</returns>
        bool DecrementMenu();
        /// <summary>
        /// Locks the cursor to its current position and confirms the upgrade its over as the selection.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ConfirmSelection();
        /// <summary>
        /// Releases the selection allowing the cursor to move again.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ReleaseSelection();

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
        /// <summary>
        /// Behavior to execute when the current step is PROCESSING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Process() override;
        /// <summary>
        /// Behavior to execute when the current step is ENDING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool End() override;
        /// <summary>
        /// Resets the state back to its initial state, occasionally undoing the work it did.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ResetSteps() override;
    };
}
