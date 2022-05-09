#include "gamelovstorage.h"
#include <iostream>
#include "../misc/loaddata.h"

namespace AWE {
    const std::string GameLOVStorage::DEFAULT_LOAD_RES_LOC = "../res";

    bool GameLOVStorage::isInitialized() const { return _isInitialized; }

    const BattlerStat_shptr GameLOVStorage::GetBattlerStat(BattlerStatKey key) const {
        BattlerStat_shptr bstat;

        if (_isInitialized) {
            auto found = _battlerStats.find(key);
            if (found != _battlerStats.end()) {
                bstat = found->second;
            }
        }

        return bstat;
    }

    const DamageInclination_shptr GameLOVStorage::GetDamageInclination(DamageInclinationKey key) const {
        DamageInclination_shptr incl;

        if (_isInitialized) {
            auto found = _damageInclinations.find(key);
            if (found != _damageInclinations.end()) {
                incl = found->second;
            }
        }

        return incl;
    }

    const DamageType_shptr GameLOVStorage::GetDamageType(DamageTypeKey key) const {
        DamageType_shptr dtype;

        if (_isInitialized) {
            auto found = _damageTypes.find(key);
            if (found != _damageTypes.end()) {
                dtype = found->second;
            }
        }

        return dtype;
    }

    const EquipmentType_shptr GameLOVStorage::GetEquipmentType(EquipmentTypeKey key) const {
        EquipmentType_shptr eqtype;

        if (_isInitialized) {
            auto found = _equipmentTypes.find(key);
            if (found != _equipmentTypes.end()) {
                eqtype = found->second;
            }
        }

        return eqtype;
    }

    const SkillElement_shptr GameLOVStorage::GetSkillElement(SkillElementKey key) const {
        SkillElement_shptr ele;

        if (_isInitialized) {
            auto found = _skillElements.find(key);
            if (found != _skillElements.end()) {
                ele = found->second;
            }
        }

        return ele;
    }

    const SkillElementGroup_shptr GameLOVStorage::GetSkillElementGroup(SkillElementGroupKey key) const {
        SkillElementGroup_shptr elegroup;

        if (_isInitialized) {
            auto found = _skillElementGroups.find(key);
            if (found != _skillElementGroups.end()) {
                elegroup = found->second;
            }
        }

        return elegroup;
    }

    const BattlerStatMap& GameLOVStorage::battlerStats() const { return _battlerStats; }
    const DamageInclinationMap& GameLOVStorage::damageInclinations() const { return _damageInclinations; }
    const DamageTypeMap& GameLOVStorage::damageTypes() const { return _damageTypes; }
    const EquipmentTypeMap& GameLOVStorage::equipmentTypes() const { return _equipmentTypes; }
    const SkillElementMap& GameLOVStorage::skillElements() const { return _skillElements; }
    const SkillElementGroupMap& GameLOVStorage::skillElementGroups() const { return _skillElementGroups; }

    bool GameLOVStorage::Initialize(const std::string& resloc) {
        _isInitialized = false;

        std::cout << "Begin load.\n";

        if (AWE::LoadBattlerStats(_battlerStats, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Battler stats loaded: " << _battlerStats.size() << "\n";

        if (AWE::LoadDamageInclinations(_damageInclinations, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Damage inclinations loaded: " << _damageInclinations.size() << "\n";

        if (AWE::LoadDamageTypes(_damageTypes, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Damage types loaded: " << _damageTypes.size() << "\n";

        if (AWE::LoadEquipmentTypes(_equipmentTypes, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Equipment types loaded: " << _equipmentTypes.size() << "\n";

        if (AWE::LoadSkillElements(_skillElements, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Skill elements loaded: " << _skillElements.size() << "\n";

        if (AWE::LoadElementGroups(_skillElementGroups, &_skillElements, resloc) != AWE::LoadDataError::LOAD_OK) {
            return false;
        }

        std::cout << "Skill element groups loaded: " << _skillElementGroups.size() << "\n";

        _isInitialized = true;
        return true;
    }
}
