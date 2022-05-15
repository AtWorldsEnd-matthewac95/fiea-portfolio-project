#pragma once
#include "gamelovstorage.h"
#include "../models/lovpairs.h"
#include "../models/battler.h"
#include "../models/battlerstat.h"
#include "../models/damageinclination.h"
#include "../models/equipment.h"
#include "../models/skill.h"

namespace AWE {
    /// <summary>
    /// XLO = XML Loaded Objects. Stores all the objects loaded using tinyxml2.
    /// </summary>
    class GameXLOStorage {
    private:
        EquipmentTypeCountMap _defaultEquipmentSlotSchema;
        DamageInclinationStatListMap _inclinationAttackingStats;
        DamageInclinationStatListMap _inclinationDefendingStats;
        SkillMap _skills;
        EquipmentMap _equipment;
        BattlerMap _battlers;

        bool _isInitialized;

    public:
        static const char* DEFAULT_XMLFILENAME;

        const EquipmentTypeCountMap& defaultEquipmentSlotSchema() const;
        const SkillMap& skills() const;
        const EquipmentMap& equipment() const;
        const BattlerMap& battlers() const;
        bool isInitialized() const;

        bool Initialize(const GameLOVStorage& lov, const DamageInclination_shptr& nullDamageInclination, const char* xmlfilename = DEFAULT_XMLFILENAME);

        BattlerStatList CopyAttackingStats(DamageInclinationKey) const;
        BattlerStatList CopyDefendingStats(DamageInclinationKey) const;
        const Skill_shptr& GetSkill(const SkillKey&) const;
        const Equipment_shptr& GetEquipment(const EquipmentKey&) const;
        const Battler_shptr& GetBattler(const BattlerKey&) const;

        Battler_shptr GetBattler(const BattlerKey&);
        BattlerStatValue AdjustBattlerStat(const BattlerKey&, const BattlerStatKey&, int);
        ElementalAffinityValue AdjustBattlerAffinity(const BattlerKey&, const ElementalAffinityKey&, int);
    };
}
