#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    class BattlerStat : public AbbreviatedKey {
    public:
        BattlerStat();
        BattlerStat(std::string name, ABRV abrv);

        static const BattlerStat INVALID;
        static const ABRV MXHP;

        bool Equals(const AbbreviatedKey& other) const override;
        bool Equals(const BattlerStat& other) const;
    };

    typedef ABRV_long BattlerStatKey;
    typedef std::shared_ptr<BattlerStat> BattlerStat_shptr;
    typedef std::unordered_map<BattlerStatKey, BattlerStat_shptr> BattlerStatMap;
    typedef unsigned int BattlerStatValue;
    typedef std::unordered_map<BattlerStatKey, BattlerStatValue> BattlerStatValues;
    typedef std::vector<BattlerStat_shptr> BattlerStatList;
}
