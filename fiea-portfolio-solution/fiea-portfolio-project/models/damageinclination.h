#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"
#include "battlerstat.h"

namespace AWE {
    class DamageInclination : public AbbreviatedKey {
    public:
        DamageInclination();
        DamageInclination(std::string name, ABRV abrv);

        static const DamageInclination INVALID;
        static const ABRV AUTO_KEY;

        bool Equals(const AbbreviatedKey& other) const override;
        bool Equals(const DamageInclination& other) const;
    };

    typedef ABRV_long DamageInclinationKey;
    typedef std::shared_ptr<DamageInclination> DamageInclination_shptr;
    typedef std::unordered_map<DamageInclinationKey, DamageInclination_shptr> DamageInclinationMap;
    typedef std::unordered_map<DamageInclinationKey, BattlerStatList> DamageInclinationStatListMap;
}
