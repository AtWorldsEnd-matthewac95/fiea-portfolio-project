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
        /// <summary>
        /// Default constructor. Initializes the object with 0 value and an invalid inclination.
        /// </summary>
        SkillBaseDamage();
        SkillBaseDamage(int, DamageInclination_shptr);

        /// <summary>
        /// Any SkillBaseDamage which is equivalent to this object is invalid.
        /// </summary>
        static const SkillBaseDamage INVALID;

        /// <returns>Value of the base damage.</returns>
        int value() const;
        /// <returns>shared_ptr to this base damage's inclination.</returns>
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
        /// <summary>
        /// Initializes the object with a skill element, making this NOT a group binding.
        /// </summary>
        SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElement_shptr element);
        /// <summary>
        /// Initializes the object with a skill element group, making this a group binding.
        /// </summary>
        SkillElementBinding(bool isPenetrating, float scaling, DamageInclination_shptr inclination, DamageType_shptr damageType, SkillElementGroup_shptr group);

        /// <returns>Is this portion of the skill penetrating damage?</returns>
        bool isPenetrating() const;
        /// <returns>Percentage from 0.f to 1.f of the stat-scaled damage which should be given to this binding.</returns>
        float scaling() const;
        /// <returns>shared_ptr to this object's inclination.</returns>
        DamageInclination_shptr inclination() const;
        /// <returns>shared_ptr to this object's damage type.</returns>
        DamageType_shptr damageType() const;
        /// <returns>shared_ptr to this object's skill element, if it exists. Otherwise, returns an empty shared_ptr.</returns>
        SkillElement_shptr element() const;
        /// <returns>shared_ptr to this object's skill element group, if it exists. Otherwise, returns an empty shared_ptr.</returns>
        SkillElementGroup_shptr group() const;

        /// <returns>Does this object use a skill element group for its binding?</returns>
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

        /// <returns>Value from 0.f to 1.f of this scaling. Example: if the stat is Strength and the value is 0.5f, the skill will use 50% of the Strength stat in its calculation.</returns>
        float value() const;
        /// <returns>shared_ptr to the inclination of this object.</returns>
        DamageInclination_shptr inclination() const;
        /// <returns>shared_ptr to the battler stat of this object.</returns>
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
        /// <summary>
        /// Constructor. Initializes the object with only base damage.
        /// </summary>
        SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey);
        SkillDamage(SkillBaseDamage baseDamage, DamageInclinationKey inclinationKey, std::vector<SkillStatScaling>& statScalings, std::vector<SkillElementBinding>& elementBindings);

        /// <returns>const reference to this object's base damage.</returns>
        const SkillBaseDamage& baseDamage() const;
        /// <returns>Key of this object's inclination.</returns>
        DamageInclinationKey inclinationKey() const;
        /// <returns>const reference to this object's stat scalings.</returns>
        const std::vector<SkillStatScaling>& statScalings() const;
        /// <returns>const reference to this object's element bindings.</returns>
        const std::vector<SkillElementBinding>& elementBindings() const;
        /// <returns>const reference to the list of all skill element groups used for this object's damage calculation.</returns>
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
        /// <summary>
        /// Constructor. Initializes the object without any damage, which is valid.
        /// </summary>
        Skill(std::string name, unsigned int textureIndex = 0U, std::string soundFlename = "");
        Skill(std::string name, std::vector<SkillDamage>& damages, unsigned int textureIndex = 0U, std::string soundFilename = "");

        /// <returns>const reference to this skill's name.</returns>
        const std::string& name() const;
        /// <returns>const reference to this skill's damages.</returns>
        const std::vector<SkillDamage>& damages() const;
        /// <returns>const reference to the list of all skill element groups used for this skill's damage calculation.</returns>
        const std::set<SkillElementGroupKey>& elementGroups() const;
        /// <returns>The texture index corresponding to the correct image for this skill.</returns>
        unsigned int textureIndex() const;
        /// <returns>const reference to this skill's sound's file name.</returns>
        const std::string& soundFilename() const;

        /// <param name="">New value for the texture index.</param>
        /// <returns>Old value for the texture index.</returns>
        unsigned int textureIndex(unsigned int);
        /// <param name="">New value for this skill's sound's filename.</param>
        /// <returns>Old value for this skill's sound's filename.</returns>
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
