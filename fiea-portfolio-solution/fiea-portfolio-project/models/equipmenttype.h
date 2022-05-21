#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    /// <summary>
    /// Represents one of the possible types of equipment.
    /// </summary>
    class EquipmentType : public AbbreviatedKey {
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        EquipmentType();
        EquipmentType(std::string name, ABRV abrv);

        /// <summary>
        /// Any EquipmentType which is equivalent to this object is invalid.
        /// </summary>
        static const EquipmentType INVALID;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other EquipmentType object.</param>
        /// <returns>Equivalence to the other EquipmentType.</returns>
        bool Equals(const EquipmentType& other) const;
    };

    /// <summary>
    /// Used to index EquipmentType objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long EquipmentTypeKey;
    /// <summary>
    /// Represents one of the possible types of equipment.
    /// </summary>
    typedef std::shared_ptr<EquipmentType> EquipmentType_shptr;
    typedef std::unordered_map<EquipmentTypeKey, EquipmentType_shptr> EquipmentTypeMap;
}
