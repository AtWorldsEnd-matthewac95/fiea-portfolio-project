#include "damagetype.h"

namespace AWE {
    DamageType::DamageType() : AbbreviatedKey() {}
    DamageType::DamageType(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv) {}

    const DamageType DamageType::INVALID = DamageType();

    bool DamageType::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const DamageType* converted = dynamic_cast<const DamageType*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool DamageType::Equals(const DamageType& other) const {
        return _abrv.Equals(other._abrv);
    }
}
