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
    /// <summary>
    /// Represents base damage of one of a skill's damage compontents.
    /// </summary>
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

    /// <summary>
    /// Represents how much of the stat-scaled damage should be bound to a certain skill element or skill element group, as well as whether that damage should be penetrating
    /// and what type of damage should be dealt.
    /// </summary>
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

    /// <summary>
    /// Represents how much a certain stat should affect the damage output of this skill. The inclination indicates what stats should be used to calculate damage reduction later.
    /// </summary>
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

    /// <summary>
    /// Combines base damage, element bindings, and stat scalings to create one damage component of the skill. Skills can have several of these.
    /// </summary>
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

    /// <summary>
    /// Represents a skill which battlers may use in combat against each other.
    /// </summary>
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

    /// <summary>
    /// Used to index skills into a map. Currently designed to be a string which should be equivalent to the skill's name.
    /// </summary>
    typedef std::string SkillKey;
    /// <summary>
    /// Represents a skill which battlers may use in combat against each other.
    /// </summary>
    typedef std::shared_ptr<Skill> Skill_shptr;
    typedef std::map<SkillKey, Skill_shptr> SkillMap;
}
