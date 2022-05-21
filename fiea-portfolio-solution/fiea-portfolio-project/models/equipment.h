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

    /// <summary>
    /// Represents information about equipment.
    /// </summary>
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
        /// <summary>
        /// Default constructor. Equivalent to the EMPTY Equipment constant - so, technically a valid equipment, but not useful for much of anything.
        /// </summary>
        Equipment();
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="skillPossibilities">
        /// The set from which this equipment will look for potential skills. It's perfectly valid, even suggested,
        /// to pass every skill that exists into this parameter, but a more restricted subset is also possible.
        /// </param>
        /// <param name="conversions">
        /// Conversions from skill element groups to skill elements. Used in determining what skills this equipment should get.
        /// If all of a skill's element group bindings have a valid conversion, the equipment will gain that skill.
        /// </param>
        Equipment(
            std::string name,
            DamageResistances& bonusResistances,
            DamageSourceMap& damageSources,
            EquipmentType_shptr equipmentType,
            const SkillMap& skillPossibilities,
            BattlerStatValues& bonusStats,
            SkillElementGroupConversionMap& conversions
        );

        /// <summary>
        /// Meant to represent an empty equipment. Has no name, no equipment type, no damage sources, no stats or resistances, and no skills.
        /// </summary>
        static const Equipment EMPTY;

        /// <returns>Name of this equipment.</returns>
        const std::string& name() const;
        /// <returns>Bonus stats the equipped battler gains from this equipment.</returns>
        const BattlerStatValues& bonusStats() const;
        /// <returns>Bonus damage resistances the equipped battler gains from this equipment.</returns>
        const DamageResistances& bonusResistances() const;
        /// <returns>Damage sources this equipment provides.</returns>
        const DamageSourceMap& damageSources() const;
        /// <returns>Bonus stats the equipped battler gains from this equipment.</returns>
        const EquipmentType_shptr& equipmentType() const;
        /// <returns>Bonus stats the equipped battler gains from this equipment.</returns>
        const std::vector<Skill_shptr>& skills() const;

        /// <returns>This equipment's value of the given bonus stat, or 0 if this equipment has no such bonus stat.</returns>
        BattlerStatValue GetBonusStat(BattlerStatKey) const;
        /// <returns>This equipment's damage source value of the given type inclination pair, or 0 if this equipment has no such damage source.</returns>
        DamageSourceValue GetDamageSourceValue(DamageTypeInclination) const;
        /// <returns>This equipment's damage source value of the given type inclination pair, or 0 if this equipment has no such damage source.</returns>
        DamageSourceValue GetDamageSourceValue(const DamageType&, const DamageInclination&) const;
        /// <returns>Does this equipment have a conversion for the given skill element group?</returns>
        bool HasConversion(SkillElementGroupKey) const;
        /// <returns>This equipment's conversion for the given skill element group, or an empty pointer if no such conversion exists.</returns>
        SkillElement_shptr GetConversion(SkillElementGroupKey) const;
    };

    /// <summary>
    /// Used to index Equipment into a map. Currently designed to be a string which should be equivalent to the equipment's name.
    /// </summary>
    typedef std::string EquipmentKey;
    /// <summary>
    /// Represents information about equipment.
    /// </summary>
    typedef std::shared_ptr<Equipment> Equipment_shptr;
    typedef std::map<EquipmentKey, Equipment_shptr> EquipmentMap;
    typedef std::vector<Equipment_shptr> EquipmentList;
}
