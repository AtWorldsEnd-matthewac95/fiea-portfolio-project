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

        bool ResetBack(GameStateType);

        friend class GameState_MachineScene_Transition;
        friend class GameState_MachineBattle_BattleLoop;
        friend class GameState_MachineBattle_PlayerLoop;
        friend class GameState_MachineBattle_SkillInit;
        friend class GameState_MachineBattle_SkillLoop;
        friend class GameState_MachineLevelUp_SetSelect;
        friend class GameState_MachineLevelUp_Advance;

    public:
        GameStateMachine(GameXLOStorage&, GameSceneTransitioner&, GameBattleInfo&);
        GameStateMachine(GameXLOStorage&, GameSceneTransitioner&, GameBattleInfo&, GameSFMLStorage&, sf::Sound&);

        GameStateType stateType() const override { return GameStateType::MACHINE; }

        bool isSceneInitialized() const;
        bool isBattleInitialized() const;
        bool isBattleConfigured() const;
        bool isBattleLostConfigured() const;
        bool isLevelupInitialized() const;
        bool isLevelupConfigured() const;
        GameState* current();

        template <typename TGameState> TGameState* GetState(GameStateType);
        void ConfigureForBeginning();
        bool InitializeScene(GameSFMLStorage&);
        bool InitializeBattle(GameSFMLStorage&, sf::Sound&);
        bool ConfigureForBattle();
        bool ConfigureForBattleLost();
        bool InitializeLevelup(GameSFMLStorage&);
        bool ConfigureForLevelup();

        bool Begin() override;
        bool Process() override;
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

        GameStateType stateType() const override { return GameStateType::MACHINESCENE_TRANSITION; }

        bool Begin() override;
    };


    /* -- Battle -- */

    /// <summary>
    /// State-machine-exclusive state. Assists with ensuring every available character has received the opportunity for input.
    /// </summary>
    class GameState_MachineBattle_PlayerLoop : public GameState {
    private:
        GameStateMachine* _parent;

        void ConductReset();

    public:
        GameState_MachineBattle_PlayerLoop(GameStateMachine&);

        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_PLAYERLOOP; }

        bool Begin() override;
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

        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_SKILLINIT; }

        bool Begin() override;
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with ensuring all battler decisions are displayed and resolved.
    /// </summary>
    class GameState_MachineBattle_SkillLoop : public GameState {
    private:
        GameStateMachine* _parent;

        void ConductReset();

    public:
        GameState_MachineBattle_SkillLoop(GameStateMachine&);

        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_SKILLLOOP; }

        bool Begin() override;
        bool Process() override;
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with determining the state of battle. If the battle is not over, it will reset the whole loop.
    /// </summary>
    class GameState_MachineBattle_BattleLoop : public GameState {
    private:
        GameStateMachine* _parent;
        bool _battleresult;

        void ConductReset();

    public:
        GameState_MachineBattle_BattleLoop(GameStateMachine&);

        GameStateType stateType() const override { return GameStateType::MACHINEBATTLE_BATTLELOOP; }

        bool Begin() override;
        bool Process() override;
        bool End() override;
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

        GameStateType stateType() const override { return GameStateType::MACHINELEVELUP_SETSELECT; }

        bool Begin() override;
    };

    /// <summary>
    /// State-machine-exclusive state. Assists with iterating through the level-up process.
    /// </summary>
    class GameState_MachineLevelUp_Advance : public GameState {
    private:
        GameStateMachine* _parent;

        void ConductReset();

    public:
        GameState_MachineLevelUp_Advance(GameStateMachine&);

        GameStateType stateType() const override { return GameStateType::MACHINELEVELUP_ADVANCE; }

        bool Begin() override;
        bool Process() override;
    };
}
