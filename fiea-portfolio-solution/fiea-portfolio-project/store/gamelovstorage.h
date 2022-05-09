#pragma once
#include "../models/battlerstat.h"
#include "../models/damageinclination.h"
#include "../models/damagetype.h"
#include "../models/equipmenttype.h"
#include "../models/skillelement.h"
#include "../models/skillelementgroup.h"

namespace AWE {
    class GameLOVStorage {
    private:
        static const std::string DEFAULT_LOAD_RES_LOC;

        BattlerStatMap _battlerStats;
        DamageInclinationMap _damageInclinations;
        DamageTypeMap _damageTypes;
        EquipmentTypeMap _equipmentTypes;
        SkillElementMap _skillElements;
        SkillElementGroupMap _skillElementGroups;

        bool _isInitialized;

    public:
        const BattlerStatMap& battlerStats() const;
        const DamageInclinationMap& damageInclinations() const;
        const DamageTypeMap& damageTypes() const;
        const EquipmentTypeMap& equipmentTypes() const;
        const SkillElementMap& skillElements() const;
        const SkillElementGroupMap& skillElementGroups() const;
        bool isInitialized() const;

        bool Initialize(const std::string& resloc = DEFAULT_LOAD_RES_LOC);

        const BattlerStat_shptr GetBattlerStat(BattlerStatKey) const;
        const DamageInclination_shptr GetDamageInclination(DamageInclinationKey) const;
        const DamageType_shptr GetDamageType(DamageTypeKey) const;
        const EquipmentType_shptr GetEquipmentType(EquipmentTypeKey) const;
        const SkillElement_shptr GetSkillElement(SkillElementKey) const;
        const SkillElementGroup_shptr GetSkillElementGroup(SkillElementGroupKey) const;
    };
}
