#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"
#include "battlerstat.h"

namespace AWE {
    /// <summary>
    /// Represents one of the possible inclinations damage can have. Can also be thought of as the damage "archetype" when compared to the damage type, though this relationship has no OOP bearing.
    /// Examples: physical, magical, etc.
    /// </summary>
    class DamageInclination : public AbbreviatedKey {
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        DamageInclination();
        DamageInclination(std::string name, ABRV abrv);

        /// <summary>
        /// Any DamageInclination which is equivalent to this object is invalid.
        /// </summary>
        static const DamageInclination INVALID;
        /// <summary>
        /// ABRV which represents the "AUTO" damage inclination. This is not an actual valid damage inclination, but when this value is found in certain places where an inclination is
        /// expected, the algorithm would be designed to know to determine the correct inclination from the surrounding context.
        /// </summary>
        static const ABRV AUTO_KEY;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other DamageInclination object.</param>
        /// <returns>Equivalence to the other DamageInclination.</returns>
        bool Equals(const DamageInclination& other) const;
    };

    /// <summary>
    /// Used to index DamageInclination objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long DamageInclinationKey;
    /// <summary>
    /// Represents one of the possible inclinations damage can have. Can also be thought of as the damage "archetype" when compared to the damage type, though this relationship has no OOP bearing.
    /// Examples: physical, magical, etc.
    /// </summary>
    typedef std::shared_ptr<DamageInclination> DamageInclination_shptr;
    typedef std::unordered_map<DamageInclinationKey, DamageInclination_shptr> DamageInclinationMap;
    /// <summary>
    /// This map is used to associate BattlerStat objects to inclinations. This is somewhat unusual, but necessary for the damage formula.
    /// </summary>
    typedef std::unordered_map<DamageInclinationKey, BattlerStatList> DamageInclinationStatListMap;
}
