#include "damageinclination.h"

namespace AWE {
    DamageInclination::DamageInclination() : AbbreviatedKey() {}
    DamageInclination::DamageInclination(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv) {}

    const DamageInclination DamageInclination::INVALID = DamageInclination();
    const ABRV DamageInclination::AUTO_KEY = ABRV('A', 'U', 'T', 'O');

    bool DamageInclination::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const DamageInclination* converted = dynamic_cast<const DamageInclination*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool DamageInclination::Equals(const DamageInclination& other) const {
        return _abrv.Equals(other._abrv);
    }
}
