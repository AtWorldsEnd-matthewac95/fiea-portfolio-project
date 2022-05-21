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

        /// <summary>
        /// Sets the value of the given resistance. The given resistance will be created if it did not exist.
        /// </summary>
        /// <returns>New value of the given resistance. Note this does not return the old value.</returns>
        DamageResistanceValue SetValue(DamageTypeInclination, DamageResistanceValue);
        /// <summary>
        /// Adds the given value to the current value of the given resistance. Value can be negative. The given resistance will be created if it did not exist.
        /// </summary>
        /// <returns>New value of the given resistance after the addition.</returns>
        DamageResistanceValue AddValue(DamageTypeInclination, DamageResistanceValue);

        friend class Battler;
        friend class BattlerInstance;

    public:
        DamageResistances();
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="">Map to use for resistances. Note this parameter is moved into the initialized object.</param>
        DamageResistances(DamageResistanceMap&);
        /// <summary>
        /// Constructor. Initializes resistances for all types and inclinations given, and sets them to 0.
        /// </summary>
        DamageResistances(const DamageTypeMap&, const DamageInclinationMap&);
        /// <summary>
        /// Constructor. Initializes resistances for all types and inclinations given, and copies their values from the given map. None of the given maps are modified.
        /// </summary>
        DamageResistances(const DamageTypeMap&, const DamageInclinationMap&, const DamageResistanceMap&);

        /// <returns>const reference to the internal map.</returns>
        const DamageResistanceMap& map() const;

        /// <returns>Value of the resistance for the given type and inclination.</returns>
        DamageResistanceValue GetValue(DamageTypeKey, DamageInclinationKey) const;
        /// <returns>Value of the resistance for the given type and inclination.</returns>
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

        /// <returns>const iterator to the first resistance value pair in this object.</returns>
        const_iterator begin() const;
        /// <returns>const iterator to the spot in memory behind the last resistance value pair in this object. Attempting access from this address will result in undefined behavior.</returns>
        const_iterator end() const;
    };
}
