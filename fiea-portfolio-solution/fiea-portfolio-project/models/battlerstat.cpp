#include "battlerstat.h"

namespace AWE {
    BattlerStat::BattlerStat() : AbbreviatedKey() {}
    BattlerStat::BattlerStat(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv) {}

    const BattlerStat BattlerStat::INVALID = BattlerStat();
    const ABRV BattlerStat::MXHP = ABRV('M', 'X', 'H', 'P');

    bool BattlerStat::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const BattlerStat* converted = dynamic_cast<const BattlerStat*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool BattlerStat::Equals(const BattlerStat& other) const {
        return _abrv.Equals(other._abrv);
    }
}
