#include "equipmenttype.h"

namespace AWE {
    EquipmentType::EquipmentType() : AbbreviatedKey() {}
    EquipmentType::EquipmentType(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv) {}

    const EquipmentType EquipmentType::INVALID = EquipmentType();

    bool EquipmentType::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const EquipmentType* converted = dynamic_cast<const EquipmentType*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool EquipmentType::Equals(const EquipmentType& other) const {
        return _abrv.Equals(other._abrv);
    }
}
