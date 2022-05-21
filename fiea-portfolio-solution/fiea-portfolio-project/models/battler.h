#pragma once
#include <map>
#include <memory>
#include <string>
#include "battlerstat.h"
#include "damageresistances.h"
#include "damagesource.h"
#include "elementalaffinities.h"
#include "equipmentslots.h"
//#include "../store/gamexlostorage.h"

namespace AWE {
    /// <summary>
    /// Represents "global" information about a battler. For the objects used in actual battles, see AWE::BattlerInstance.
    /// </summary>
    class Battler {
    private:
        std::string _name;
        bool _isCharacter;
        unsigned short _priority;
        unsigned int _textureIndex;
        unsigned int _textureType;
        BattlerStatValues _stats;
        DamageResistances _resistances;
        DamageSourceMap _innateDamageSources;
        ElementalAffinities _affinities;
        EquipmentSlots _currentEquipment;

    public:
        /// <summary>
        /// Constructor. EquipmentSlots object is moved directly into the battler.
        /// </summary>
        Battler(
            std::string,
            bool isCharacter,
            unsigned short priority,
            unsigned int textureIndex,
            unsigned int textureType,
            BattlerStatValues&,
            DamageResistances&,
            DamageSourceMap&,
            ElementalAffinities&,
            EquipmentSlots&
        );
        /// <summary>
        /// Constructor. EquipmentSlots are built internally using the EquipmentList object.
        /// </summary>
        Battler(
            std::string,
            bool isCharacter,
            unsigned short priority,
            unsigned int textureIndex,
            unsigned int textureType,
            BattlerStatValues&,
            DamageResistances&,
            DamageSourceMap&,
            ElementalAffinities&,
            const EquipmentList&
        );

        /// <returns>const reference to the name of this battler.</returns>
        const std::string& name() const;
        /// <returns>Indicates whether this battler is a playable character or not.</returns>
        bool isCharacter() const;
        /// <returns>Battler's battle priority. A lower value means the battler goes earlier in the turn. Does not necessarily correlate to order of skill selection.</returns>
        unsigned short priority() const;
        /// <returns>What index of the texture should this battler's sprite occupy?</returns>
        unsigned int textureIndex() const;
        /// <returns>What texture should be used to represent this battler? Value should be convertable to GameTextureType.</returns>
        unsigned int textureType() const;
        /// <returns>const reference to this battler's damage type resistances. Note this is not the same as the battler's defense stat.</returns>
        const DamageResistances& resistances() const;
        /// <returns>const reference to this battler's elemental affinities.</returns>
        const ElementalAffinities& affinities() const;
        /// <returns>const refernece to this battler's current equipment slots.</returns>
        const EquipmentSlots& currentEquipment() const;

        /// <returns>Value of the stat with the given key, or 0 if this battler has no such stat.</returns>
        BattlerStatValue GetStat(BattlerStatKey) const;
        /// <returns>Innate damage source value of this battler for the given damage type and inclination.</returns>
        DamageSourceValue GetInnateDamageSource(DamageTypeKey, DamageInclinationKey) const;
        /// <returns>Innate damage source value of this battler for the given damage type and inclination.</returns>
        DamageSourceValue GetInnateDamageSource(DamageTypeInclination) const;

        /// <summary>
        /// Adjusts the battler's stat by the value given.
        /// </summary>
        /// <returns>Returns the new value of that stat.</returns>
        BattlerStatValue AdjustStat(BattlerStatKey, int);
        /// <summary>
        /// Adjusts the battler's elemental affinity by the value given.
        /// </summary>
        /// <returns>Returns the new value of that affinity.</returns>
        ElementalAffinityValue AdjustAffinity(ElementalAffinityKey, int);

        friend class BattlerInstance;
    };

    /// <summary>
    /// Represents "instanced" information about a battler. A battler instance is created from an AWE::Battler at the start of combat, and is used to manage that battler's status during the battle.
    /// </summary>
    class BattlerInstance {
    private:
        Battler* _parent;
        BattlerStatValue _hp;
        BattlerStatValues _stats;
        DamageResistances _resistances;
        DamageSourceMap _damageSources;
        ElementalAffinities _affinities;
        SkillMap _skills;

    public:
        /// <summary>
        /// Constructor. Creates a new battler instance from the given battler.
        /// </summary>
        /// <param name="hp">If 0 or less, the battler instance will be created using the `MXHP` stat of the given battler. Otherwise, the battler instance will have the given HP value.</param>
        BattlerInstance(Battler&, BattlerStatValue hp = 0);

        /// <returns>const reference to the name of this battler.</returns>
        const std::string& name() const;
        /// <returns>Indicates whether this battler is a playable character or not.</returns>
        bool isCharacter() const;
        /// <returns>Current HP value of this battler instance.</returns>
        BattlerStatValue hp() const;
        /// <returns>Battler's battle priority. A lower value means the battler goes earlier in the turn. Does not necessarily correlate to order of skill selection.</returns>
        unsigned short priority() const;
        /// <returns>What index of the texture should this battler's sprite occupy?</returns>
        unsigned int textureIndex() const;
        /// <returns>What texture should be used to represent this battler? Value should be convertable to GameTextureType.</returns>
        unsigned int textureType() const;
        /// <returns>const reference to this battler's damage type resistances. Note this is not the same as the battler's defense stat.</returns>
        const DamageResistances& resistances() const;
        /// <returns>const reference to this battler's elemental affinities.</returns>
        const ElementalAffinities& affinities() const;
        /// <returns>const reference to the battler instance's skills. Skills are determined using the battler's equipment.</returns>
        const SkillMap& skills() const;

        /// <param name="">New HP value.</param>
        /// <returns>Old HP value.</returns>
        BattlerStatValue hp(BattlerStatValue);

        /// <returns>Value of the stat with the given key, or 0 if this battler has no such stat.</returns>
        BattlerStatValue GetStat(BattlerStatKey) const;
        /// <returns>Innate damage source value of this battler for the given damage type and inclination.</returns>
        DamageSourceValue GetDamageSource(DamageTypeKey, DamageInclinationKey) const;
        /// <returns>Innate damage source value of this battler for the given damage type and inclination.</returns>
        DamageSourceValue GetDamageSource(DamageTypeInclination) const;
    };

    /// <summary>
    /// Represents "global" information about a battler. For the objects used in actual battles, see AWE::BattlerInstance.
    /// </summary>
    typedef std::shared_ptr<Battler> Battler_shptr;
    /// <summary>
    /// Represents "instanced" information about a battler. A battler instance is created from an AWE::Battler at the start of combat, and is used to manage that battler's status during the battle.
    /// </summary>
    typedef std::shared_ptr<BattlerInstance> BattlerInstance_shptr;
    /// <summary>
    /// Used to index battlers into a map. Currently designed to be a string which should be equivalent to the battler's name.
    /// </summary>
    typedef std::string BattlerKey;
    typedef std::map<BattlerKey, Battler_shptr> BattlerMap;
}
