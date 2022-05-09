#include "equipment.h"

namespace AWE {
    Equipment::Equipment() : _name(EQUIPMENT_EMPTYNAME) {}

    Equipment::Equipment(
        std::string name,
        DamageResistances& bonusResistances,
        DamageSourceMap& damageSources,
        EquipmentType_shptr equipmentType,
        const SkillMap& skillPossibilities,
        BattlerStatValues& bonusStats,
        SkillElementGroupConversionMap& conversions
    )
            : _name(std::move(name))
            , _bonusStats(std::move(bonusStats))
            , _bonusResistances(std::move(bonusResistances))
            , _damageSources(std::move(damageSources))
            , _equipmentType(std::move(equipmentType))
            , _conversions(std::move(conversions))
            , _skills(std::vector<Skill_shptr>()) {
        for (const SkillMap::value_type& skillPossibility : skillPossibilities) {
            bool add = true;
            const std::set<SkillElementGroupKey>& skillElementGroups = skillPossibility.second->elementGroups();
            for (SkillElementGroupKey groupKey : skillElementGroups) {
                if (!_conversions.contains(groupKey)) {
                    add = false;
                    break;
                }
            }

            if (add) {
                _skills.push_back(skillPossibility.second);
            }
        }
    }

    const Equipment Equipment::EMPTY = Equipment();

    const std::string& Equipment::name() const { return _name; }
    const BattlerStatValues& Equipment::bonusStats() const { return _bonusStats; }
    const DamageResistances& Equipment::bonusResistances() const { return _bonusResistances; }
    const DamageSourceMap& Equipment::damageSources() const { return _damageSources; }
    const EquipmentType_shptr& Equipment::equipmentType() const { return _equipmentType; }
    const std::vector<Skill_shptr>& Equipment::skills() const { return _skills; }

    BattlerStatValue Equipment::GetBonusStat(BattlerStatKey key) const {
        BattlerStatValue val = 0;
        auto found = _bonusStats.find(key);

        if (found != _bonusStats.end()) {
            val = found->second;
        }

        return val;
    }

    DamageSourceValue Equipment::GetDamageSourceValue(DamageTypeInclination key) const {
        DamageSourceValue val = 0;
        auto found = _damageSources.find(key);

        if (found != _damageSources.end()) {
            val = found->second;
        }

        return val;
    }

    DamageSourceValue Equipment::GetDamageSourceValue(const DamageType& dmgtype, const DamageInclination& inclination) const { return GetDamageSourceValue(DamageTypeInclination(dmgtype.abrvlong(), inclination.abrvlong())); }

    bool Equipment::HasConversion(SkillElementGroupKey group) const { return _conversions.contains(group); }

    SkillElement_shptr Equipment::GetConversion(SkillElementGroupKey group) const {
        SkillElement_shptr conversion;
        auto found = _conversions.find(group);

        if (found != _conversions.end()) {
            conversion = found->second;
        }

        return conversion;
    }
}
