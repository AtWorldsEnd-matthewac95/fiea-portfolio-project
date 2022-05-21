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

        /// <returns>Value of the base damage.</returns>
        float value() const;
        /// <returns>const reference to the base damage inclination.</returns>
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

        /// <returns>Sum of all subtotals.</returns>
        float total() const;
        /// <returns>const reference to the base damage.</returns>
        const DamageBaseDamageValue& base() const;
        /// <returns>const reference to the subtotals.</returns>
        const DamageValueMap& subtotals() const;

        /// <returns>Returns the value of a specific total indicated by damage type and inclination.</returns>
        float GetSubtotal(DamageTypeInclination) const;
        /// <returns>Creates a map of subtotals keyed by inclinations.</returns>
        DamageValueSubmap CreateInclinationSubtotals() const;
        /// <returns>Creates a map of subtotals of a specific damage type keyed by inclinations.</returns>
        DamageValueSubmap CreateInclinationSubtotals(DamageTypeKey) const;
        /// <returns>Creates a map of subtotals keyed by damage types.</returns>
        DamageValueSubmap CreateTypeSubtotals() const;
        /// <returns>Creates a map of subtotals of a specific inclination keyed by damage types.</returns>
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

        /// <returns>The total amount of damage.</returns>
        DamageValueFinalTotal final() const;
        /// <returns>const reference to the values.</returns>
        const DamageValuesList& values() const;

        /// <param name="damageTypes">Map which can convert damage type keys to qualified damage type objects. Used to get string forms of damage types.</param>
        /// <returns>A string representation of this object.</returns>
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

        /// <summary>This number * 2.5 should be roughly what a "late-game" affinity value should look like.</summary>
        static const unsigned int ELEM_THRESHOLD_CONSTANT;
        /// <summary>Helps determine minumum multiplier for nonpenetrating damage. The higher this number is, each point of affinity will be less effective at raising the minimum.</summary>
        static const float ELEMTHRESH_MULTIP_CONSTANT;
        /// <summary>Maximum multiplier for bonuses from nonpenetrating damage.</summary>
        static const float NONPEN_BONUS_MAX;
        /// <summary>Needs to be between or equal to 0.0f and/or 1.0f. The amount of reduction penetrating damage ignores.</summary>
        static const float PENETRATION_EFFECTIVENESS;
        /// <summary>Should be roughly double the highest expected value for a battler stat.</summary>
        static const float REDUCTION_CONSTANT;
        /// <summary>Attacking stats must be (1 + this number) times greater than their defensive counterparts to achieve parity.</summary>
        static const float RESIDUAL_CONSTANT;

        /// <returns>const pointer to the LOV storage this damage calculator uses.</returns>
        const GameLOVStorage* lov() const;
        /// <returns>const pointer to the XLO storage this damage calculator uses.</returns>
        const GameXLOStorage* xlo() const;

        /// <returns>Resultant damage from attacker using skill on defender.</returns>
        Damage CalculateDamage(const Skill_shptr& skill, const BattlerInstance_shptr& attacker, const BattlerInstance_shptr& defender) const;
    };
}
