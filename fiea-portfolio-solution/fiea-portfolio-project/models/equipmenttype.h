#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    class EquipmentType : public AbbreviatedKey {
    public:
        EquipmentType();
        EquipmentType(std::string name, ABRV abrv);

        static const EquipmentType INVALID;

        bool Equals(const AbbreviatedKey& other) const override;
        bool Equals(const EquipmentType& other) const;
    };

    typedef ABRV_long EquipmentTypeKey;
    typedef std::shared_ptr<EquipmentType> EquipmentType_shptr;
    typedef std::unordered_map<EquipmentTypeKey, EquipmentType_shptr> EquipmentTypeMap;
}
