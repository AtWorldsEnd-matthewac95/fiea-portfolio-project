#pragma once
#include <map>
#include "lovpairs.h"
#include "equipment.h"
#include "equipmenttype.h"

namespace AWE {
    // Forward declarations, defined in battler.h
    class Battler;
    class BattlerInstance;

    /// <summary>
    /// There can be multiple equipment of the same type equipped. This distinguishes which equipment of a particular type is being referenced.
    /// </summary>
    typedef unsigned int EquipmentSlotIndex;
    /// <summary>
    /// There can be multiple equipment of the same type equipped. This distinguishes which equipment of a particular type is being referenced.
    /// </summary>
    typedef std::pair<EquipmentTypeKey, EquipmentSlotIndex> EquipmentSlotKey;

    struct EquipmentSlotKey_comp {
        bool operator()(const EquipmentSlotKey& left, const EquipmentSlotKey& right) const {
            if (left.first == right.first) {
                return left.second < right.second;
            }
            return left.first < right.first;
        }
    };

    /// <summary>
    /// Represents equipment slots. Note that an equipment slot can be empty, but the map record for that slot may still be there. Use the EquipmentSlots class to use this map properly.
    /// </summary>
    typedef std::map<EquipmentSlotKey, Equipment_shptr, EquipmentSlotKey_comp> EquipmentSlotMap;

    /// <summary>
    /// Wraps a map of equiment and provides several utility functions to properly populate it.
    /// </summary>
    class EquipmentSlots {
    private:
        EquipmentSlotMap _map;
        EquipmentTypeCountMap _counts;

        EquipmentSlotKey AddSlot(EquipmentTypeKey);
        EquipmentSlotKey AddSlot(Equipment_shptr);
        EquipmentSlotKey AddSlot(EquipmentTypeKey, Equipment_shptr);
        Equipment_shptr RemoveSlot(EquipmentTypeKey, EquipmentSlotIndex);
        Equipment_shptr RemoveSlot(EquipmentSlotKey);
        Equipment_shptr RemoveSlot(EquipmentTypeKey);
        Equipment_shptr Equip(EquipmentSlotIndex, Equipment_shptr);
        Equipment_shptr Equip(Equipment_shptr);
        Equipment_shptr Equip(Equipment_shptr equipment, EquipmentSlotKey &newkey);
        Equipment_shptr ForceEquip(Equipment_shptr equipment, int maxCountToAdd = -1);
        Equipment_shptr ForceEquip(Equipment_shptr equipment, EquipmentSlotKey& newkey, int maxCountToAdd = -1);
        Equipment_shptr Unequip(EquipmentTypeKey);
        Equipment_shptr Unequip(EquipmentTypeKey, EquipmentSlotIndex);
        Equipment_shptr Unequip(EquipmentSlotKey);

        friend class Battler;

    public:
        EquipmentSlots();
        EquipmentSlots(EquipmentSlotMap&);
        EquipmentSlots(EquipmentTypeCountMap&);

        static const EquipmentSlotKey INVALID_KEY;

        const EquipmentSlotMap& map() const;

        bool HasSlot(EquipmentTypeKey, EquipmentSlotIndex) const;
        bool HasSlot(EquipmentSlotKey) const;
        bool IsSlotFilled(EquipmentTypeKey, EquipmentSlotIndex) const;
        bool IsSlotFilled(EquipmentSlotKey) const;
        int GetCount(EquipmentTypeKey) const;
        Equipment_shptr GetEquipment(EquipmentTypeKey, EquipmentSlotIndex) const;
        Equipment_shptr GetEquipment(EquipmentSlotKey) const;
        bool FindFirstEmptySlot(EquipmentTypeKey eqtype, EquipmentSlotKey& output) const;
        bool FindFirstEmptyOrLastSlot(EquipmentTypeKey eqtype, EquipmentSlotKey& output) const;

        class const_iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = EquipmentSlotMap::value_type;
            using difference_type = EquipmentSlotMap::difference_type;
            using pointer = EquipmentSlotMap::const_pointer;
            using reference = EquipmentSlotMap::const_reference;

            const_iterator(const EquipmentSlotMap&);
            const_iterator(const EquipmentSlotMap::const_iterator&);

            reference operator*() const;
            pointer operator->();
            const_iterator& operator++();
            const_iterator operator++(int);

            friend bool operator==(const const_iterator& it1, const const_iterator& it2) { return it1._itr == it2._itr; }
            friend bool operator!=(const const_iterator& it1, const const_iterator& it2) { return it1._itr != it2._itr; }

        private:
            EquipmentSlotMap::const_iterator _itr;
        };

        const_iterator begin() const;
        const_iterator end() const;
    };
}
