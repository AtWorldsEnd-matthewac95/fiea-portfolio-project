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

        const std::string& name() const;
        bool isCharacter() const;
        unsigned short priority() const;
        unsigned int textureIndex() const;
        unsigned int textureType() const;
        const DamageResistances& resistances() const;
        const ElementalAffinities& affinities() const;
        const EquipmentSlots& currentEquipment() const;

        BattlerStatValue GetStat(BattlerStatKey) const;
        DamageSourceValue GetInnateDamageSource(DamageTypeKey, DamageInclinationKey) const;
        DamageSourceValue GetInnateDamageSource(DamageTypeInclination) const;

        BattlerStatValue AdjustStat(BattlerStatKey, int);
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
        BattlerInstance(Battler&, BattlerStatValue hp = 0);

        const std::string& name() const;
        bool isCharacter() const;
        BattlerStatValue hp() const;
        unsigned short priority() const;
        unsigned int textureIndex() const;
        unsigned int textureType() const;
        const DamageResistances& resistances() const;
        const ElementalAffinities& affinities() const;
        const SkillMap& skills() const;

        BattlerStatValue hp(BattlerStatValue);

        BattlerStatValue GetStat(BattlerStatKey) const;
        DamageSourceValue GetDamageSource(DamageTypeKey, DamageInclinationKey) const;
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
