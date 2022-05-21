#pragma once
#include "../models/battlerstat.h"
#include "../models/damageinclination.h"
#include "../models/damagetype.h"
#include "../models/equipmenttype.h"
#include "../models/skillelement.h"
#include "../models/skillelementgroup.h"

namespace AWE {
    /// <summary>
    /// LOV = List Of Values. This object stores all "list entry" objects loaded from the `.txt` files in the `res` folder. These objects all happen to be subclasses of AbbreviatedKey.
    /// </summary>
    class GameLOVStorage {
    private:
        static const std::string DEFAULT_LOAD_RES_LOC;

        BattlerStatMap _battlerStats;
        DamageInclinationMap _damageInclinations;
        DamageTypeMap _damageTypes;
        EquipmentTypeMap _equipmentTypes;
        SkillElementMap _skillElements;
        SkillElementGroupMap _skillElementGroups;

        bool _isInitialized;

    public:
        /// <returns>const reference to the loaded battler stats.</returns>
        const BattlerStatMap& battlerStats() const;
        /// <returns>const reference to the loaded damage inclinations.</returns>
        const DamageInclinationMap& damageInclinations() const;
        /// <returns>const reference to the loaded damage types.</returns>
        const DamageTypeMap& damageTypes() const;
        /// <returns>const reference to the loaded equipment types.</returns>
        const EquipmentTypeMap& equipmentTypes() const;
        /// <returns>const reference to the loaded skill elements.</returns>
        const SkillElementMap& skillElements() const;
        /// <returns>const reference to the loaded skill element groups.</returns>
        const SkillElementGroupMap& skillElementGroups() const;
        /// <returns>Is this object initialized? If false, the Initialize function may need to be invoked.</returns>
        bool isInitialized() const;

        /// <summary>
        /// Initializes the load. Running this function after this object is initialized will simply re-run the load.
        /// </summary>
        /// <param name="resloc">Location of the text files to load data from.</param>
        /// <returns>Whether the load was successful.</returns>
        bool Initialize(const std::string& resloc = DEFAULT_LOAD_RES_LOC);

        /// <returns>Copy of the shared pointer to the battler stat with the given key. Returns an empty pointer if the key has no match.</returns>
        BattlerStat_shptr GetBattlerStat(BattlerStatKey) const;
        /// <returns>Copy of the shared pointer to the damage inclination with the given key. Returns an empty pointer if the key has no match.</returns>
        DamageInclination_shptr GetDamageInclination(DamageInclinationKey) const;
        /// <returns>Copy of the shared pointer to the damage type with the given key. Returns an empty pointer if the key has no match.</returns>
        DamageType_shptr GetDamageType(DamageTypeKey) const;
        /// <returns>Copy of the shared pointer to the equipment type with the given key. Returns an empty pointer if the key has no match.</returns>
        EquipmentType_shptr GetEquipmentType(EquipmentTypeKey) const;
        /// <returns>Copy of the shared pointer to the skill element with the given key. Returns an empty pointer if the key has no match.</returns>
        SkillElement_shptr GetSkillElement(SkillElementKey) const;
        /// <returns>Copy of the shared pointer to the skill element group with the given key. Returns an empty pointer if the key has no match.</returns>
        SkillElementGroup_shptr GetSkillElementGroup(SkillElementGroupKey) const;
    };
}
