#pragma once
#include <unordered_map>
#include <vector>
#include "gamebattleinfo.h"
#include "gamestate.h"
#include "../store/gamesfmlstorage.h"
#include "../store/gamexlostorage.h"

namespace AWE {
    typedef std::unordered_map<GameStateType, std::unique_ptr<GameState>> GameStateMap;
    typedef std::vector<GameState*> GameStatePtrList;

    // Forward declarations, defined later in the file.
    class GameState_MachineBattle_BattleLoop;
    class GameState_MachineBattle_PlayerLoop;
    class GameState_MachineBattle_SkillInit;
    class GameState_MachineBattle_SkillLoop;

    /// <summary>
    /// Manages, organizes, stores, and tracks status of game states. The state machine needs to have the conditions for its various configurations initialized
    /// before it can assume any of them - some of these initializations happen automatically depending on the constructor used for the state machine.
    /// </summary>
    class GameStateMachine : public GameState {
    private:
        GameStateMap _map;
        GameStatePtrList _states;
        unsigned int _index;

        GameBattleInfo* _battle;
        GameSFMLStorage* _sfmls;
        GameSceneTransitioner* _scenes;
        GameXLOStorage* _xlo;

        bool _isSceneInitialized;
        bool _isBattleInitialized;
        bool _isBattleConfigured;
        bool _isBattleLostConfigured;
        bool _isLevelupInitialized;
        bool _isLevelupConfigured;

        /// <summary>
        /// Starting with the current state, calls ResetSteps on each state and then moves the current state backwards until a state with the given type is reached.
        /// BE VERY CAREFUL USING THIS FUNCTION. Several things can go wrong. For example, if the given state is not in the current list, every state in the list
        /// will be reset.
        /// </summary>
        /// <returns>true if successful, false otherwise. Note returning false may still mean several, if not all, states have been reset.</returns>
        bool ResetBack(GameStateType);

        friend class GameState_MachineScene_Transition;
        friend class GameState_MachineBattle_BattleLoop;
        friend class GameState_MachineBattle_PlayerLoop;
        friend class GameState_MachineBattle_SkillInit;
        friend class GameState_MachineBattle_SkillLoop;
        friend class GameState_MachineLevelUp_SetSelect;
        friend class GameState_MachineLevelUp_Advance;

    public:
        /// <summary>
        /// Constructor. Creates all states, but does not initialize any of them.
        /// </summary>
        GameStateMachine(GameXLOStorage&, GameSceneTransitioner&, GameBattleInfo&);
        /// <summary>
        /// Constructor. Creates all states, then initializes all scene-related, battle-related, and level-up-related states.
        /// </summary>
        GameStateMachine(GameXLOStorage&, GameSceneTransitioner&, GameBattleInfo&, GameSFMLStorage&, sf::Sound&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINE; }

        /// <returns>Have all scene-related states in the state machine been initialized?</returns>
        bool isSceneInitialized() const;
        /// <returns>Have all battle-related states in the state machine been initialized?</returns>
        bool isBattleInitialized() const;
        /// <returns>Is the state machine currently configured for battle?</returns>
        bool isBattleConfigured() const;
        /// <returns>Is the state machine currently configured for a battle-lost scenario?</returns>
        bool isBattleLostConfigured() const;
        /// <returns>Have all level-up-related states in the state machine been initialized?</returns>
        bool isLevelupInitialized() const;
        /// <returns>Is the state machine currently configured for level-up?</returns>
        bool isLevelupConfigured() const;
        /// <returns>Mutable pointer to the current state. Note the object will be returned at the superclass level, so subclass-specific functionality will not immediately be available.</returns>
        GameState* current();

