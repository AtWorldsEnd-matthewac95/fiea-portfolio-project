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
        /// <summary>
        /// Constructor. Automatically determines the initial enemy by reading texture indicies.
        /// </summary>
        GameBattleInfo(const GameLOVStorage&, const GameXLOStorage&);
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="enemy">The initial enemy.</param>
        GameBattleInfo(const GameLOVStorage&, const GameXLOStorage&, BattlerInstance_shptr enemy);

        /// <returns>const reference to the damage calculator.</returns>
        const DamageCalculator& damagecalc() const;
        /// <returns>const reference to the character battler instance list.</returns>
        const std::vector<BattlerInstance_shptr>& characters() const;
        /// <returns>const reference to the current battler decision ordering.</returns>
        const BattlerDecisionOrdering& decisions() const;
        /// <returns>const reference to the battler instance of the current enemy in the battle.</returns>
        const BattlerInstance_shptr& enemy() const;

        /// <returns>Mutable pointer to the current battler decision ordering.</returns>
        BattlerDecisionOrdering* decisions();
        /// <returns>Mutable shared pointer to the battler instance of the current enemy in the battle.</returns>
        BattlerInstance_shptr enemy();

        /// <param name="">New value for enemy.</param>
        /// <returns>Old value for enemy.</returns>
        BattlerInstance_shptr enemy(BattlerInstance_shptr);

        /// <summary>
        /// Uses texture indicies from the battler map to determine what the next enemy should be. If the current battler has the highest texture index of the enemies,
        /// then this function will fail to find a new enemy and will return false.
        /// </summary>
        /// <param name="">Map of battlers to search through.</param>
        /// <returns>true if a new enemy was found, false otherwise.</returns>
        bool GoToNextEnemy(const BattlerMap&);
        /// <summary>
        /// Internally creates new battler instances from the character battlers contained in the XLO storage.
        /// </summary>
        void RefreshCharacters(const GameXLOStorage&);
    };
}
