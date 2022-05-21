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
    /// There can be multiple equipment of the same type equipped. This distinguishes which equipment of a particular type is being referenced. Indicies start at 1.
    /// </summary>
    typedef unsigned int EquipmentSlotIndex;
    /// <summary>
    /// There can be multiple equipment of the same type equipped. This distinguishes which equipment of a particular type is being referenced. Indicies start at 1.
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

        /// <summary>
        /// Creates an empty equipment slot of the given type.
        /// </summary>
        /// <returns>The key for the new slot.</returns>
        EquipmentSlotKey AddSlot(EquipmentTypeKey);
        /// <summary>
        /// Creates an appropriate equipment slot for the given equipment, then equips the equipment to that new slot.
        /// </summary>
        /// <returns>The key for the new slot.</returns>
        EquipmentSlotKey AddSlot(Equipment_shptr);
        /// <summary>
        /// Creates an equipment slot of the given type, then equips the given equipment if it's of the correct equipment type.
        /// </summary>
        /// <returns>The key for the new slot.</returns>
        EquipmentSlotKey AddSlot(EquipmentTypeKey, Equipment_shptr);
        /// <summary>
        /// Removes the slot with the given equipment type at the given index. If such a slot does not exist, this method does nothing.
        /// </summary>
        /// <returns>The removed equipment at the removed slot, or an empty pointer if that slot was empty or did not exist.</returns>
        Equipment_shptr RemoveSlot(EquipmentTypeKey, EquipmentSlotIndex);
        /// <summary>
        /// Removes the slot with the given equipment type at the given index. If such a slot does not exist, this method does nothing.
        /// </summary>
        /// <returns>The removed equipment at the removed slot, or an empty pointer if that slot was empty or did not exist.</returns>
        Equipment_shptr RemoveSlot(EquipmentSlotKey);
        /// <summary>
        /// Removes the last slot of the given equipment type. If such a slot does not exist, this method does nothing.
        /// </summary>
        /// <returns>The removed equipment at the removed slot, or an empty pointer if that slot was empty or did not exist.</returns>
        Equipment_shptr RemoveSlot(EquipmentTypeKey);
        /// <summary>
        /// Attempts to equip the given equipment at the given slot. If that slot does not exist, this method does nothing.
        /// If an equipment was already equipped to this slot, that equipment is removed and the given equipment replaces it.
        /// </summary>
        /// <returns>The previously equipped equipment at this slot, or an empty pointer if the slot was empty or did not exist.</returns>
        Equipment_shptr Equip(EquipmentSlotIndex, Equipment_shptr);
        /// <summary>
        /// Attempts to equip the given equipment at the first empty slot. If there are no empty slots, the equipment is equipped at the
        /// last slot of that type and the equipment at that slot is replaced with the given equipment. If there are no slots at all for
        /// the given equipment type, this method does nothing.
        /// </summary>
        /// <returns>The previously equipped equipment at the equipped slot, or an empty pointer if the slot was empty or did not exist.</returns>
        Equipment_shptr Equip(Equipment_shptr);
        /// <summary>
        /// Attempts to equip the given equipment at the first empty slot. If there are no empty slots, the equipment is equipped at the
        /// last slot of that type and the equipment at that slot is replaced with the given equipment. If there are no slots at all for
        /// the given equipment type, this method does nothing.
        /// </summary>
        /// <param name="newkey">Output parameter. If the given equipment was equipped to a slot, this will be set to the new slot value. Otherwise, this will be set to INVALID_KEY.</param>
        /// <returns>The previously equipped equipment at the equipped slot, or an empty pointer if the slot was empty or did not exist.</returns>
        Equipment_shptr Equip(Equipment_shptr equipment, EquipmentSlotKey &newkey);
        /// <summary>
        /// Attempts to equip the given equipment to the first available empty slot. If none exists, depending on the `maxCountToAdd` parameter,
        /// a new slot may be added to which the equipment will be equipped. If a slot is not added and no empty slots exist, then the last slot
        /// of that type will have its equipment replaced with the given equipment.
        /// </summary>
        /// <param name="maxCountToAdd">If this is negative, then a slot will always be added if no empty slots are available. Otherwise, a slot will only be added if there are less than this many slots.</param>
        /// <returns></returns>
        Equipment_shptr ForceEquip(Equipment_shptr equipment, int maxCountToAdd = -1);
        /// <summary>
        /// Attempts to equip the given equipment to the first available empty slot. If none exists, depending on the `maxCountToAdd` parameter,
        /// a new slot may be added to which the equipment will be equipped. If a slot is not added and no empty slots exist, then the last slot
        /// of that type will have its equipment replaced with the given equipment.
        /// </summary>
        /// <param name="newkey">Output parameter. This will be set to the new slot value of the equipped equipment.</param>
        /// <param name="maxCountToAdd">If this is negative, then a slot will always be added if no empty slots are available. Otherwise, a slot will only be added if there are less than this many slots.</param>
        /// <returns></returns>
        Equipment_shptr ForceEquip(Equipment_shptr equipment, EquipmentSlotKey& newkey, int maxCountToAdd = -1);
        /// <summary>
        /// Unequips the equipment with the highest index of the given type. Ignores slots which are empty.
        /// </summary>
        /// <returns>The unequipped equipment, or an empty pointer if no such equipment was equipped.</returns>
        Equipment_shptr Unequip(EquipmentTypeKey);
        /// <summary>
        /// Unequips the equipment at the given slot. If that slot does not exist or is empty, this method does nothing.
        /// </summary>
        /// <returns>The unequipped equipment, or an empty pointer if no such equipment was equipped.</returns>
        Equipment_shptr Unequip(EquipmentTypeKey, EquipmentSlotIndex);
        /// <summary>
        /// Unequips the equipment at the given slot. If that slot does not exist or is empty, this method does nothing.
        /// </summary>
        /// <returns>The unequipped equipment, or an empty pointer if no such equipment was equipped.</returns>
        Equipment_shptr Unequip(EquipmentSlotKey);

        friend class Battler;

    public:
        EquipmentSlots();
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="">An existing map to use for this object. Note the map is moved into this object.</param>
        EquipmentSlots(EquipmentSlotMap&);
        /// <summary>
        /// Constructor.
        /// </summary>
        /// <param name="">Creates equipment slots based on the type map, leaving each slot empty. Note the given map is moved into this object.</param>
        EquipmentSlots(EquipmentTypeCountMap&);

        /// <summary>
        /// An equipment slot with this key is considered invalid.
        /// </summary>
        static const EquipmentSlotKey INVALID_KEY;

        /// <returns>const reference to the internal map.</returns>
        const EquipmentSlotMap& map() const;

        /// <returns>true if the given slot exists on this object, false otherwise. Note if this returns true that does not necessarily mean the slot is filled.</returns>
        bool HasSlot(EquipmentTypeKey, EquipmentSlotIndex) const;
        /// <returns>true if the given slot exists on this object, false otherwise. Note if this returns true that does not necessarily mean the slot is filled.</returns>
        bool HasSlot(EquipmentSlotKey) const;
        /// <returns>true if the given slot exists on this object and has an equipment in it, false otherwise.</returns>
        bool IsSlotFilled(EquipmentTypeKey, EquipmentSlotIndex) const;
        /// <returns>true if the given slot exists on this object and has an equipment in it, false otherwise.</returns>
        bool IsSlotFilled(EquipmentSlotKey) const;
        /// <returns>The number of slots this object has for the given type.</returns>
        int GetCount(EquipmentTypeKey) const;
        /// <returns>The equipment at the given slot. If that slot does not exist or is empty, an empty pointer is returned.</returns>
        Equipment_shptr GetEquipment(EquipmentTypeKey, EquipmentSlotIndex) const;
        /// <returns>The equipment at the given slot. If that slot does not exist or is empty, an empty pointer is returned.</returns>
        Equipment_shptr GetEquipment(EquipmentSlotKey) const;
        /// <summary>
        /// Finds the first available empty slot of the given equipment type.
        /// </summary>
        /// <param name="eqtype">The type to search for.</param>
        /// <param name="output">Output parameter. If an empty slot is found this will be set to that slot key, otherwise it will be set to INVALID_KEY.</param>
        /// <returns>true if an empty slot was found, false otherwise.</returns>
        bool FindFirstEmptySlot(EquipmentTypeKey eqtype, EquipmentSlotKey& output) const;
        /// <summary>
        /// Finds the first available empty slot of the given equipment type, or the last slot of that type if there are no empty slots.
        /// </summary>
        /// <param name="eqtype">The type to search for.</param>
        /// <param name="output">Output parameter. Will be set to the first empty slot of the type if it exists, or the last slot of the type if it exists, or INVALID_KEY otherwise.</param>
        /// <returns>true if a slot was found, false otherwise.</returns>
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

        /// <returns>const iterator to the first equipment in this object.</returns>
        const_iterator begin() const;
        /// <returns>const iterator to the spot in memory behind the last equipment in this object. Attempting access from this address will result in undefined behavior.</returns>
        const_iterator end() const;
    };
}
