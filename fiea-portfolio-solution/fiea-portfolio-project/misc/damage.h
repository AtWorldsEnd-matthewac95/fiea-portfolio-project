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
    typedef long DamageValueFinalTotal;
    typedef std::map<DamageTypeInclination, float, DamageTypeInclination_comp> DamageValueMap;
    typedef std::unordered_map<ABRV_long, float> DamageValueSubmap;

    class DamageBaseDamageValue {
    private:
        DamageInclination_shptr _inclination;
        float _value;

    public:
        DamageBaseDamageValue(DamageInclination_shptr, float);

        float value() const;
        const DamageInclination_shptr& inclination() const;
    };

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
