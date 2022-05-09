#include "gamebattleinfo.h"

namespace AWE {
    GameBattleInfo::GameBattleInfo(const GameLOVStorage& lov, const GameXLOStorage& xlo, BattlerInstance_shptr enemy) : _damagecalc(DamageCalculator(lov, xlo)), _enemy(std::move(enemy)) {
        RefreshCharacters(xlo);
    }
    GameBattleInfo::GameBattleInfo(const GameLOVStorage& lov, const GameXLOStorage& xlo) : _damagecalc(DamageCalculator(lov, xlo)) {
        const BattlerMap& battlers = xlo.battlers();
        Battler_shptr enemy;
        for (const BattlerMap::value_type& battler : battlers) {
            if (battler.second->isCharacter()) {
                _characters.push_back(std::make_shared<BattlerInstance>(BattlerInstance(*battler.second)));
            } else {
                if (enemy) {
                    if (battler.second->textureIndex() < enemy->textureIndex()) {
                        enemy = battler.second;
                    }
                } else {
                    enemy = battler.second;
                }
            }
        }

        if (enemy) {
            _enemy = std::make_shared<BattlerInstance>(BattlerInstance(*enemy));
        }
    }

    const std::vector<BattlerInstance_shptr>& GameBattleInfo::characters() const { return _characters; }
    const DamageCalculator& GameBattleInfo::damagecalc() const { return _damagecalc; }
    const BattlerDecisionOrdering& GameBattleInfo::decisions() const { return _decisions; }
    const BattlerInstance_shptr& GameBattleInfo::enemy() const { return _enemy; }

    BattlerDecisionOrdering* GameBattleInfo::decisions() { return &_decisions; }
    BattlerInstance_shptr GameBattleInfo::enemy() { return _enemy; }

    BattlerInstance_shptr GameBattleInfo::enemy(BattlerInstance_shptr newval) { BattlerInstance_shptr oldval = std::move(_enemy); _enemy = std::move(newval); return oldval; }

    bool GameBattleInfo::GoToNextEnemy(const BattlerMap& battlers) {
        if (!_enemy) {
            return false;
        }

        bool found = false;
        Battler_shptr next;

        for (const BattlerMap::value_type& battler : battlers) {
            if (battler.second->isCharacter()) {
                continue;
            }

            if (next) {
                if (battler.second->textureIndex() < next->textureIndex() && battler.second->textureIndex() > _enemy->textureIndex()) {
                    next = battler.second;
                }
            } else if (battler.second->textureIndex() > _enemy->textureIndex()) {
                found = true;
                next = battler.second;
            }
        }

        if (found) {
            _enemy = std::make_shared<BattlerInstance>(BattlerInstance(*next));
        }

        return found;
    }

    void GameBattleInfo::RefreshCharacters(const GameXLOStorage& xlo) {
        _characters.clear();
        const BattlerMap& battlers = xlo.battlers();
        for (const BattlerMap::value_type& battler : battlers) {
            if (battler.second->isCharacter()) {
                _characters.push_back(std::make_shared<BattlerInstance>(BattlerInstance(*battler.second)));
            }
        }
    }
}
