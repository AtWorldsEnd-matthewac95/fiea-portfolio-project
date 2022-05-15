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

        ElementalAffinityValue SetValue(ElementalAffinityKey, ElementalAffinityValue);
        ElementalAffinityValue AddValue(ElementalAffinityKey, ElementalAffinityValue);

        friend class Battler;
        friend class BattlerInstance;

    public:
        ElementalAffinities();
        ElementalAffinities(ElementalAffinityMap&);
        ElementalAffinities(const SkillElementMap&, const DamageTypeMap&);
        ElementalAffinities(const SkillElementMap&, const DamageTypeMap&, const ElementalAffinityMap&);

        const ElementalAffinityMap& map() const;

        ElementalAffinityValue GetValue(SkillElementKey, DamageTypeKey) const;
        ElementalAffinityValue GetValue(const SkillElementGroup_shptr&, DamageTypeKey) const;
        ElementalAffinityValue GetValue(SkillElementKey) const;
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

        const_iterator begin() const;
        const_iterator end() const;
    };
}
