#pragma once
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../models/battler.h"
#include "../models/lovpairs.h"
#include "../models/skill.h"
#include "../store/gamelovstorage.h"
#include "../store/gamexlostorage.h"

namespace AWE {
    /// <summary>
    /// All damage calculations ultimately get rounded into a 32-bit integer.
    /// </summary>
    typedef long DamageValueFinalTotal;
    /// <summary>
    /// Map of type-inclination pairs to intermediate damage values. Examples: 3.75 physical slashing damage, 4.00 magical aetheric damage, etc.
    /// </summary>
    typedef std::map<DamageTypeInclination, float, DamageTypeInclination_comp> DamageValueMap;
    /// <summary>
    /// Map used to report intermediate damage values keyed by either inclination or damage type.
    /// For example, say you had the following DamageValueMap: { 3.0 physical crushing damage, 7.0 physical pyretic damage, 5.0 magical crushing damage, 9.0 magical pyretic damage }.
    /// Then if you wanted to break that down by inclination, you'd get a map of this type looking like { 10.0 physical damage, 14.0 magical damage }.
    /// Or if you wanted a break-down by damage type, you'd get { 8.0 crushing damage, 16.0 pyretic damage }.
    /// </summary>
    typedef std::unordered_map<ABRV_long, float> DamageValueSubmap;

    /// <summary>
    /// Represents base damage. Mostly needed to pair an inclination with an intermediate damage value.
    /// </summary>
    class DamageBaseDamageValue {
    private:
        DamageInclination_shptr _inclination;
        float _value;

    public:
        DamageBaseDamageValue(DamageInclination_shptr, float);

        float value() const;
        const DamageInclination_shptr& inclination() const;
    };

    /// <summary>
    /// Wraps a base damage, a DamageValueMap, and an intermediate total. Skills can have multiple instances of damage - this object represents one such instance.
    /// </summary>
    class DamageValues {
    private:
        float _total;
        DamageBaseDamageValue _base;
        DamageValueMap _subtotals;

    public:
        DamageValues(DamageBaseDamageValue&, DamageValueMap&);

        float total() const;
        const DamageBaseDamageValue& base() const;
        const DamageValueMap& subtotals() const;

        float GetSubtotal(DamageTypeInclination) const;
        DamageValueSubmap CreateInclinationSubtotals() const;
        DamageValueSubmap CreateInclinationSubtotals(DamageTypeKey) const;
        DamageValueSubmap CreateTypeSubtotals() const;
        DamageValueSubmap CreateTypeSubtotals(DamageInclinationKey) const;
    };

    typedef std::vector<DamageValues> DamageValuesList;

    /// <summary>
    /// Represents the "grand total" of all damage calculation. Skills can have multiple instances of damage - this object aggregates all of them.
    /// </summary>
    class Damage {
    private:
        DamageValueFinalTotal _final;
        DamageValuesList _values;

    public:
        Damage(DamageValuesList&);

        DamageValueFinalTotal final() const;
        const DamageValuesList& values() const;

        std::string ToString(const DamageTypeMap& damageTypes) const;
    };

    /// <summary>
    /// The damage calculator.
    /// </summary>
    class DamageCalculator {
    private:
        const GameLOVStorage* _lov;
        const GameXLOStorage* _xlo;

        template <typename T> T range(T value, T min, T max) const;
        template <typename T> T mingate(T value, T min) const;
        template <typename T> T maxgate(T value, T max) const;

    public:
        DamageCalculator(const GameLOVStorage&, const GameXLOStorage&);

        static const unsigned int ELEM_THRESHOLD_CONSTANT;
        static const float ELEMTHRESH_MULTIP_CONSTANT;
        static const float NONPEN_BONUS_MAX;
        static const float PENETRATION_EFFECTIVENESS;
        static const float REDUCTION_CONSTANT;
        static const float RESIDUAL_CONSTANT;

        const GameLOVStorage* lov() const;
        const GameXLOStorage* xlo() const;

        Damage CalculateDamage(const Skill_shptr& skill, const BattlerInstance_shptr& attacker, const BattlerInstance_shptr& defender) const;
    };
}
