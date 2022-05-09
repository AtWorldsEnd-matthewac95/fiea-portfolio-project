#include "elementalaffinities.h"

namespace AWE {
    ElementalAffinities::ElementalAffinities() : _map(ElementalAffinityMap()) {}
    ElementalAffinities::ElementalAffinities(ElementalAffinityMap& map) : _map(ElementalAffinityMap(map)) {}
    ElementalAffinities::ElementalAffinities(const SkillElementMap& elements, const DamageTypeMap& dmgtypes) : _map(ElementalAffinityMap()) {
        for (const SkillElementMap::value_type& element : elements) {
            for (const DamageTypeMap::value_type& dmgtype : dmgtypes) {
                _map.insert(std::make_pair(ElementalAffinityKey(element.first, dmgtype.first), 0));
            }
        }
    }
    ElementalAffinities::ElementalAffinities(const SkillElementMap& elements, const DamageTypeMap& dmgtypes, const ElementalAffinityMap& values) : _map(ElementalAffinityMap()) {
        for (const SkillElementMap::value_type& element : elements) {
            for (const DamageTypeMap::value_type& dmgtype : dmgtypes) {
                auto key = ElementalAffinityKey(element.first, dmgtype.first);
                _map.insert(std::make_pair(key, values.contains(key) ? values.at(key) : 0));
            }
        }
    }

    ElementalAffinityValue ElementalAffinities::SetValue(ElementalAffinityKey key, ElementalAffinityValue value) {
        auto found = _map.find(key);
        if (found == _map.end()) {
            _map.insert(std::make_pair(key, value));
        } else {
            found->second = value;
        }

        return value;
    }

    ElementalAffinityValue ElementalAffinities::AddValue(ElementalAffinityKey key, ElementalAffinityValue value) {
        ElementalAffinityValue newval = value;

        auto found = _map.find(key);
        if (found == _map.end()) {
            _map.insert(std::make_pair(key, value));
        } else {
            newval += found->second;
            found->second = newval;
        }

        return newval;
    }

    const ElementalAffinityMap& ElementalAffinities::map() const { return _map; }

    ElementalAffinityValue ElementalAffinities::GetValue(SkillElementKey element, DamageTypeKey dmgtype) const {
        auto key = ElementalAffinityKey(element, dmgtype);
        return _map.contains(key) ? _map.at(key) : 0;
    }

    ElementalAffinityValue ElementalAffinities::GetValue(const SkillElementGroup_shptr& elegroup, DamageTypeKey dmgtype) const {
        if (!elegroup->IsValid()) {
            return 0;
        }

        ElementalAffinityValue sum = 0;

        for (const SkillElement_shptr& ele : *elegroup) {
            sum += GetValue(ele->abrvlong(), dmgtype);
        }

        return sum;
    }

    ElementalAffinityValue ElementalAffinities::GetValue(SkillElementKey element) const {
        ElementalAffinityValue sum = 0;

        for (const ElementalAffinityMap::value_type& itr : _map) {
            if (itr.first.first == element) {
                sum += itr.second;
            }
        }

        return sum;
    }

    ElementalAffinityValue ElementalAffinities::GetValue(const SkillElementGroup_shptr& elegroup) const {
        if (!elegroup->IsValid()) {
            return 0;
        }

        SkillElementList elements = std::move(elegroup->elements());
        ElementalAffinityValue sum = 0;

        for (ElementalAffinityMap::value_type itr : _map) {
            for (const SkillElement_shptr& element : elements) {
                if (itr.first.first == element->abrvlong()) {
                    sum += itr.second;
                    break;
                }
            }
        }

        return sum;
    }

    // iterator

    ElementalAffinities::const_iterator::const_iterator(const ElementalAffinityMap& map) : _itr(std::begin(map)) {}
    ElementalAffinities::const_iterator::const_iterator(const ElementalAffinityMap::const_iterator& itr) : _itr(itr) {}

    ElementalAffinities::const_iterator::reference ElementalAffinities::const_iterator::operator*() const { return _itr.operator*(); }
    ElementalAffinities::const_iterator::pointer ElementalAffinities::const_iterator::operator->() { return _itr.operator->(); }
    ElementalAffinities::const_iterator& ElementalAffinities::const_iterator::operator++() { _itr++; return *this; }
    ElementalAffinities::const_iterator ElementalAffinities::const_iterator::operator++(int) { const_iterator prev = *this; this->operator++(); return prev; }

    ElementalAffinities::const_iterator ElementalAffinities::begin() const { return const_iterator(_map); }
    ElementalAffinities::const_iterator ElementalAffinities::end() const { return const_iterator(std::end(_map)); }
}
