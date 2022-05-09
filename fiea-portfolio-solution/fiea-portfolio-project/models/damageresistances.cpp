#include "damageresistances.h"

namespace AWE {
    DamageResistances::DamageResistances() : _map(DamageResistanceMap()) {}
    DamageResistances::DamageResistances(DamageResistanceMap& map) : _map(std::move(map)) {}
    DamageResistances::DamageResistances(const DamageTypeMap& dmgtypes, const DamageInclinationMap& dmgincls) : _map(DamageResistanceMap()) {
        for (const DamageTypeMap::value_type& dmgtype : dmgtypes) {
            for (const DamageInclinationMap::value_type& dmgincl : dmgincls) {
                _map.insert(std::make_pair(DamageTypeInclination(dmgtype.first, dmgincl.first), 0));
            }
        }
    }
    DamageResistances::DamageResistances(const DamageTypeMap& dmgtypes, const DamageInclinationMap& dmgincls, const DamageResistanceMap& values) : _map(DamageResistanceMap()) {
        for (const DamageTypeMap::value_type& dmgtype : dmgtypes) {
            for (const DamageInclinationMap::value_type& dmgincl : dmgincls) {
                auto key = DamageTypeInclination(dmgtype.first, dmgincl.first);
                _map.insert(std::make_pair(key, values.contains(key) ? values.at(key) : 0));
            }
        }
    }

    DamageResistanceValue DamageResistances::SetValue(DamageTypeInclination key, DamageResistanceValue value) {
        auto found = _map.find(key);
        if (found == _map.end()) {
            _map.insert(std::make_pair(key, value));
        } else {
            found->second = value;
        }

        return value;
    }

    DamageResistanceValue DamageResistances::AddValue(DamageTypeInclination key, DamageResistanceValue value) {
        DamageResistanceValue newval = value;

        auto found = _map.find(key);
        if (found == _map.end()) {
            _map.insert(std::make_pair(key, value));
        } else {
            newval += found->second;
            found->second = newval;
        }

        return newval;
    }

    const DamageResistanceMap& DamageResistances::map() const { return _map; }

    DamageResistanceValue DamageResistances::GetValue(DamageTypeKey dmgtype, DamageInclinationKey dmgincl) const { return GetValue(DamageTypeInclination(dmgtype, dmgincl)); }
    DamageResistanceValue DamageResistances::GetValue(DamageTypeInclination key) const { return _map.contains(key) ? _map.at(key) : 0; }

    // iterator

    DamageResistances::const_iterator::const_iterator(const DamageResistanceMap& map) : _itr(std::begin(map)) {}
    DamageResistances::const_iterator::const_iterator(const DamageResistanceMap::const_iterator& itr) : _itr(itr) {}

    DamageResistances::const_iterator::reference DamageResistances::const_iterator::operator*() const { return _itr.operator*(); }
    DamageResistances::const_iterator::pointer DamageResistances::const_iterator::operator->() { return _itr.operator->(); }
    DamageResistances::const_iterator& DamageResistances::const_iterator::operator++() { _itr++; return *this; }
    DamageResistances::const_iterator DamageResistances::const_iterator::operator++(int) { const_iterator prev = *this; this->operator++(); return prev; }

    DamageResistances::const_iterator DamageResistances::begin() const { return const_iterator(_map); }
    DamageResistances::const_iterator DamageResistances::end() const { return const_iterator(std::end(_map)); }
}
