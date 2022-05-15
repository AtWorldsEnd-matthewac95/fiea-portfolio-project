#pragma once
#include "battlerdecision.h"
#include "../misc/damage.h"
#include "../models/battler.h"
#include "../store/gamelovstorage.h"
#include "../store/gamexlostorage.h"

namespace AWE {
    /// <summary>
    /// Represents battle information about the game. For this early version of the project, each battle only has one enemy for simplicity's sake.
    /// </summary>
    class GameBattleInfo {
    private:
        DamageCalculator _damagecalc;
        std::vector<BattlerInstance_shptr> _characters;
        BattlerDecisionOrdering _decisions;
        BattlerInstance_shptr _enemy;

    public:
        GameBattleInfo(const GameLOVStorage&, const GameXLOStorage&);
        GameBattleInfo(const GameLOVStorage&, const GameXLOStorage&, BattlerInstance_shptr enemy);

        const DamageCalculator& damagecalc() const;
        const std::vector<BattlerInstance_shptr>& characters() const;
        const BattlerDecisionOrdering& decisions() const;
        const BattlerInstance_shptr& enemy() const;

        BattlerDecisionOrdering* decisions();
        BattlerInstance_shptr enemy();

        BattlerInstance_shptr enemy(BattlerInstance_shptr);

        bool GoToNextEnemy(const BattlerMap&);
        void RefreshCharacters(const GameXLOStorage&);
    };
}
