#include "equipmentslots.h"

namespace AWE {
    EquipmentSlots::EquipmentSlots() : _map(EquipmentSlotMap()) {}
    EquipmentSlots::EquipmentSlots(EquipmentSlotMap& map) : _map(std::move(map)) {}
    EquipmentSlots::EquipmentSlots(EquipmentTypeCountMap& schema) : _map(EquipmentSlotMap()), _counts(std::move(schema)) {
        for (EquipmentTypeCountMap::value_type count : _counts) {
            for (unsigned int i = 1; i <= count.second; i++) {
                _map.insert(std::make_pair(EquipmentSlotKey(count.first, i), Equipment_shptr()));
            }
        }
    }

    const EquipmentSlotKey EquipmentSlots::INVALID_KEY = EquipmentSlotKey(0, 0);

    const EquipmentSlotMap& EquipmentSlots::map() const { return _map; }

    EquipmentSlotKey EquipmentSlots::AddSlot(EquipmentTypeKey eqtype) { return AddSlot(eqtype, Equipment_shptr()); }
    EquipmentSlotKey EquipmentSlots::AddSlot(Equipment_shptr equipment) { return (equipment && equipment->equipmentType()) ? AddSlot(equipment->equipmentType()->abrvlong(), equipment) : INVALID_KEY; }
    EquipmentSlotKey EquipmentSlots::AddSlot(EquipmentTypeKey eqtype, Equipment_shptr equipment) {
        int newcount = 1;
        auto found = _counts.find(eqtype);
        if (found == _counts.end()) {
            _counts.insert(std::make_pair(eqtype, newcount));
        } else {
            newcount = found->second + 1;
            found->second = newcount;
        }

        EquipmentSlotKey newSlotKey = EquipmentSlotKey(eqtype, newcount);
        _map.insert(std::make_pair(newSlotKey, (equipment && equipment->equipmentType() && eqtype == equipment->equipmentType()->abrvlong()) ? std::move(equipment) : Equipment_shptr()));
        return newSlotKey;
    }

    Equipment_shptr EquipmentSlots::RemoveSlot(EquipmentTypeKey eqtype, EquipmentSlotIndex index) { return RemoveSlot(EquipmentSlotKey(eqtype, index)); }
    Equipment_shptr EquipmentSlots::RemoveSlot(EquipmentSlotKey key) {
        Equipment_shptr removed;

        auto itr = _map.find(key);
        if (itr != _map.end()) {
            removed = std::move(itr->second);

            int count = _counts.at(key.first);
            if (count != key.second) {
                for (int i = key.second; i < count; i++) {
                    itr = _map.find(EquipmentSlotKey(key.first, i + 1));
                    _map.find(EquipmentSlotKey(key.first, i))->second = std::move(itr->second);
                }
            }

            _map.erase(itr);
            _counts.find(key.first)->second -= 1;
        }

        return removed;
    }

    Equipment_shptr EquipmentSlots::RemoveSlot(EquipmentTypeKey eqtype) {
        Equipment_shptr removed;

        auto found = _counts.find(eqtype);
        if (found != _counts.end()) {
            auto key = EquipmentSlotKey(eqtype, found->second);
            removed = std::move(_map.at(key));
            _map.erase(_map.find(key));
            found->second -= 1;
        }

        return removed;
    }

    Equipment_shptr EquipmentSlots::Equip(EquipmentSlotIndex index, Equipment_shptr equipment) {
        if (!equipment || !equipment->equipmentType()) {
            return Equipment_shptr();
        }

        EquipmentSlotKey key = EquipmentSlotKey(equipment->equipmentType()->abrvlong(), index);
        auto found = _map.find(key);
        if (found == _map.end()) {
            return Equipment_shptr();
        }

        Equipment_shptr unequipped = std::move(found->second);
        found->second = std::move(equipment);
        return unequipped;
    }

    Equipment_shptr EquipmentSlots::Equip(Equipment_shptr equipment) { EquipmentSlotKey _; return Equip(equipment, _); }
    Equipment_shptr EquipmentSlots::Equip(Equipment_shptr equipment, EquipmentSlotKey& newkey) {
        if (!equipment || !equipment->equipmentType()) {
            return Equipment_shptr();
        }

        return FindFirstEmptyOrLastSlot(equipment->equipmentType()->abrvlong(), newkey) ? Equip(newkey.second, equipment) : Equipment_shptr();
    }

