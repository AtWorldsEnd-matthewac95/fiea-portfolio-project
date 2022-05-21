#pragma once
#include <map>
#include "damagetype.h"
#include "skillelement.h"
#include "skillelementgroup.h"

namespace AWE {
    // Forward declarations, defined in battler.h
    class Battler;
    class BattlerInstance;

    typedef int ElementalAffinityValue;
    typedef std::pair<SkillElementKey, DamageTypeKey> ElementalAffinityKey;

    struct ElementalAffinityKey_comp {
        bool operator()(const ElementalAffinityKey& left, const ElementalAffinityKey& right) const {
            if (left.first == right.first) {
                return left.second < right.second;
            }
            return left.first < right.first;
        }
    };

    typedef std::map<ElementalAffinityKey, ElementalAffinityValue, ElementalAffinityKey_comp> ElementalAffinityMap;

    /// <summary>
    /// Represents a battler's affinities to specific types of elemental damage. Note that "types of elemental damage" here actually means
    /// element-type pairs, e.g. sword slashing, wand arcane, etc.
    /// Also note that ElementalAffinityValue is not unsigned - there can be negative affinities.
    /// </summary>
    class ElementalAffinities {
    private:
        ElementalAffinityMap _map;

        /// <summary>
        /// Sets the value of the given affinity. The given affinity will be created if it did not exist.
        /// </summary>
        /// <returns>New value of the given affinity. Note this does not return the old value.</returns>
        ElementalAffinityValue SetValue(ElementalAffinityKey, ElementalAffinityValue);
        /// <summary>
        /// Adds the given value to the current value of the given affinity. Value can be negative. The given affinity will be created if it did not exist.
        /// </summary>
        /// <returns>New value of the given affinity after the addition.</returns>
        ElementalAffinityValue AddValue(ElementalAffinityKey, ElementalAffinityValue);

        friend class Battler;
        friend class BattlerInstance;

    public:
        ElementalAffinities();
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="">Map to use for affinities. Note this parameter is moved into the initialized object.</param>
        ElementalAffinities(ElementalAffinityMap&);
        /// <summary>
        /// Constructor. Initializes affinities for all elements and damage types given, and sets them to 0.
        /// </summary>
        ElementalAffinities(const SkillElementMap&, const DamageTypeMap&);
        /// <summary>
        /// Constructor. Initializes affinities for all elements and damage types given, and copies their values from the given map. None of the given maps are modified.
        /// </summary>
        ElementalAffinities(const SkillElementMap&, const DamageTypeMap&, const ElementalAffinityMap&);

        /// <returns>const reference to the internal map.</returns>
        const ElementalAffinityMap& map() const;

        /// <returns>Value of the affinity for the given skill element and damage type.</returns>
        ElementalAffinityValue GetValue(SkillElementKey, DamageTypeKey) const;
        /// <returns>Total sum value of all affinities for the given damage type and each skill element in the given group.</returns>
        ElementalAffinityValue GetValue(const SkillElementGroup_shptr&, DamageTypeKey) const;
        /// <returns>Total sum value of all affinities for the given skill element.</returns>
        ElementalAffinityValue GetValue(SkillElementKey) const;
        /// <returns>Total sum value of all affinities for each skill element in the given group.</returns>
        ElementalAffinityValue GetValue(const SkillElementGroup_shptr&) const;

        class const_iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = ElementalAffinityMap::value_type;
            using difference_type = ElementalAffinityMap::difference_type;
            using pointer = ElementalAffinityMap::const_pointer;
            using reference = ElementalAffinityMap::const_reference;

            const_iterator(const ElementalAffinityMap&);
            const_iterator(const ElementalAffinityMap::const_iterator&);

            reference operator*() const;
            pointer operator->();
            const_iterator& operator++();
            const_iterator operator++(int);

            friend bool operator==(const const_iterator& it1, const const_iterator& it2) { return it1._itr == it2._itr; }
            friend bool operator!=(const const_iterator& it1, const const_iterator& it2) { return it1._itr != it2._itr; }

        private:
            ElementalAffinityMap::const_iterator _itr;
        };

        /// <returns>const iterator to the first affinity value pair in this object.</returns>
        const_iterator begin() const;
        /// <returns>const iterator to the spot in memory behind the last affinity value pair in this object. Attempting access from this address will result in undefined behavior.</returns>
        const_iterator end() const;
    };
}
