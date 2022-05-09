#pragma once
#include <unordered_map>
#include <utility>
#include "damageinclination.h"
#include "damagetype.h"
#include "equipmenttype.h"
#include "skillelement.h"
#include "skillelementgroup.h"

namespace AWE {
    typedef std::pair<DamageTypeKey, DamageInclinationKey> DamageTypeInclination;

    struct DamageTypeInclination_comp {
        bool operator()(const DamageTypeInclination& left, const DamageTypeInclination& right) const {
            if (left.first == right.first) {
                return left.second < right.second;
            }
            return left.first < right.first;
        }
    };

    typedef std::unordered_map<SkillElementGroupKey, SkillElement_shptr> SkillElementGroupConversionMap;

    typedef std::unordered_map<EquipmentTypeKey, unsigned int> EquipmentTypeCountMap;
}