    Equipment_shptr EquipmentSlots::ForceEquip(Equipment_shptr equipment, int maxCountToAdd) { EquipmentSlotKey _; return ForceEquip(equipment, _, maxCountToAdd); }
    Equipment_shptr EquipmentSlots::ForceEquip(Equipment_shptr equipment, EquipmentSlotKey& newkey, int maxCountToAdd) {
        if (!equipment || !equipment->equipmentType()) {
            return Equipment_shptr();
        }

        Equipment_shptr unequipped;

        EquipmentTypeKey eqtype = equipment->equipmentType()->abrvlong();
        auto count = _counts.find(eqtype);
        if (count == _counts.end() || (!FindFirstEmptySlot(eqtype, newkey) && (maxCountToAdd < 0 || static_cast<int>(count->second) < maxCountToAdd))) {
            newkey = AddSlot(equipment);
        } else {
            unequipped = newkey.first == INVALID_KEY.first ? Equip(equipment, newkey) : Equip(newkey.second, equipment);
        }

        return unequipped;
    }

    Equipment_shptr EquipmentSlots::Unequip(EquipmentTypeKey eqtype) {
        auto countfound = _counts.find(eqtype);
        if (countfound == _counts.end()) {
            return Equipment_shptr();
        }

        Equipment_shptr unequipped;

        for (EquipmentSlotIndex i = countfound->second; i >= 0; i--) {
            auto key = EquipmentSlotKey(eqtype, i);
            auto slotfound = _map.find(key);
            if (slotfound != _map.end() && slotfound->second) {
                return Unequip(key);
            }
        }

        return unequipped;
    }

    Equipment_shptr EquipmentSlots::Unequip(EquipmentTypeKey eqtype, EquipmentSlotIndex index) { return Unequip(EquipmentSlotKey(eqtype, index)); }
    Equipment_shptr EquipmentSlots::Unequip(EquipmentSlotKey key) {
        auto found = _map.find(key);
        if (found == _map.end()) {
            return Equipment_shptr();
        }

        Equipment_shptr unequipped = std::move(found->second);
        found->second.reset();
        return unequipped;
    }

    bool EquipmentSlots::HasSlot(EquipmentTypeKey eqtype, EquipmentSlotIndex index) const { return HasSlot(EquipmentSlotKey(eqtype, index)); }
    bool EquipmentSlots::HasSlot(EquipmentSlotKey key) const { return _map.contains(key); }

    bool EquipmentSlots::IsSlotFilled(EquipmentTypeKey eqtype, EquipmentSlotIndex index) const { return IsSlotFilled(EquipmentSlotKey(eqtype, index)); }
    bool EquipmentSlots::IsSlotFilled(EquipmentSlotKey key) const { return HasSlot(key) && _map.at(key); }

    int EquipmentSlots::GetCount(EquipmentTypeKey eqtype) const { return _counts.contains(eqtype) ? _counts.at(eqtype) : 0; }

    bool EquipmentSlots::FindFirstEmptySlot(EquipmentTypeKey eqtype, EquipmentSlotKey& output) const {
        auto count = _counts.find(eqtype);
        if (count == _counts.end()) {
            output = INVALID_KEY;
            return false;
        }

        EquipmentSlotIndex index;
        for (index = 1; index <= count->second; index++) {
            if (!IsSlotFilled(eqtype, index)) {
                output = EquipmentSlotKey(eqtype, index);
                return true;
            }
        }

        output = INVALID_KEY;
        return false;
    }

    bool EquipmentSlots::FindFirstEmptyOrLastSlot(EquipmentTypeKey eqtype, EquipmentSlotKey& output) const {
        auto count = _counts.find(eqtype);
        if (count == _counts.end()) {
            output = INVALID_KEY;
            return false;
        }

        EquipmentSlotIndex index;
        for (index = 1; index < count->second; index++) {
            if (!IsSlotFilled(eqtype, index)) {
                break;
            }
        }

        output = EquipmentSlotKey(eqtype, index);
        return true;
    }

    Equipment_shptr EquipmentSlots::GetEquipment(EquipmentTypeKey eqtype, EquipmentSlotIndex index) const { return GetEquipment(EquipmentSlotKey(eqtype, index)); }
    Equipment_shptr EquipmentSlots::GetEquipment(EquipmentSlotKey key) const { return HasSlot(key) ? _map.at(key) : Equipment_shptr(); }

    // iterator

    EquipmentSlots::const_iterator::const_iterator(const EquipmentSlotMap& map) : _itr(std::begin(map)) {}
    EquipmentSlots::const_iterator::const_iterator(const EquipmentSlotMap::const_iterator& itr) : _itr(itr) {}

    EquipmentSlots::const_iterator::reference EquipmentSlots::const_iterator::operator*() const { return _itr.operator*(); }
    EquipmentSlots::const_iterator::pointer EquipmentSlots::const_iterator::operator->() { return _itr.operator->(); }
    EquipmentSlots::const_iterator& EquipmentSlots::const_iterator::operator++() { _itr++; return *this; }
    EquipmentSlots::const_iterator EquipmentSlots::const_iterator::operator++(int) { const_iterator prev = *this; this->operator++(); return prev; }

    EquipmentSlots::const_iterator EquipmentSlots::begin() const { return const_iterator(_map); }
    EquipmentSlots::const_iterator EquipmentSlots::end() const { return const_iterator(std::end(_map)); }
}
