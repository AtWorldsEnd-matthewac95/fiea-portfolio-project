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
        DamageType();
        DamageType(std::string name, ABRV abrv);

        static const DamageType INVALID;

        bool Equals(const AbbreviatedKey& other) const override;
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
