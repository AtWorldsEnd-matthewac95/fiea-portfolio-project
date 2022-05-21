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

        // For each damage component of the skill, we will calculate the damage values. The final damage object will be the total of each of these calculations.
        std::vector<SkillDamage> skillDamages = skill->damages();
        for (const SkillDamage& skillDamage : skillDamages) {
            std::vector<SkillElementBinding> elementBindings = skillDamage.elementBindings();
            std::vector<SkillStatScaling> statScalings = skillDamage.statScalings();
            DamageValueMap subtotals;
            std::unordered_map<DamageInclinationKey, float> damageReductions;

            // Most damage is calculated per inclination. In other words, calculate all physical damage first, then all magical damage, etc.
            for (const DamageInclinationMap::value_type& damageInclination : damageInclinations) {
                DamageInclinationKey skillDamageInclination = (skillDamage.inclinationKey() == DamageInclination::AUTO_KEY.AsLong()) ? damageInclination.first : skillDamage.inclinationKey();
                // An inclination's "attacking stats" are used to calculate damage reduction, and do not have a bearing on actual damage totals outside of that.
                BattlerStatList attackingStats = _xlo->CopyAttackingStats(skillDamageInclination);
                // The attacking stats are compared against the defending stats to determine total damage reduction.
                BattlerStatList defendingStats = _xlo->CopyDefendingStats(damageInclination.first);

                unsigned long defendingStatTotal = 0;
                for (const BattlerStat_shptr& defendingStat : defendingStats) {
                    defendingStatTotal += defender->GetStat(defendingStat->abrvlong());
                }

                // Sum total of ALL resistances the battler has against this inclination of damage. Used for the secondary component of damage reduction.
                long defendingResistTotal = 0;
                for (const DamageResistances::const_iterator::value_type& defendingResist : defender->resistances()) {
                    defendingResistTotal += mingate<long>(defendingResist.second, 0);
                }

                unsigned long attackingStatTotal = 0;
                for (const BattlerStat_shptr& attackingStat : attackingStats) {
                    attackingStatTotal += attacker->GetStat(attackingStat->abrvlong());
                }

                // The "main" component of damage reduction. This is almost equivalent to/can basically be thought of as a ratio of attacking stats to defending stats.
                // It's not an exact ratio because of the RESIDUAL_CONSTANT expression - essentially, the attacking stats have to be (1 + resid const) times greater than defending to achieve parity.
                // This ratio may end up being greater than 1, but it's capped at 1 at a later stage in calculation.
                float damageReduction_stats = ((attackingStatTotal - (RESIDUAL_CONSTANT * defendingStatTotal)) / (defendingStatTotal > 0UL ? static_cast<float>(defendingStatTotal) : 1.0f));

                // The "secondary" component of damage reduction. This is a ratio of the REDUCTION_CONSTANT (note this is different from RESIDUAL_CONSTANT, their names look similar) to the sum of
                // the REDUCTION_CONSTANT and defending stats and defending resistances. This expression should be close to 1 for small values of stats and resistances because the reduction constant
                // is a rather large number. This is meant to come into effect later in the game as stats become larger - the idea being, defensive investment always means *something*, even if
                // the aggresor has considerably greater attacking stats. Note also that attacking stats cannot affect this component of damage reduction, so this can be quite dangerous for
                // balancing if some counter measures are not put into place...
                float damageReduction_resists = REDUCTION_CONSTANT / (REDUCTION_CONSTANT + defendingResistTotal + defendingStatTotal);

                // ...such as this term. This is a minimum value for the secondary damage reduction. It's very similar to the previous calculation, only with the attacking stat total playing a role
                // at the cost of the large REDUCTION_CONSTANT value being doubled in the denominator.
                float damageReduction_resists_min = (REDUCTION_CONSTANT + (0.5f * attackingStatTotal)) / ((REDUCTION_CONSTANT * 2.0f) + defendingResistTotal + defendingStatTotal);

                // In the end, the final damage reduction is the result of passing every value through a range and multiplying them all together, then the reduction is inserted into the map for later use.
                float damageReduction = range<float>(damageReduction_stats, 0.0f, 1.0f) * range<float>(damageReduction_resists, range<float>(damageReduction_resists_min, 0.0f, 1.0f), 1.0f);
                damageReductions.insert(std::make_pair(damageInclination.first, damageReduction));

                // All damage which scales off of stats are calculated here. This is the damage which then gets "split" into elemental damage.
                // I use quotes around "split" there because there does not necessarily need to be a 1:1 correlation to the total elemental damage to this stat damage total.
                // Some skills will use less than 100% of the damage, other skills will use close to double or triple this amount in elemental damage. It all depends on the skill's design.
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

                        // RESIDUAL_CONSTANT comes into play once again here, fulfilling much the same purpose as before.
                        float affinityReduced = mingate<float>(affinity - (RESIDUAL_CONSTANT * resistance), 0.0f);

                        // The elemental threshold constant can be thought of in the following way: If the difference between the attacker's affinity and the defender's resistance is equal
                        // to the threshold constant, the defender will take 100% more of that kind of damage. If the difference is double the threshold constant, the defender will take
                        // 200% more of that kind of damage. If the difference is NEGATIVE (-) and equal to half the threshold constant, the defender will take 50% less of that kind of damage,
                        // and so on. Of course, due to the RESIDUAL_CONSTANT expression above that's not *exactly* how it works, but it's close enough to give a clear picture of what's going on.
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
                            /*
                             * This bonus scaling term was one of the only terms which did not end up working the way I thought it would.
                             * Right now, it results in most all non-penetrating damage being doubled.
                             *
                             * The original idea of this term was that because penetrating damage just gets to flat-out ignore damage reduction, I was worried penetrating damage would just be
                             * "strictly better" than nonpenetrating damage and that I would feel inclined to give every powerful ability penetrating damage. To combat this, I thought it would
                             * be a good idea to create a term which can provide a bonus for using nonpenetrating damage which may not be apparent at lower stat values but would begin to be
                             * noticeable with higher stat investments.
                             *
                             * I eventually reasoned that a good implementation of this bonus would be as a sort of logical inverse to damage reduction. Where damage reduction gets capped at
                             * 1, assuming the player has more attacking stats than the defender's defensive stats, this term could have a floor of 1 and would become greater if the attacker's
                             * stats far exceeded the defensive stats of their opponent. I especially liked this because it created a clear role for penetrating damage vs nonpenetrating damage:
                             * if you don't think your offenses greatly surpass their defenses, use penetrating damage, otherwise prefer nonpenetrating damage.
                             *
                             * In principle, it's a good idea. The problem is that I neglected to realize something rather obvious: at lower stat values, it's quite easy for an attacker's
                             * attacking stats to be close to double the defender's defensive stats, especially because I use two attacking stats and one defensive stat. The easy "band-aid" solution
                             * would be to give each battler an equipment which gives them bonus defenses (as would actually happen in a real version of this game), or to double each battler's
                             * defenses. Unfortunately, by the time I realized what was happening I had already balanced the game around the current damage values, so I decided to leave it as
                             * is for now for simplicity's sake, and because people like seeing bigger numbers, and finally as a testimony to how drastic the consequences can be to overlooking
                             * even one small detail in a combat system this complicated.
                             *
                             * If I were to test a "real" version of this game with armor and continue to be unhappy with how the results play out (right now it might make things rather swingy
                             * at low levels), a potential long-term solution would be to create a new constant whose value is something like 20 or so, and simply subtract this value from the
                             * bonus numerator. That way, the player would have to get their stats to a high enough level to see this bonus, and because of the range this term can't go any lower
                             * than 1 so I wouldn't have to worry about accidentally making anything negative.
                             */
                            float bonusScaling = range<float>(
                                (elementalStatDamage - (RESIDUAL_CONSTANT * defendingStatTotal)) / (defendingStatTotal > 0UL ? static_cast<float>(defendingStatTotal) : 1.0f),
                                1.0f,
                                NONPEN_BONUS_MAX);

                            // This is another advantage nonpenetrating damage has over penetrating damage. It's possible, with enough resistance investment on the defender's part, to completely
                            // reduce the amount of penetrating damage one takes to 0. This "mingate" term makes this not so for nonpenetrating damage.
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
