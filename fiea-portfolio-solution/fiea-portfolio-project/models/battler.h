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

    typedef std::shared_ptr<Battler> Battler_shptr;
    typedef std::shared_ptr<BattlerInstance> BattlerInstance_shptr;
    typedef std::string BattlerKey;
    typedef std::map<BattlerKey, Battler_shptr> BattlerMap;
}
