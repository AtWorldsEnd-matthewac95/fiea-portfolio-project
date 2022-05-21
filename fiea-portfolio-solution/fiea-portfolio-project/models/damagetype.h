#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    /// <summary>
    /// Represents one of the possible types of damage. Examples: pyretic, slashing, arcane, etc.
    /// Note this does not cover things like physical damage and magical damage; that's represented with DamageInclination.
    /// </summary>
    class DamageType : public AbbreviatedKey {
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        DamageType();
        DamageType(std::string name, ABRV abrv);

        /// <summary>
        /// Any DamageType which is equivalent to this object is invalid.
        /// </summary>
        static const DamageType INVALID;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other DamageType object.</param>
        /// <returns>Equivalence to the other DamageType.</returns>
        bool Equals(const DamageType& other) const;
    };

    /// <summary>
    /// Used to index DamageType objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long DamageTypeKey;
    /// <summary>
    /// Represents one of the possible types of damage. Examples: pyretic, slashing, arcane, etc.
    /// Note this does not cover things like physical damage and magical damage; that's represented with DamageInclination.
    /// </summary>
    typedef std::shared_ptr<DamageType> DamageType_shptr;
    typedef std::unordered_map<DamageTypeKey, DamageType_shptr> DamageTypeMap;
}
