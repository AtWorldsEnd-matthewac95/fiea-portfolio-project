#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "battlerstat.h"
#include "damageresistances.h"
#include "damagesource.h"
#include "lovpairs.h"
#include "skill.h"

namespace AWE {
    static const std::string EQUIPMENT_EMPTYNAME = std::string("Empty");

    class Equipment {
    private:
        std::string _name;
        BattlerStatValues _bonusStats;
        DamageResistances _bonusResistances;
        DamageSourceMap _damageSources;
        EquipmentType_shptr _equipmentType;
        SkillElementGroupConversionMap _conversions;
        std::vector<Skill_shptr> _skills;

    public:
        Equipment();
        Equipment(
            std::string name,
            DamageResistances& bonusResistances,
            DamageSourceMap& damageSources,
            EquipmentType_shptr equipmentType,
            const SkillMap& skillPossibilities,
            BattlerStatValues& bonusStats,
            SkillElementGroupConversionMap& conversions
        );

        static const Equipment EMPTY;

        const std::string& name() const;
        const BattlerStatValues& bonusStats() const;
        const DamageResistances& bonusResistances() const;
        const DamageSourceMap& damageSources() const;
        const EquipmentType_shptr& equipmentType() const;
        const std::vector<Skill_shptr>& skills() const;

        BattlerStatValue GetBonusStat(BattlerStatKey) const;
        DamageSourceValue GetDamageSourceValue(DamageTypeInclination) const;
        DamageSourceValue GetDamageSourceValue(const DamageType&, const DamageInclination&) const;
        bool HasConversion(SkillElementGroupKey) const;
        SkillElement_shptr GetConversion(SkillElementGroupKey) const;
    };

    typedef std::string EquipmentKey;
    typedef std::shared_ptr<Equipment> Equipment_shptr;
    typedef std::map<EquipmentKey, Equipment_shptr> EquipmentMap;
    typedef std::vector<Equipment_shptr> EquipmentList;
}
