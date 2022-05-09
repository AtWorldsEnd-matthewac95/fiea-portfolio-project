#pragma once
#include <map>
#include "damageinclination.h"
#include "damagetype.h"
#include "lovpairs.h"

namespace AWE {
    typedef unsigned short DamageSourceValue;
    typedef std::map<DamageTypeInclination, DamageSourceValue, DamageTypeInclination_comp> DamageSourceMap;

    static const DamageSourceValue DAMAGESOURCE_MAXVALUE = 4;
    static const float DAMAGESOURCE_MAXVALUE_F = 4.0f;

    DamageSourceMap CreateFullDamageSources(const DamageInclinationMap& inclinations, const DamageTypeMap& dmgtypes);
    float DetermineDamageSourcePercentage(DamageSourceValue);
}
