#include "gamexlostorage.h"
#include <iostream>
#include <memory>
#include "../misc/xmlload.h"

namespace AWE {
    const char* GameXLOStorage::DEFAULT_XMLFILENAME = "res/data.xml";

    const EquipmentTypeCountMap& GameXLOStorage::defaultEquipmentSlotSchema() const { return _defaultEquipmentSlotSchema; }
    const SkillMap& GameXLOStorage::skills() const { return _skills; }
    const EquipmentMap& GameXLOStorage::equipment() const { return _equipment; }
    const BattlerMap& GameXLOStorage::battlers() const { return _battlers; }
    bool GameXLOStorage::isInitialized() const { return _isInitialized; }

    BattlerStatList GameXLOStorage::CopyAttackingStats(DamageInclinationKey incl) const {
        BattlerStatList output;

        auto found = _inclinationAttackingStats.find(incl);
        if (found != _inclinationAttackingStats.end()) {
            output = BattlerStatList(found->second);
        }

        return output;
    }

    BattlerStatList GameXLOStorage::CopyDefendingStats(DamageInclinationKey incl) const {
        BattlerStatList output;

        auto found = _inclinationDefendingStats.find(incl);
        if (found != _inclinationDefendingStats.end()) {
            output = BattlerStatList(found->second);
        }

        return output;
    }

    const Skill_shptr& GameXLOStorage::GetSkill(const SkillKey& key) const {
        Skill_shptr skill;

        if (_isInitialized) {
            auto found = _skills.find(key);
            if (found != _skills.end()) {
                skill = found->second;
            }
        }

        return skill;
    }

    const Equipment_shptr& GameXLOStorage::GetEquipment(const EquipmentKey& key) const {
        Equipment_shptr equip;

        if (_isInitialized) {
            auto found = _equipment.find(key);
            if (found != _equipment.end()) {
                equip = found->second;
            }
        }

        return equip;
    }

    const Battler_shptr& GameXLOStorage::GetBattler(const BattlerKey& key) const {
        Battler_shptr battler;

        if (_isInitialized) {
            auto found = _battlers.find(key);
            if (found != _battlers.end()) {
                battler = found->second;
            }
        }

        return battler;
    }

    Battler_shptr GameXLOStorage::GetBattler(const BattlerKey& key) {
        Battler_shptr battler;

        if (_isInitialized) {
            auto found = _battlers.find(key);
            if (found != _battlers.end()) {
                battler = found->second;
            }
        }

        return battler;
    }

    BattlerStatValue GameXLOStorage::AdjustBattlerStat(const BattlerKey& bkey, const BattlerStatKey& skey, int delta) {
        auto found = _battlers.find(bkey);
        if (found == _battlers.end()) {
            return 0;
        }
        return found->second->AdjustStat(skey, delta);
    }

    ElementalAffinityValue GameXLOStorage::AdjustBattlerAffinity(const BattlerKey& bkey, const ElementalAffinityKey& ekey, int delta) {
        auto found = _battlers.find(bkey);
        if (found == _battlers.end()) {
            return 0;
        }
        return found->second->AdjustAffinity(ekey, delta);
    }

    bool GameXLOStorage::Initialize(const GameLOVStorage& lov, const DamageInclination_shptr& nullDamageInclination, const char* xmlfilename) {
        _isInitialized = false;

        XML_LOAD_PRIVATE::LoadDataVisitor visitor(&_skills, &_equipment, &_battlers, &_defaultEquipmentSlotSchema, &_inclinationAttackingStats, &_inclinationDefendingStats, lov, nullDamageInclination);

        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(xmlfilename) != tinyxml2::XML_SUCCESS) {
            std::cout << xmlfilename << " failed to load successfully with tinyxml2.\n";
            return false;
        }

        if (!doc.Accept(&visitor)) {
            std::cout << "XML doc failed to process visitor.\n";
            return false;
        }

        if (visitor.isFailed()) {
            std::cout << "LoadDataVisitor encountered a problem when parsing the XML doc \"" + std::string(xmlfilename) + "\".\n";
            return false;
        }

        _isInitialized = true;
        return _isInitialized;
    }
}
