#include "damagesource.h"

namespace AWE {
    DamageSourceMap CreateFullDamageSources(const DamageInclinationMap& inclinations, const DamageTypeMap& dmgtypes) {
        DamageSourceMap sources;

        for (DamageInclinationMap::value_type inclination : inclinations) {
            for (DamageTypeMap::value_type dmgtype : dmgtypes) {
                sources.insert(std::make_pair(std::make_pair(dmgtype.first, inclination.first), DAMAGESOURCE_MAXVALUE));
            }
        }

        return sources;
    }

    float DetermineDamageSourcePercentage(DamageSourceValue val) {
        if (val < 1) {
            return 0.0f;
        }
        if (val >= DAMAGESOURCE_MAXVALUE) {
            return 1.0f;
        }
        return ((float)val / DAMAGESOURCE_MAXVALUE_F);
    }
}
