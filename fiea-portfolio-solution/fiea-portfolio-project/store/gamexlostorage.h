#pragma once
#include "gamelovstorage.h"
#include "../models/lovpairs.h"
#include "../models/battler.h"
#include "../models/battlerstat.h"
#include "../models/damageinclination.h"
#include "../models/equipment.h"
#include "../models/skill.h"

namespace AWE {
    /// <summary>
    /// XLO = XML Loaded Objects. Stores all the objects loaded using tinyxml2.
    /// </summary>
    class GameXLOStorage {
    private:
        EquipmentTypeCountMap _defaultEquipmentSlotSchema;
        DamageInclinationStatListMap _inclinationAttackingStats;
        DamageInclinationStatListMap _inclinationDefendingStats;
        SkillMap _skills;
        EquipmentMap _equipment;
        BattlerMap _battlers;

        bool _isInitialized;

    public:
        /// <summary>
        /// Default name for the XML file which will be parsed for data.
        /// </summary>
        static const char* DEFAULT_XMLFILENAME;

        /// <returns>const reference to the loaded default equipment slot schema.</returns>
        const EquipmentTypeCountMap& defaultEquipmentSlotSchema() const;
        /// <returns>const reference to the loaded skills.</returns>
        const SkillMap& skills() const;
        /// <returns>const reference to the loaded equipment.</returns>
        const EquipmentMap& equipment() const;
        /// <returns>const reference to the loaded battlers.</returns>
        const BattlerMap& battlers() const;
        /// <returns>Is this object initialized? If false, the Initialize method may need to be invoked.</returns>
        bool isInitialized() const;

        /// <summary>
        /// Initializes the load. Running this function after this object is initialized will simply re-run the load.
        /// </summary>
        /// <param name="lov">LOVs to be used for ABRV lookup.</param>
        /// <param name="nullDamageInclination">The damage inclination to be used when no damage inclination is given for base damage.</param>
        /// <param name="xmlfilename">Filename of the XML file to be used for the load.</param>
        /// <returns>Whether the load was successful.</returns>
        bool Initialize(const GameLOVStorage& lov, const DamageInclination_shptr& nullDamageInclination, const char* xmlfilename = DEFAULT_XMLFILENAME);

        /// <returns>Returns a copy of all attacking stats for the given damage inclination.</returns>
        BattlerStatList CopyAttackingStats(DamageInclinationKey) const;
        /// <returns>Returns a copy of all defending stats for the given damage inclination.</returns>
        BattlerStatList CopyDefendingStats(DamageInclinationKey) const;

        /// <returns>Mutable copy of the battler with the given key, or an empty pointer if no such battler exists.</returns>
        Battler_shptr GetBattler(const BattlerKey&);
        /// <summary>
        /// Adjusts the stat of the given battler by the given amount.
        /// </summary>
        /// <returns>New value of the battler's stat, or 0 if no such battler or stat exist.</returns>
        BattlerStatValue AdjustBattlerStat(const BattlerKey&, const BattlerStatKey&, int);
        /// <summary>
        /// Adjusts the elemental affinity of the given battler by the given amount.
        /// </summary>
        /// <returns>New value of the battler's elemental affinity, or 0 if no such battler or affinity exist.</returns>
        ElementalAffinityValue AdjustBattlerAffinity(const BattlerKey&, const ElementalAffinityKey&, int);
    };
}
