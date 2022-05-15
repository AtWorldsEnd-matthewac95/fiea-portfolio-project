#pragma once
#include <map>
#include "damageinclination.h"
#include "damagetype.h"
#include "lovpairs.h"

namespace AWE {
    /// <summary>
    /// Damage sources are necessary for any battler to deal any kind of damage. If a battler meets or exceeds the maximum amount of sources for a certain type of
    /// damage, that battler will deal the normal amount of damage of that type - otherwise, they deal a reduced amount of that type of damage.
    /// This system allows for some interesting design space. For instance, perhaps a player may discover a skill they've been using the whole game has a hidden
    /// additional damage component they've never seen before simply because that battler has never had sources of that damage.
    /// For this version of the project, this system is not really used for simplicity's sake - however, it is implemented, and can be used in later projects.
    /// </summary>
    typedef unsigned short DamageSourceValue;
    /// <summary>
    /// Damage sources are necessary for any battler to deal any kind of damage. If a battler meets or exceeds the maximum amount of sources for a certain type of
    /// damage, that battler will deal the normal amount of damage of that type - otherwise, they deal a reduced amount of that type of damage.
    /// This system allows for some interesting design space. For instance, perhaps a player may discover a skill they've been using the whole game has a hidden
    /// additional damage component they've never seen before simply because that battler has never had sources of that damage.
    /// For this version of the project, this system is not really used for simplicity's sake - however, it is implemented, and can be used in later projects.
    /// </summary>
    typedef std::map<DamageTypeInclination, DamageSourceValue, DamageTypeInclination_comp> DamageSourceMap;

    /// <summary>
    /// If a battler's amount of a damage source meets or exceeds this value, they will deal the normal amount of damage for that type they have sources of.
    /// Having more than this value has no effect.
    /// </summary>
    static const DamageSourceValue DAMAGESOURCE_MAXVALUE = 4;
    /// <summary>
    /// If a battler's amount of a damage source meets or exceeds this value, they will deal the normal amount of damage for that type they have sources of.
    /// Having more than this value has no effect.
    /// </summary>
    static const float DAMAGESOURCE_MAXVALUE_F = 4.0f;

    /// <returns>A DamageSourceMap which has full damage sources for all inclinations and types given.</returns>
    DamageSourceMap CreateFullDamageSources(const DamageInclinationMap& inclinations, const DamageTypeMap& dmgtypes);
    /// <summary>
    /// Converts a DamageSourceValue into a percentage of damage.
    /// </summary>
    /// <returns>A number between 0 and 1.</returns>
    float DetermineDamageSourcePercentage(DamageSourceValue);
}
