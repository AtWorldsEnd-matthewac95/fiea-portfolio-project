#include "damage.h"
#include <math.h>
#include <sstream>
#include "../models/damagesource.h"

namespace AWE {

    /* DamageBaseDamageValue */

    DamageBaseDamageValue::DamageBaseDamageValue(DamageInclination_shptr inclination, float value) : _inclination(inclination), _value(value) {}
    const DamageInclination_shptr& DamageBaseDamageValue::inclination() const { return _inclination; }
    float DamageBaseDamageValue::value() const { return _value; }


    /* DamageValues */

    DamageValues::DamageValues(DamageBaseDamageValue& base, DamageValueMap& subtotals) : _base(std::move(base)), _subtotals(subtotals) {
        _total = _base.value();

        for (const DamageValueMap::value_type& subtotal : subtotals) {
            _total += subtotal.second;
        }
    }
    float DamageValues::total() const { return _total; }
    const DamageBaseDamageValue& DamageValues::base() const { return _base; }
    const DamageValueMap& DamageValues::subtotals() const { return _subtotals; }

    float DamageValues::GetSubtotal(DamageTypeInclination key) const {
        auto found = _subtotals.find(key);
        if (found != _subtotals.end()) {
            return found->second;
        }

        return 0.0f;
    }

    DamageValueSubmap DamageValues::CreateInclinationSubtotals() const {
        DamageValueSubmap submap;

        for (const DamageValueMap::value_type& subtotal : _subtotals) {
            auto found = submap.find(subtotal.first.second);
            if (found == submap.end()) {
                found = submap.insert(std::make_pair(subtotal.first.second, 0.0f)).first;
            }
            found->second += subtotal.second;
        }

        return std::move(submap);
    }
    DamageValueSubmap DamageValues::CreateInclinationSubtotals(DamageTypeKey dtype) const {
        DamageValueSubmap submap;

        for (const DamageValueMap::value_type& subtotal : _subtotals) {
            if (dtype == subtotal.first.first) {
                auto found = submap.find(subtotal.first.second);
                if (found == submap.end()) {
                    found = submap.insert(std::make_pair(subtotal.first.second, 0.0f)).first;
                }
                found->second += subtotal.second;
            }
        }

        return std::move(submap);
    }
    DamageValueSubmap DamageValues::CreateTypeSubtotals() const {
        DamageValueSubmap submap;

        for (const DamageValueMap::value_type& subtotal : _subtotals) {
            auto found = submap.find(subtotal.first.first);
            if (found == submap.end()) {
                found = submap.insert(std::make_pair(subtotal.first.first, 0.0f)).first;
            }
            found->second += subtotal.second;
        }

        return std::move(submap);
    }
    DamageValueSubmap DamageValues::CreateTypeSubtotals(DamageInclinationKey incl) const {
        DamageValueSubmap submap;

        for (const DamageValueMap::value_type& subtotal : _subtotals) {
            if (incl == subtotal.first.second) {
                auto found = submap.find(subtotal.first.first);
                if (found == submap.end()) {
                    found = submap.insert(std::make_pair(subtotal.first.first, 0.0f)).first;
                }
                found->second += subtotal.second;
            }
        }

        return std::move(submap);
    }


    /* Damage */

    Damage::Damage(DamageValuesList& values) : _values(values) {
        float sum = 0.0f;

        for (const DamageValues& val : values) {
            sum += val.total();
        }

        _final = static_cast<DamageValueFinalTotal>(std::ceil(sum));
    }
    DamageValueFinalTotal Damage::final() const { return _final; }
    const DamageValuesList& Damage::values() const { return _values; }

    std::string Damage::ToString(const DamageTypeMap& damageTypes) const {
        std::stringstream ss;
        DamageValueSubmap subtotals;

        ss << "Total  " << _final;

        for (const DamageValues& subdamage : _values) {
            DamageValueSubmap typesubs = subdamage.CreateTypeSubtotals();
            for (DamageValueSubmap::value_type typesub : typesubs) {
                auto found = subtotals.find(typesub.first);
                if (found == subtotals.end()) {
                    found = subtotals.insert(subtotals.begin(), std::make_pair(typesub.first, 0.f));
                }
                found->second += typesub.second;
            }
        }

        for (const DamageValueSubmap::value_type& subtotal : subtotals) {
            auto abrv = damageTypes.find(subtotal.first);
            if (abrv == damageTypes.end()) {
                continue;
            }
            ss << "\n " << abrv->second->abrvstr() << "  " << static_cast<DamageValueFinalTotal>(subtotal.second);
        }

        return ss.str();
    }


    /* DamageCalculator */

    const unsigned int DamageCalculator::ELEM_THRESHOLD_CONSTANT = 20U;
    const float DamageCalculator::ELEMTHRESH_MULTIP_CONSTANT = 10U;
    const float DamageCalculator::NONPEN_BONUS_MAX = 2.0f;
    const float DamageCalculator::PENETRATION_EFFECTIVENESS = 1.0f;
    const float DamageCalculator::REDUCTION_CONSTANT = 2000.0f;
    const float DamageCalculator::RESIDUAL_CONSTANT = 0.1f;

    DamageCalculator::DamageCalculator(const GameLOVStorage& lov, const GameXLOStorage& xlo) : _lov(&lov), _xlo(&xlo) {}

    const GameLOVStorage* DamageCalculator::lov() const { return _lov; }
    const GameXLOStorage* DamageCalculator::xlo() const { return _xlo; }

    template <typename T> T DamageCalculator::range(T value, T min, T max) const { return (value > max) ? max : ((value < min) ? min : value); }
    template <typename T> T DamageCalculator::mingate(T value, T min) const { return (value < min) ? min : value; }
    template <typename T> T DamageCalculator::maxgate(T value, T max) const { return (value > max) ? max : value; }

