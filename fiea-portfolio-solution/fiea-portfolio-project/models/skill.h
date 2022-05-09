#pragma once
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "battlerstat.h"
#include "damageinclination.h"
#include "damagetype.h"
#include "skillelement.h"
#include "skillelementgroup.h"

namespace AWE {
    class SkillBaseDamage {
    private:
        int _value;
        DamageInclination_shptr _inclination;

    public:
        SkillBaseDamage();
        SkillBaseDamage(int, DamageInclination_shptr);

        static const SkillBaseDamage INVALID;

        int value() const;
        DamageInclination_shptr inclination() const;
    };

    class SkillElementBinding {
    private:
        bool _isPenetrating;
        float _scaling;
        DamageInclination_shptr _inclination;
        DamageType_shptr _damageType;
        SkillElement_shptr _element;
        SkillElementGroup_shptr _group;

    public:
        SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElement_shptr element);
        SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElementGroup_shptr group);

        bool isPenetrating() const;
        float scaling() const;
        DamageInclination_shptr inclination() const;
        DamageType_shptr damageType() const;
        SkillElement_shptr element() const;
        SkillElementGroup_shptr group() const;

        bool IsGroupBinding() const;
    };

    class SkillStatScaling {
    private:
        float _value;
        DamageInclination_shptr _inclination;
        BattlerStat_shptr _battlerStat;

    public:
        SkillStatScaling(float value, DamageInclination_shptr inclination, BattlerStat_shptr battlerStat);

        float value() const;
        DamageInclination_shptr inclination() const;
        BattlerStat_shptr battlerStat() const;
    };

    class SkillDamage {
    private:
        SkillBaseDamage _baseDamage;
        DamageInclinationKey _inclinationKey;
        std::vector<SkillStatScaling> _statScalings;
        std::vector<SkillElementBinding> _elementBindings;
        std::set<SkillElementGroupKey> _elementGroups;

    public:
        SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey);
        SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey, std::vector<SkillStatScaling>& statScalings, std::vector<SkillElementBinding>& elementBindings);

        const SkillBaseDamage& baseDamage() const;
        DamageInclinationKey inclinationKey() const;
        const std::vector<SkillStatScaling>& statScalings() const;
        const std::vector<SkillElementBinding>& elementBindings() const;
        const std::set<SkillElementGroupKey>& elementGroups() const;
    };

    class Skill {
    private:
        std::string _name;
        std::vector<SkillDamage> _damages;
        std::set<SkillElementGroupKey> _elementGroups;
        unsigned int _textureIndex;
        std::string _soundFilename;

    public:
        Skill(std::string name, unsigned int textureIndex = 0U, std::string soundFilename = "");
        Skill(std::string name, std::vector<SkillDamage>& damages, unsigned int textureIndex = 0U, std::string soundFilename = "");

        const std::string& name() const;
        const std::vector<SkillDamage>& damages() const;
        const std::set<SkillElementGroupKey>& elementGroups() const;
        unsigned int textureIndex() const;
        const std::string& soundFilename() const;

        unsigned int textureIndex(unsigned int);
        std::string soundFilename(std::string);
    };

    typedef std::string SkillKey;
    typedef std::shared_ptr<Skill> Skill_shptr;
    typedef std::map<SkillKey, Skill_shptr> SkillMap;
}
