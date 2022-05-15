#pragma once
#include <map>
#include "damageinclination.h"
#include "damagetype.h"
#include "lovpairs.h"

namespace AWE {
    // Forward declarations, defined in battler.h
    class Battler;
    class BattlerInstance;

    typedef int DamageResistanceValue;
    typedef std::map<DamageTypeInclination, DamageResistanceValue, DamageTypeInclination_comp> DamageResistanceMap;

    /// <summary>
    /// Represents a battler's resistances to specific types of damage. Note that "types of damage" here actually means inclination-type pairs, e.g. physical slashing, magical arcane, etc.
    /// Also note that DamageResistanceValue is not unsigned - there can be negative resistances.
    /// </summary>
    class DamageResistances {
    private:
        DamageResistanceMap _map;

        DamageResistanceValue SetValue(DamageTypeInclination, DamageResistanceValue);
        DamageResistanceValue AddValue(DamageTypeInclination, DamageResistanceValue);

        friend class Battler;
        friend class BattlerInstance;

    public:
        DamageResistances();
        DamageResistances(DamageResistanceMap&);
        DamageResistances(const DamageTypeMap&, const DamageInclinationMap&);
        DamageResistances(const DamageTypeMap&, const DamageInclinationMap&, const DamageResistanceMap&);

        const DamageResistanceMap& map() const;

        DamageResistanceValue GetValue(DamageTypeKey, DamageInclinationKey) const;
        DamageResistanceValue GetValue(DamageTypeInclination) const;

        class const_iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = DamageResistanceMap::value_type;
            using difference_type = DamageResistanceMap::difference_type;
            using pointer = DamageResistanceMap::const_pointer;
            using reference = DamageResistanceMap::const_reference;

            const_iterator(const DamageResistanceMap&);
            const_iterator(const DamageResistanceMap::const_iterator&);

            reference operator*() const;
            pointer operator->();
            const_iterator& operator++();
            const_iterator operator++(int);

            friend bool operator==(const const_iterator& it1, const const_iterator& it2) { return it1._itr == it2._itr; }
            friend bool operator!=(const const_iterator& it1, const const_iterator& it2) { return it1._itr != it2._itr; }

        private:
            DamageResistanceMap::const_iterator _itr;
        };

        const_iterator begin() const;
        const_iterator end() const;
    };
}