    Damage DamageCalculator::CalculateDamage(const Skill_shptr& skill, const BattlerInstance_shptr& attacker, const BattlerInstance_shptr& defender) const {
        DamageValuesList values;
        DamageInclinationMap damageInclinations = _lov->damageInclinations();

        std::vector<SkillDamage> skillDamages = skill->damages();
        for (const SkillDamage& skillDamage : skillDamages) {
            std::vector<SkillElementBinding> elementBindings = skillDamage.elementBindings();
            std::vector<SkillStatScaling> statScalings = skillDamage.statScalings();
            DamageValueMap subtotals;
            std::unordered_map<DamageInclinationKey, float> damageReductions;

            for (const DamageInclinationMap::value_type& damageInclination : damageInclinations) {
                DamageInclinationKey skillDamageInclination = (skillDamage.inclinationKey() == DamageInclination::AUTO_KEY.AsLong()) ? damageInclination.first : skillDamage.inclinationKey();
                BattlerStatList attackingStats = _xlo->CopyAttackingStats(skillDamageInclination);
                BattlerStatList defendingStats = _xlo->CopyDefendingStats(damageInclination.first);

                unsigned long defendingStatTotal = 0;
                for (const BattlerStat_shptr& defendingStat : defendingStats) {
                    defendingStatTotal += defender->GetStat(defendingStat->abrvlong());
                }

                long defendingResistTotal = 0;
                for (const DamageResistances::const_iterator::value_type& defendingResist : defender->resistances()) {
                    defendingResistTotal += mingate<long>(defendingResist.second, 0);
                }

                unsigned long attackingStatTotal = 0;
                for (const BattlerStat_shptr& attackingStat : attackingStats) {
                    attackingStatTotal += attacker->GetStat(attackingStat->abrvlong());
                }

                float damageReduction_stats = ((attackingStatTotal - (RESIDUAL_CONSTANT * defendingStatTotal)) / (defendingStatTotal > 0UL ? static_cast<float>(defendingStatTotal) : 1.0f));
                float damageReduction_resists = REDUCTION_CONSTANT / (REDUCTION_CONSTANT + defendingResistTotal + defendingStatTotal);
                float damageReduction_resists_min = (REDUCTION_CONSTANT + (0.5f * attackingStatTotal)) / ((REDUCTION_CONSTANT * 2.0f) + defendingResistTotal + defendingStatTotal);
                float damageReduction = range<float>(damageReduction_stats, 0.0f, 1.0f) * range<float>(damageReduction_resists, range<float>(damageReduction_resists_min, 0.0f, 1.0f), 1.0f);
                damageReductions.insert(std::make_pair(damageInclination.first, damageReduction));

                float elementalStatDamage = 0.0f;
                for (const SkillStatScaling& statScaling : statScalings) {
                    if (statScaling.inclination()->abrvlong() == damageInclination.first) {
                        elementalStatDamage += attacker->GetStat(statScaling.battlerStat()->abrvlong()) * statScaling.value();
                    }
                }

                for (const SkillElementBinding& elementBinding : elementBindings) {
                    if (elementBinding.inclination()->abrvlong() == damageInclination.first) {
                        auto typeincl = DamageTypeInclination(elementBinding.damageType()->abrvlong(), damageInclination.first);
                        ElementalAffinityValue affinity = elementBinding.IsGroupBinding()
                            ? attacker->affinities().GetValue(elementBinding.group(), typeincl.first)
                            : attacker->affinities().GetValue(elementBinding.element()->abrvlong(), typeincl.first);
                        DamageResistanceValue resistance = defender->resistances().GetValue(typeincl);

                        float affinityReduced = mingate<float>(affinity - (RESIDUAL_CONSTANT * resistance), 0.0f);
                        float multiplier = mingate<float>(1.0f + ((affinityReduced - resistance) / ELEM_THRESHOLD_CONSTANT), 0.0f);

                        auto found = subtotals.find(typeincl);
                        if (found == subtotals.end()) {
                            found = subtotals.insert(std::make_pair(typeincl, 0.0f)).first;
                        }

                        float damageSourcePercentage = DetermineDamageSourcePercentage(attacker->GetDamageSource(typeincl));
                        if (elementBinding.isPenetrating()) {
                            float penetratedDamageReduction = range<float>(PENETRATION_EFFECTIVENESS + mingate<float>(damageReduction * (1.0f - PENETRATION_EFFECTIVENESS), 0.0f), 0.0f, 1.0f);
                            found->second += penetratedDamageReduction * elementBinding.scaling() * multiplier * elementalStatDamage * damageSourcePercentage;
                        } else {
                            float bonusScaling = range<float>((elementalStatDamage - (RESIDUAL_CONSTANT * defendingStatTotal)) / defendingStatTotal, 1.0f, NONPEN_BONUS_MAX);
                            float multiplier_mingate = mingate<float>(1.0f - ((ELEMTHRESH_MULTIP_CONSTANT * ELEM_THRESHOLD_CONSTANT) / ((ELEMTHRESH_MULTIP_CONSTANT * ELEM_THRESHOLD_CONSTANT) + affinity)), 0.0f);

                            found->second += damageReduction * elementBinding.scaling() * mingate<float>(multiplier, multiplier_mingate) * bonusScaling * elementalStatDamage * damageSourcePercentage;
                        }
                    }
                }
            }

            auto basedmg = DamageBaseDamageValue(skillDamage.baseDamage().inclination(), damageReductions.at(skillDamage.baseDamage().inclination()->abrvlong()) * skillDamage.baseDamage().value());
            values.push_back(DamageValues(basedmg, subtotals));
        }

        return Damage(values);
    }
}
