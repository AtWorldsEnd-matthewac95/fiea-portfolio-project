#pragma once
#include <unordered_map>
#include <utility>
#include "damageinclination.h"
#include "damagetype.h"
#include "equipmenttype.h"
#include "skillelement.h"
#include "skillelementgroup.h"

namespace AWE {
    /// <summary>
    /// A pair of a damage type key (first) and a damage inclination key (second).
    /// The type key comes first because for comparison purposes, that's the key which is most likely to be different.
    /// </summary>
    typedef std::pair<DamageTypeKey, DamageInclinationKey> DamageTypeInclination;

    struct DamageTypeInclination_comp {
        bool operator()(const DamageTypeInclination& left, const DamageTypeInclination& right) const {
            if (left.first == right.first) {
                return left.second < right.second;
            }
            return left.first < right.first;
        }
    };

    /// <summary>
    /// Maps a skill element group key to a specific element. This is not meant to be a universal map! Currently is used by equiments on an instance basis.
    /// </summary>
    typedef std::unordered_map<SkillElementGroupKey, SkillElement_shptr> SkillElementGroupConversionMap;

    /// <summary>
    /// Used to assign a count to an equipment type. Equipment slots use this as a sort of schema for construction, but it has other uses.
    /// </summary>
    typedef std::unordered_map<EquipmentTypeKey, unsigned int> EquipmentTypeCountMap;
}
