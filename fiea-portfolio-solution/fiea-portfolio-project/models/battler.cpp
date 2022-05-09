#include "battler.h"

namespace AWE {
    Battler::Battler(
        std::string name,
        bool isCharacter,
        unsigned short priority,
        unsigned int textureIndex,
        unsigned int textureType,
        BattlerStatValues& stats,
        DamageResistances& resists,
        DamageSourceMap& innateDamageSources,
        ElementalAffinities& affinities,
        EquipmentSlots& startingEquipment
    )   : _name(std::move(name))
        , _isCharacter(isCharacter)
        , _priority(priority)
        , _textureIndex(textureIndex)
        , _textureType(textureType)
        , _stats(std::move(stats))
        , _resistances(std::move(resists))
        , _innateDamageSources(std::move(innateDamageSources))
        , _affinities(std::move(affinities))
        , _currentEquipment(std::move(startingEquipment)) {}
    Battler::Battler(
        std::string name,
        bool isCharacter,
        unsigned short priority,
        unsigned int textureIndex,
        unsigned int textureType,
        BattlerStatValues& stats,
        DamageResistances& resists,
        DamageSourceMap& innateDamageSources,
        ElementalAffinities& affinities,
        const EquipmentList& startingEquipment
    )       : _name(std::move(name))
            , _isCharacter(isCharacter)
            , _priority(priority)
            , _textureIndex(textureIndex)
            , _textureType(textureType)
            , _stats(std::move(stats))
            , _resistances(std::move(resists))
            , _innateDamageSources(std::move(innateDamageSources))
            , _affinities(std::move(affinities)) 
            , _currentEquipment(std::move(EquipmentSlots())) {
        for (const Equipment_shptr& equipment : startingEquipment) {
            _currentEquipment.ForceEquip(equipment);
        }
    }

    const std::string& Battler::name() const { return _name; }
    bool Battler::isCharacter() const { return _isCharacter; }
    unsigned short Battler::priority() const { return _priority; }
    unsigned int Battler::textureIndex() const { return _textureIndex; }
    unsigned int Battler::textureType() const { return _textureType; }
    const DamageResistances& Battler::resistances() const { return _resistances; }
    const ElementalAffinities& Battler::affinities() const { return _affinities; }
    const EquipmentSlots& Battler::currentEquipment() const { return _currentEquipment; }

    BattlerStatValue Battler::GetStat(BattlerStatKey key) const { return _stats.contains(key) ? _stats.at(key) : 0; }
    DamageSourceValue Battler::GetInnateDamageSource(DamageTypeKey dmgtype, DamageInclinationKey dmgincl) const { return GetInnateDamageSource(DamageTypeInclination(dmgtype, dmgincl)); }
    DamageSourceValue Battler::GetInnateDamageSource(DamageTypeInclination key) const { return _innateDamageSources.contains(key) ? _innateDamageSources.at(key) : 0; }

    BattlerStatValue Battler::AdjustStat(BattlerStatKey key, int delta) {
        auto found = _stats.find(key);
        if (found == _stats.end()) {
            found = _stats.insert(_stats.begin(), std::make_pair(key, 0));
        }

        int newval = found->second + delta;
        found->second = newval < 0 ? 0 : newval;
        return found->second;
    }

    ElementalAffinityValue Battler::AdjustAffinity(ElementalAffinityKey key, int delta) {
        return _affinities.AddValue(key, delta);
    }

    BattlerInstance::BattlerInstance(Battler& parent, BattlerStatValue hp) : _parent(&parent), _affinities(ElementalAffinities(parent._affinities)) {
        _resistances = DamageResistances(parent._resistances);
        _stats = BattlerStatValues(parent._stats);
        _damageSources = DamageSourceMap(parent._innateDamageSources);

        _hp = std::move(hp);

        if (hp == 0) {
            auto parenthp = _stats.find(BattlerStat::MXHP.AsLong());
            _hp = parenthp == _stats.end() ? 0 : parenthp->second;
        }

        _skills = SkillMap();

        for (const EquipmentSlots::const_iterator::value_type& equipment : parent._currentEquipment) {
            const BattlerStatValues& bonusStats = equipment.second->bonusStats();

            for (const BattlerStatValues::value_type& bonusStat : bonusStats) {
                auto found = _stats.find(bonusStat.first);
                if (found == _stats.end()) {
                    _stats.insert(std::make_pair(bonusStat.first, bonusStat.second));
                } else {
                    found->second += bonusStat.second;
                }
            }

            const DamageResistances& bonusResists = equipment.second->bonusResistances();

            for (const DamageResistances::const_iterator::value_type& bonusResist : bonusResists) {
                _resistances.AddValue(bonusResist.first, bonusResist.second);
            }

            const std::vector<Skill_shptr>& equipmentSkills = equipment.second->skills();

            for (const Skill_shptr& equipmentSkill : equipmentSkills) {
                _skills.insert(std::make_pair(equipmentSkill->name(), equipmentSkill));
            }

            const DamageSourceMap& damageSources = equipment.second->damageSources();

            for (const DamageSourceMap::value_type& damageSource : damageSources) {
                auto found = _damageSources.find(damageSource.first);
                if (found == _damageSources.end()) {
                    found = _damageSources.insert(_damageSources.begin(), std::make_pair(damageSource.first, 0));
                }
                found->second += damageSource.second;
            }
        }
    }

    const std::string& BattlerInstance::name() const { return _parent->_name; }
    bool BattlerInstance::isCharacter() const { return _parent->_isCharacter; }
    unsigned short BattlerInstance::priority() const { return _parent->_priority; }
    unsigned int BattlerInstance::textureIndex() const { return _parent->_textureIndex; }
    unsigned int BattlerInstance::textureType() const { return _parent->_textureType; }
    const DamageResistances& BattlerInstance::resistances() const { return _resistances; }
    const ElementalAffinities& BattlerInstance::affinities() const { return _affinities; }
    const SkillMap& BattlerInstance::skills() const { return _skills; }

    BattlerStatValue BattlerInstance::hp() const { return _hp; }
    BattlerStatValue BattlerInstance::hp(BattlerStatValue newval) { BattlerStatValue oldval = std::move(_hp); _hp = std::move(newval); return oldval; }

    BattlerStatValue BattlerInstance::GetStat(BattlerStatKey key) const { return _stats.contains(key) ? _stats.at(key) : 0; }
    DamageSourceValue BattlerInstance::GetDamageSource(DamageTypeKey dmgtype, DamageInclinationKey dmgincl) const { return GetDamageSource(DamageTypeInclination(dmgtype, dmgincl)); }
    DamageSourceValue BattlerInstance::GetDamageSource(DamageTypeInclination key) const { return _damageSources.contains(key) ? _damageSources.at(key) : 0; }
}