        /// <summary>
        /// Gets a state of the given type and converts it to the type parameter given.
        /// </summary>
        /// <returns>Pointer to the requested state if it exists and the type is valid and if the type parameter matched, nullptr otherwise.</returns>
        template <typename TGameState> TGameState* GetState(GameStateType);
        /// <summary>
        /// Configure the state machine for the beginning of the game.
        /// </summary>
        void ConfigureForBeginning();
        /// <summary>
        /// Initializes all scene-related states in the state machine.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool InitializeScene(GameSFMLStorage&);
        /// <summary>
        /// Initializes all battle-related states in the state machine.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool InitializeBattle(GameSFMLStorage&, sf::Sound&);
        /// <summary>
        /// Configures the state machine for battle.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ConfigureForBattle();
        /// <summary>
        /// Configures the state machine for a battle-lost scenario.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ConfigureForBattleLost();
        /// <summary>
        /// Initializes all level-up-related states in the state machine.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool InitializeLevelup(GameSFMLStorage&);
        /// <summary>
        /// Configures the state machine for level-up.
        /// </summary>
        /// <returns>true if successful, false otherwise.</returns>
        bool ConfigureForLevelup();

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


    /* -- Scene -- */

    /// <summary>
    /// State-machine-exclusive state. Assists with scene transitions.
    /// </summary>
    class GameState_MachineScene_Transition : public GameState {
    private:
        GameStateMachine* _parent;

    public:
        GameState_MachineScene_Transition(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINESCENE_TRANSITION; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
    };


    /* -- Battle -- */

    /// <summary>
    /// State-machine-exclusive state. Assists with ensuring every available character has received the opportunity for input.
    /// </summary>
    class GameState_MachineBattle_PlayerLoop : public GameState {
    private:
        GameStateMachine* _parent;

        /// <summary>
        /// Used to reset several states of the state machine and move its current state back.
        /// </summary>
        void ConductReset();

    public:
        GameState_MachineBattle_PlayerLoop(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_PLAYERLOOP; }

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
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with initializing all of the game elements to display a skill usage.
    /// </summary>
    class GameState_MachineBattle_SkillInit : public GameState {
    private:
        GameStateMachine* _parent;

    public:
        GameState_MachineBattle_SkillInit(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_SKILLINIT; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with ensuring all battler decisions are displayed and resolved.
    /// </summary>
    class GameState_MachineBattle_SkillLoop : public GameState {
    private:
        GameStateMachine* _parent;

        /// <summary>
        /// Used to reset several states of the state machine and move its current state back.
        /// </summary>
        void ConductReset();

    public:
        GameState_MachineBattle_SkillLoop(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_SKILLLOOP; }

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
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with determining the state of battle. If the battle is not over, it will reset the whole loop.
    /// </summary>
    class GameState_MachineBattle_BattleLoop : public GameState {
    private:
        GameStateMachine* _parent;
        bool _battleresult;

        /// <summary>
        /// Used to reset several states of the state machine and move its current state back.
        /// </summary>
        void ConductReset();

    public:
        GameState_MachineBattle_BattleLoop(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_BATTLELOOP; }

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


    /* -- Level Up -- */

    /// <summary>
    /// State-machine-exclusive state. Assists with initializing the "Select" state.
    /// </summary>
    class GameState_MachineLevelUp_SetSelect : public GameState {
    private:
        GameStateMachine* _parent;

    public:
        GameState_MachineLevelUp_SetSelect(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINELEVELUP_SETSELECT; }

        /// <summary>
        /// Behavior to execute when the current step is BEGINNING.
        /// </summary>
        /// <returns>true if successful, false otherwise. Returning true does not necessarily guarantee the current step changed.</returns>
        bool Begin() override;
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with iterating through the level-up process.
    /// </summary>
    class GameState_MachineLevelUp_Advance : public GameState {
    private:
        GameStateMachine* _parent;

        /// <summary>
        /// Used to reset several states of the state machine and move its current state back.
        /// </summary>
        void ConductReset();

    public:
        GameState_MachineLevelUp_Advance(GameStateMachine&);

        /// <returns>State type.</returns>
        GameStateType stateType() const override { return GameStateType::MACHINELEVELUP_ADVANCE; }

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
    };
}
