#include "skill.h"

namespace AWE {

    /* SkillBaseDamage */

    SkillBaseDamage::SkillBaseDamage() : _value(0) {}
    SkillBaseDamage::SkillBaseDamage(int value, DamageInclination_shptr inclination) : _value(std::move(value)), _inclination(inclination) {}

    const SkillBaseDamage SkillBaseDamage::INVALID = SkillBaseDamage();

    int SkillBaseDamage::value() const { return _value; }
    DamageInclination_shptr SkillBaseDamage::inclination() const { return _inclination; }


    /* SkillElementBinding */

    SkillElementBinding::SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElement_shptr element)
        : _isPenetrating(std::move(isPenetrating)), _scaling(std::move(scaling)), _inclination(std::move(inclination)), _damageType(std::move(damageType)), _element(std::move(element)) {}
    SkillElementBinding::SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElementGroup_shptr group)
        : _isPenetrating(std::move(isPenetrating)), _scaling(std::move(scaling)), _inclination(std::move(inclination)), _damageType(std::move(damageType)), _group(std::move(group)) {}

    bool SkillElementBinding::isPenetrating() const { return _isPenetrating; }
    float SkillElementBinding::scaling() const { return _scaling; }
    DamageInclination_shptr SkillElementBinding::inclination() const { return _inclination; }
    DamageType_shptr SkillElementBinding::damageType() const { return _damageType; }
    SkillElement_shptr SkillElementBinding::element() const { return _element; }
    SkillElementGroup_shptr SkillElementBinding::group() const { return _group; }

    bool SkillElementBinding::IsGroupBinding() const { return (bool)_group; }


    /* SkillStatScaling */

    SkillStatScaling::SkillStatScaling(float value, DamageInclination_shptr inclination, BattlerStat_shptr battlerStat) : _value(std::move(value)), _inclination(std::move(inclination)), _battlerStat(std::move(battlerStat)) {}

    float SkillStatScaling::value() const { return _value; }
    DamageInclination_shptr SkillStatScaling::inclination() const { return _inclination; }
    BattlerStat_shptr SkillStatScaling::battlerStat() const { return _battlerStat; }


    /* SkillDamage */

    SkillDamage::SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey)
        : _baseDamage(std::move(baseDamage))
        , _inclinationKey(std::move(inclinationKey))
        , _statScalings(std::vector<SkillStatScaling>())
        , _elementBindings(std::vector<SkillElementBinding>())
        , _elementGroups(std::set<SkillElementGroupKey>()) {}
    SkillDamage::SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey, std::vector<SkillStatScaling>& statScalings, std::vector<SkillElementBinding>& elementBindings)
            : _baseDamage(std::move(baseDamage))
            , _inclinationKey(std::move(inclinationKey))
            , _statScalings(std::move(statScalings))
            , _elementBindings(std::move(elementBindings)) {
        _elementGroups = std::set<SkillElementGroupKey>();

        for (const SkillElementBinding& binding : _elementBindings) {
            if (binding.IsGroupBinding()) {
                _elementGroups.insert(binding.group()->abrvlong());
            }
        }
    }

    const SkillBaseDamage& SkillDamage::baseDamage() const { return _baseDamage; }
    DamageInclinationKey SkillDamage::inclinationKey() const { return _inclinationKey; }
    const std::vector<SkillStatScaling>& SkillDamage::statScalings() const { return _statScalings; }
    const std::vector<SkillElementBinding>& SkillDamage::elementBindings() const { return _elementBindings; }
    const std::set<SkillElementGroupKey>& SkillDamage::elementGroups() const { return _elementGroups; }


    /* Skill */

    Skill::Skill(std::string name, unsigned int textureIndex, std::string soundFilename) : _name(std::move(name)), _damages(std::move(std::vector<SkillDamage>())), _textureIndex(textureIndex), _soundFilename(std::move(soundFilename)){}
    Skill::Skill(std::string name, std::vector<SkillDamage>& damages, unsigned int textureIndex, std::string soundFilename)
            : _name(std::move(name))
            , _damages(std::move(damages))
            , _textureIndex(textureIndex)
            , _soundFilename(std::move(soundFilename)) {
        _elementGroups = std::set<SkillElementGroupKey>();

        for (const SkillDamage& damage : _damages) {
            std::set<SkillElementGroupKey> damageElementGroups = damage.elementGroups();
            for (SkillElementGroupKey key : damageElementGroups) {
                _elementGroups.insert(key);
            }
        }
    }

    const std::string& Skill::name() const { return _name; }
    const std::vector<SkillDamage>& Skill::damages() const { return _damages; }
    const std::set<SkillElementGroupKey>& Skill::elementGroups() const { return _elementGroups; }
    unsigned int Skill::textureIndex() const { return _textureIndex; }
    const std::string& Skill::soundFilename() const { return _soundFilename; }

    unsigned int Skill::textureIndex(unsigned int newval) { unsigned int oldval = _textureIndex; _textureIndex = newval; return oldval; }
    std::string Skill::soundFilename(std::string newval) { std::string oldval = _soundFilename; _soundFilename = newval; return oldval; }
}
