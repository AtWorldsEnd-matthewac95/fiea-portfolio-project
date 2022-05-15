#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    /// <summary>
    /// Represents one of the possible stats a battler can have. Note that this object does not contain the values of the stat; it only describes what the stat is.
    /// </summary>
    class BattlerStat : public AbbreviatedKey {
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        BattlerStat();
        BattlerStat(std::string name, ABRV abrv);

        /// <summary>
        /// Any BattlerStat which is equivalent to this object is invalid.
        /// </summary>
        static const BattlerStat INVALID;
        /// <summary>
        /// ABRV of the BattlerStat which represents "Max HP". This particular stat is uniquely relevant for several things, hence this constant.
        /// </summary>
        static const ABRV MXHP;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other BattlerStat object.</param>
        /// <returns>Equivalence to the other BattlerStat.</returns>
        bool Equals(const BattlerStat& other) const;
    };

    /// <summary>
    /// Used to index BattlerStat objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long BattlerStatKey;
    /// <summary>
    /// Represents one of the possible stats a battler can have. Note that this object does not contain the values of the stat; it only describes what the stat is.
    /// </summary>
    typedef std::shared_ptr<BattlerStat> BattlerStat_shptr;
    typedef std::unordered_map<BattlerStatKey, BattlerStat_shptr> BattlerStatMap;
    /// <summary>
    /// Value of a BattlerStat. Usually is paired to a BattlerStat somehow to give meaning. Note this is unsigned: there are no negative stats.
    /// </summary>
    typedef unsigned int BattlerStatValue;
    typedef std::unordered_map<BattlerStatKey, BattlerStatValue> BattlerStatValues;
    typedef std::vector<BattlerStat_shptr> BattlerStatList;
}
