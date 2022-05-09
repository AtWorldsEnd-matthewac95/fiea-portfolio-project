#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    class DamageType : public AbbreviatedKey {
    public:
        DamageType();
        DamageType(std::string name, ABRV abrv);

        static const DamageType INVALID;

        bool Equals(const AbbreviatedKey& other) const override;
        bool Equals(const DamageType& other) const;
    };

    typedef ABRV_long DamageTypeKey;
    typedef std::shared_ptr<DamageType> DamageType_shptr;
    typedef std::unordered_map<DamageTypeKey, DamageType_shptr> DamageTypeMap;
}
