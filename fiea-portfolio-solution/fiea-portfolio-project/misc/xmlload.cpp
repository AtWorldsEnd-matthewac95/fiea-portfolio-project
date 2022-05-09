#include "xmlload.h"
#include <iostream>

namespace AWE {
    namespace XML_LOAD_PRIVATE {
        using namespace tinyxml2;

        const std::string LoadDataVisitor::XML_ATTRLIST_DELIM = "  ";

        static const char* XMLELE_BASEDAMAGE = "BaseDamage";
        static const char* XMLELE_BATTLER = "Battler";
        static const char* XMLELE_DAMAGE = "Damage";
        static const char* XMLELE_DAMAGEINCLINATIONSTATS = "DamageInclinationStats";
        static const char* XMLELE_DEFAULTEQUIPMENTSLOT = "DefaultEquipmentSlot";
        static const char* XMLELE_ELEMENTBINDING = "ElementBinding";
        static const char* XMLELE_ELEMENTCONVERSION = "ElementConversion";
        static const char* XMLELE_EQUIPMENT = "Equipment";
        static const char* XMLELE_SETTINGS = "Settings";
        static const char* XMLELE_SKILL = "Skill";
        static const char* XMLELE_STARTINGEQUIPMENT = "StartingEquipment";
        static const char* XMLELE_STATS = "Stats";
        static const char* XMLELE_STATSCALING = "StatScaling";
        static const char* XMLELE_STATVALUE = "StatValue";

        static const char* XMLATTR_ATTACK = "attack";
        static const char* XMLATTR_CHARACTER = "character";
        static const char* XMLATTR_DAMAGETYPE = "damagetype";
        static const char* XMLATTR_DEFEND = "defend";
        static const char* XMLATTR_ELEKEY = "elekey";
        static const char* XMLATTR_GROUP = "group";
        static const char* XMLATTR_GROUPKEY = "groupkey";
        static const char* XMLATTR_INCLINATION = "inclination";
        static const char* XMLATTR_KEY = "key";
        static const char* XMLATTR_NAME = "name";
        static const char* XMLATTR_PENETRATING = "penetrating";
        static const char* XMLATTR_PRIORITY = "priority";
        static const char* XMLATTR_SOUND = "sound";
        static const char* XMLATTR_TXR = "txr";
        static const char* XMLATTR_TXRINDEX = "txrindex";

        LoadDataVisitor::LoadDataVisitor(const GameLOVStorage& store, DamageInclination_shptr nullBaseDamageInclination)
            : XMLVisitor()
            , _inSettings(false)
            , _isFailed(false)
            , _skills(nullptr)
            , _equipment(nullptr)
            , _battlers(nullptr)
            , _defaultEquipmentSchema(nullptr)
            , _inclinationAttackingStats(nullptr)
            , _inclinationDefendingStats(nullptr)
            , _lov(&store)
            , _nullBaseDamageInclination(std::move(nullBaseDamageInclination))
            , _currentBattlerPriority(0)
            , _currentSkillTextureIndex(0) {}

        LoadDataVisitor::LoadDataVisitor(
            SkillMap* skillMap,
            EquipmentMap* equipmentMap,
            BattlerMap* battlerMap,
            EquipmentTypeCountMap* defaultEquipmentSlotSchema,
            DamageInclinationStatListMap* inclinationAttackingStats,
            DamageInclinationStatListMap* inclinationDefendingStats,
            const GameLOVStorage& store,
            DamageInclination_shptr nullBaseDamageInclination
        ) : XMLVisitor()
            , _inSettings(false)
            , _isFailed(false)
            , _skills(std::move(skillMap))
            , _equipment(std::move(equipmentMap))
            , _battlers(std::move(battlerMap))
            , _defaultEquipmentSchema(std::move(defaultEquipmentSlotSchema))
            , _inclinationAttackingStats(std::move(inclinationAttackingStats))
            , _inclinationDefendingStats(std::move(inclinationDefendingStats))
            , _lov(&store)
            , _nullBaseDamageInclination(std::move(nullBaseDamageInclination))
            , _currentBattlerPriority(0)
            , _currentSkillTextureIndex(0) {}

        void LoadDataVisitor::LogToCout(const std::string& message) const {
            std::cout << "LoadDataVisitor ==> " << message << "\n";
        }

        bool LoadDataVisitor::ReadABRVList(std::vector<ABRV>& output, const char* text, const std::string& delim) {
            std::string str = std::string(text);

            while (str.length() > 0) {
                size_t pos = 0;
                std::string token;

                if ((pos = str.find(delim)) == std::string::npos) {
                    token = str;
                } else {
                    token = str.substr(0, pos);
                }

                size_t toklen = token.length();
                if (toklen > 0) {
                    if (toklen != ABRV::SIZE) {
                        LogToCout("ABRV in list was not a four-letter string.");
                        return false;
                    }

                    output.push_back(ABRV(token[0], token[1], token[2], token[3]));
                }

                str.erase(0, toklen + delim.length());
            }

            return true;
        }

        bool LoadDataVisitor::ReadABRV(ABRV& output, const char* text) {
            std::string str = std::string(text);

            if (str.length() != ABRV::SIZE) {
                LogToCout("ABRV was not a four-letter string.");
                return false;
            }

            output = ABRV(str[0], str[1], str[2], str[3]);
            return true;
        }

        bool LoadDataVisitor::ReadSettings(const XMLElement& element) {
            // Base case - Only one settings element can be parsed at a time.
            if (_inSettings) {
                LogToCout("Only one settings element can be parsed at a time.");
                return false;
            }

            _inSettings = true;
            return true;
        }

        bool LoadDataVisitor::ReadDefaultEquipmentSlot(const XMLElement& element) {
            // Base case - Settings needs to be started.
            if (!_inSettings) {
                LogToCout("Default equipment slots must be enclosed by a settings element.");
                return false;
            }

            ABRV equipmentTypeAbrv;
            auto attr = element.FindAttribute(XMLATTR_KEY);
            if (attr) {
                if (!ReadABRV(equipmentTypeAbrv, attr->Value())) {
                    return false;
                }
                if (!_lov->equipmentTypes().contains(equipmentTypeAbrv.AsLong())) {
                    LogToCout(equipmentTypeAbrv.AsString() + " is not a valid equipment type.");
                    return false;
                }
                if (_currentDefaultEquipmentSchema.contains(equipmentTypeAbrv.AsLong())) {
                    LogToCout(equipmentTypeAbrv.AsString() + " cannot be inserted more than once.");
                }
            } else {
                LogToCout("Default equipment slot missing required attribute \"key\". (Use an equipment type key!)");
                return false;
            }

            unsigned int count = 0;
            if (element.QueryUnsignedText(&count) != XML_SUCCESS) {
                LogToCout(std::string(element.Value()) + " could not be interpreted as an unsigned integer.");
                return false;
            }

            _currentDefaultEquipmentSchema.insert(std::make_pair(equipmentTypeAbrv.AsLong(), count));
            return true;
        }

        bool LoadDataVisitor::ReadDamageInclinationStats(const XMLElement& element) {
            // Base case - Settings needs to be started.
            if (!_inSettings) {
                LogToCout("Damage inclination stats must be enclosed by a settings element.");
                return false;
            }

            ABRV inclinationAbrv;
            auto attr = element.FindAttribute(XMLATTR_KEY);
            if (attr) {
                if (!ReadABRV(inclinationAbrv, attr->Value())) {
                    return false;
                }
                if (!_lov->damageInclinations().contains(inclinationAbrv.AsLong())) {
                    LogToCout(inclinationAbrv.AsString() + " is not a valid damage inclination.");
                    return false;
                }

                attr = element.FindAttribute(XMLATTR_ATTACK);
            } else {
                LogToCout("Damage inclination stats missing required attribute \"key\". (Use a damage inclination key!)");
                return false;
            }

            BattlerStatList attackingStatPtrs;
            if (attr) {
                std::vector<ABRV> attackingStats;
                if (!ReadABRVList(attackingStats, attr->Value())) {
                    return false;
                }
                for (const ABRV& ast : attackingStats) {
                    auto found = _lov->battlerStats().find(ast.AsLong());
                    if (found == _lov->battlerStats().end()) {
                        LogToCout(ast.AsString() + " is not a valid battler stat.");
                        return false;
                    }
                    attackingStatPtrs.push_back(found->second);
                }

                attr = element.FindAttribute(XMLATTR_DEFEND);
            } else {
                LogToCout("Damage inclination stats missing required attribute \"attack\".");
                return false;
            }

            BattlerStatList defendingStatPtrs;
            if (attr) {
                std::vector<ABRV> defendingStats;
                if (!ReadABRVList(defendingStats, attr->Value())) {
                    return false;
                }
                for (const ABRV& dst : defendingStats) {
                    auto found = _lov->battlerStats().find(dst.AsLong());
                    if (found == _lov->battlerStats().end()) {
                        LogToCout(dst.AsString() + " is not a valid battler stat.");
                        return false;
                    }
                    defendingStatPtrs.push_back(found->second);
                }
            } else {
                LogToCout("Damage inclination stats missing required attribute \"attack\".");
                return false;
            }

            _currentInclinationAttackingStats.insert(std::make_pair(inclinationAbrv.AsLong(), std::move(attackingStatPtrs)));
            _currentInclinationDefendingStats.insert(std::make_pair(inclinationAbrv.AsLong(), std::move(defendingStatPtrs)));
            return true;
        }

        bool LoadDataVisitor::ReadSkill(const XMLElement& element) {
            // Base case - No more than one skill element can be parsed at a time.
            if (!_currentSkillName.empty()) {
                LogToCout("Another skill was started before " + _currentSkillName + " was closed.");
                return false;
            }
            if (_currentSkillTextureIndex > 0 || !_currentSkillSoundFilename.empty()) {
                LogToCout("Dangling skill information was not cleared before starting another skill.");
                return false;
            }

            std::string skillname;
            auto attr = element.FindAttribute(XMLATTR_NAME);
            if (attr) {
                skillname = std::string(attr->Value());
            } else {
                LogToCout("Skills must have a name.");
                return false;
            }

            attr = element.FindAttribute(XMLATTR_TXRINDEX);
            if (attr) {
                if (attr->QueryUnsignedValue(&_currentSkillTextureIndex) != XML_SUCCESS) {
                    LogToCout(std::string(XMLATTR_TXRINDEX) + " must be an unsigned integer.");
                    return false;
                }
            }

            attr = element.FindAttribute(XMLATTR_SOUND);
            if (attr) {
                _currentSkillSoundFilename = std::string(attr->Value());
            } else {
                _currentSkillSoundFilename = "";
            }

            _currentSkillName = std::move(skillname);
            return true;
        }

        bool LoadDataVisitor::ReadDamage(const XMLElement& element) {
            // Base case - Skill needs to be started.
            if (_currentSkillName.empty()) {
                LogToCout("Damage elements must be enclosed in skills.");
                return false;
            }
            // Base case - No more than one damage element can be parsed at a time.
            // NOTE THE ! BELOW
            if (!ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("A damage element was started before another one was closed.");
                return false;
            }

            auto attr = element.FindAttribute(XMLATTR_INCLINATION);
            if (attr) {
                if (ReadABRV(_currentDamageInclination, attr->Value())) {
                    if (DamageInclination::AUTO_KEY.Equals(_currentDamageInclination) ? true : (bool)_lov->GetDamageInclination(_currentDamageInclination.AsLong())) {
                        return true;
                    } else {
                        LogToCout(_currentDamageInclination.AsString() + " is not a valid damage inclination.");
                        return false;
                    }
                } else {
                    return false;
                }
            }

            _currentDamageInclination = DamageInclination::AUTO_KEY;
            return true;
        }

        bool LoadDataVisitor::ReadBaseDamage(const XMLElement& element) {
            // Base case - Skill needs to be started.
            if (_currentSkillName.empty()) {
                LogToCout("Base damage elements must be enclosed in damage elements, which must in turn be enclosed in skills.");
                return false;
            }
            // Base case - Damage needs to be started.
            if (ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("Base damage elements must be enclosed in damage elements.");
                return false;
            }
            // Base case - Damage can't have more than one base damage element.
            if (_currentBaseDamage) {
                LogToCout("Damage elements can only have one base damage element.");
                return false;
            }

            auto attr = element.FindAttribute(XMLATTR_INCLINATION);
            if (attr) {
                ABRV abrv;
                if (ReadABRV(abrv, attr->Value())) {
                    const DamageInclination_shptr& incl = _lov->GetDamageInclination(abrv.AsLong());
                    if (incl) {
                        if (DamageInclination::AUTO_KEY.AsLong() == incl->abrvlong()) {
                            LogToCout("Base damage cannot have an inclination of AUTO.");
                            return false;
                        }

                        int value = 0;
                        if (element.QueryIntText(&value) == XML_SUCCESS) {
                            _currentBaseDamage = std::make_unique<SkillBaseDamage>(SkillBaseDamage(value, incl));
                            return true;
                        }

                        LogToCout("Base damage value was not a valid integer.");
                        return false;
                    }

                    LogToCout(abrv.AsString() + " is not a valid damage inclination.");
                    return false;
                }

                return false;
            }

            LogToCout("Base damage must be given a non-AUTO inclination.");
            return false;
        }

        bool LoadDataVisitor::ReadStatScaling(const XMLElement& element) {
            // Base case - Skill needs to be started.
            if (_currentSkillName.empty()) {
                LogToCout("Stat scaling elements must be enclosed in damage elements, which must in turn be enclosed in skills.");
                return false;
            }
            // Base case - Damage needs to be started.
            if (ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("Stat scaling elements must be enclosed in damage elements.");
                return false;
            }

            ABRV abrv;
            DamageInclination_shptr incl;
            auto attr = element.FindAttribute(XMLATTR_INCLINATION);
            if (attr) {
                if (ReadABRV(abrv, attr->Value())) {
                    incl = _lov->GetDamageInclination(abrv.AsLong());
                    if (incl) {
                        attr = element.FindAttribute(XMLATTR_KEY);
                    } else {
                        LogToCout(incl->abrvstr() + " is not a valid damage inclination.");
                        return false;
                    }
                } else {
                    return false;
                }
            }

            std::vector<ABRV> abrvs;
            if (attr) {
                if (!ReadABRVList(abrvs, attr->Value())) {
                    return false;
                }
            }

            if (incl && !abrvs.empty()) {
                float value = 0.0f;
                if (element.QueryFloatText(&value) == XML_SUCCESS) {
                    for (const ABRV& l_abrv : abrvs) {
                        BattlerStat_shptr bstat = _lov->GetBattlerStat(l_abrv.AsLong());

                        if (bstat) {
                            if (!_currentStatScalings) {
                                _currentStatScalings = std::make_unique<std::vector<SkillStatScaling>>(std::vector<SkillStatScaling>());
                            }
                            _currentStatScalings->push_back(SkillStatScaling(value, incl, bstat));
                        } else {
                            LogToCout(l_abrv.AsString() + " is not a valid battler stat.");
                            return false;
                        }
                    }

                    return true;
                }
            }

            LogToCout("Stat scaling elements need battler stats.");
            return false;
        }

        bool LoadDataVisitor::ReadElementBinding(const XMLElement& element) {
            // Base case - Skill needs to be started.
            if (_currentSkillName.empty()) {
                LogToCout("Element binding elements must be enclosed in damage elements, which must in turn be enclosed in skills.");
                return false;
            }
            // Base case - Damage needs to be started.
            if (ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("Element binding elements must be enclosed in damage elements.");
                return false;
            }

            ABRV abrv;
            DamageInclination_shptr incl;
            DamageType_shptr dtype;
            SkillElement_shptr ele;
            SkillElementGroup_shptr elegroup;
            bool group = false;
            bool pen = false;
            auto attr = element.FindAttribute(XMLATTR_INCLINATION);
            if (attr) {
                if (ReadABRV(abrv, attr->Value())) {
                    incl = _lov->GetDamageInclination(abrv.AsLong());
                    if (incl) {
                        attr = element.FindAttribute(XMLATTR_GROUP);
                    } else {
                        LogToCout(abrv.AsString() + " is not a valid damage inclination.");
                        return false;
                    }
                } else {
                    return false;
                }
            }

            if (attr) {
                if (attr->QueryBoolValue(&group) == XML_SUCCESS) {
                    attr = element.FindAttribute(XMLATTR_KEY);
                } else {
                    LogToCout(std::string(attr->Value()) + " could not be interpreted as a boolean value.");
                    return false;
                }
            }

            if (attr) {
                if (ReadABRV(abrv, attr->Value())) {
                    ele = group ? ele : _lov->GetSkillElement(abrv.AsLong());
                    elegroup = group ? _lov->GetSkillElementGroup(abrv.AsLong()) : elegroup;
                    if (ele || elegroup) {
                        attr = element.FindAttribute(XMLATTR_DAMAGETYPE);
                    } else {
                        LogToCout(abrv.AsString() + " is not a valid skill element" + (group ? " group" : "") + ".");
                        return false;
                    }
                } else {
                    return false;
                }
            }

            if (attr) {
                if (ReadABRV(abrv, attr->Value())) {
                    dtype = _lov->GetDamageType(abrv.AsLong());
                    if (dtype) {
                        attr = element.FindAttribute(XMLATTR_PENETRATING);
                    } else {
                        LogToCout(abrv.AsString() + " is not a valid damage type.");
                        return false;
                    }
                } else {
                    return false;
                }
            }

            if (attr) {
                if (attr->QueryBoolValue(&pen) != XML_SUCCESS) {
                    LogToCout(std::string(attr->Value()) + " could not be interpreted as a boolean value.");
                    return false;
                }
            }

            if (incl && (ele || group) && (elegroup || !group) && dtype) {
                float value = 0.0f;
                if (element.QueryFloatText(&value) == XML_SUCCESS) {
                    if (!_currentElementBindings) {
                        _currentElementBindings = std::make_unique<std::vector<SkillElementBinding>>(std::vector<SkillElementBinding>());
                    }
                    _currentElementBindings->push_back(group ? SkillElementBinding(pen, value, incl, dtype, elegroup) : SkillElementBinding(pen, value, incl, dtype, ele));
                    return true;
                }
                LogToCout(std::string(element.Value()) + " could not be interpreted as a floating point number.");
            }

            return false;
        }

        bool LoadDataVisitor::ReadEquipment(const XMLElement& element) {
            // Base case - No more than one equipment element can be parsed at a time.
            // NOTE THE ! BELOW
            if (!_currentEquipmentName.empty() || !ABRV::INVALID.Equals(_currentEquipmentType)) {
                LogToCout("No more than one equipment element can be parsed at a time.");
                return false;
            }
            // Base case - Stats cannot be started yet.
            if (_currentStats) {
                LogToCout("Equipment cannot be started if a stats element is already started.");
                return false;
            }

            auto attr = element.FindAttribute(XMLATTR_NAME);
            if (attr) {
                _currentEquipmentName = std::string(attr->Value());
                attr = element.FindAttribute(XMLATTR_KEY);
            }

            if (attr) {
                if (ReadABRV(_currentEquipmentType, attr->Value())) {
                    if (_lov->equipmentTypes().contains(_currentEquipmentType.AsLong())) {
                        return true;
                    } else {
                        LogToCout(_currentEquipmentType.AsString() + " is not a valid equipment type.");
                        return false;
                    }
                }
            }

            LogToCout("Equipment must have a name.");
            return false;
        }

        bool LoadDataVisitor::ReadElementConversion(const XMLElement& element) {
            // Base case - Equipment needs to be started.
            if (_currentEquipmentName.empty() || ABRV::INVALID.Equals(_currentEquipmentType)) {
                LogToCout("Element conversion elements must be enclosed in equipment.");
                return false;
            }

            ABRV abrv;
            SkillElementGroupKey groupkey;
            auto attr = element.FindAttribute(XMLATTR_GROUPKEY);
            if (attr) {
                if (!ReadABRV(abrv, attr->Value())) {
                    return false;
                }
                groupkey = abrv.AsLong();

                if (!_lov->skillElementGroups().contains(groupkey)) {
                    LogToCout(abrv.AsString() + " is not a valid skill element group.");
                    return false;
                }

                attr = element.FindAttribute(XMLATTR_ELEKEY);
            }

            SkillElement_shptr skillele;
            if (attr) {
                if (!ReadABRV(abrv, attr->Value())) {
                    return false;
                }

                auto found = _lov->skillElements().find(abrv.AsLong());
                if (found == _lov->skillElements().end()) {
                    LogToCout(abrv.AsString() + " is not a valid skill element.");
                    return false;
                }
                skillele = found->second;
            }

            if (skillele) {
                if (!_currentElementConversions) {
                    _currentElementConversions = std::make_unique<SkillElementGroupConversionMap>(SkillElementGroupConversionMap());
                }
                _currentElementConversions->insert(std::make_pair(groupkey, std::move(skillele)));
                return true;
            }

            LogToCout("Skill element conversion is missing a required attribute.");
            return false;
        }

        bool LoadDataVisitor::ReadStats(const XMLElement& element) {
            // Base case - Only one stats element can be parsed per enclosing element.
            if (_currentStats) {
                LogToCout("Only one stats element can be parsed per enclosing element.");
                return false;
            }
            _currentStats = std::make_unique<BattlerStatValues>(BattlerStatValues());
            return true;
        }

        bool LoadDataVisitor::ReadStatValue(const XMLElement& element) {
            // Base case - Stats needs to be started.
            // NOTE THE ! BELOW
            if (!_currentStats) {
                LogToCout("Stat value elements must be enclosed in stat elements.");
                return false;
            }

            ABRV stat;
            auto attr = element.FindAttribute(XMLATTR_KEY);
            if (!attr || !ReadABRV(stat, attr->Value()) || !_lov->battlerStats().contains(stat.AsLong())) {
                LogToCout("Problem occurred when reading stat value \"key\" attribute.");
                return false;
            }

            BattlerStatValue val;
            if (element.QueryUnsignedText(&val) != XML_SUCCESS) {
                LogToCout(std::string(element.Value()) + " could not be interpreted as an unsigned integer.");
                return false;
            }

            _currentStats->insert(std::make_pair(stat.AsLong(), val));
            return true;
        }

        bool LoadDataVisitor::ReadBattler(const XMLElement& element) {
            // Base case - Only one battler element can be parsed at a time.
            // NOTE THE ! BELOW
            if (!_currentBattlerName.empty() || !_currentStartingEquipment.empty()) {
                LogToCout("Only one battler element can be parsed at a time.");
                return false;
            }
            // Base case - Stats cannot be started yet.
            if (_currentStats) {
                LogToCout("Battlers cannot be started if a stats element is already started.");
                return false;
            }

            auto attr = element.FindAttribute(XMLATTR_NAME);
            if (attr) {
                _currentBattlerName = std::string(attr->Value());
            } else {
                LogToCout("Battlers must have a name.");
                return false;
            }

            attr = element.FindAttribute(XMLATTR_PRIORITY);
            unsigned int val;
            if (attr && attr->QueryUnsignedValue(&val) == XML_SUCCESS) {
                _currentBattlerPriority = static_cast<unsigned short>(val);
            } else {
                LogToCout(_currentBattlerName + "'s priority must be an unsigned short integer.");
                return false;
            }

            attr = element.FindAttribute(XMLATTR_TXR);
            if (attr && attr->QueryUnsignedValue(&val) == XML_SUCCESS) {
                _currentBattlerTextureType = val;
            } else {
                LogToCout(_currentBattlerName + "'s texture type must be an unsigned integer.");
                return false;
            }

            attr = element.FindAttribute(XMLATTR_TXRINDEX);
            if (attr) {
                if (attr->QueryUnsignedValue(&val) == XML_SUCCESS) {
                    _currentBattlerTextureIndex = val;
                } else {
                    LogToCout(_currentBattlerName + "'s texture index must be an unsigned integer (or left blank to default to 0).");
                    return false;
                }
            } else {
                _currentBattlerTextureIndex = 0;
            }

            attr = element.FindAttribute(XMLATTR_CHARACTER);
            if (attr) {
                if (!attr->QueryBoolValue(&_isCurrentBattlerCharacter) == XML_SUCCESS) {
                    LogToCout(_currentBattlerName + "'s character flag couldn't be interpreted as a bool value.");
                    return false;
                }
            } else {
                _isCurrentBattlerCharacter = false;
            }

            return true;
        }

        bool LoadDataVisitor::ReadStartingEquipment(const XMLElement& element) {
            // Base case - Battler needs to be started.
            if (_currentBattlerName.empty()) {
                LogToCout("Starting equipment elements need to be enclosed in battlers.");
                return false;
            }

            auto attr = element.FindAttribute(XMLATTR_NAME);
            if (!attr) {
                LogToCout("No name provided for starting equipment.");
                return false;
            }

            std::string equipname(attr->Value());
            auto found = _equipment->find(equipname);
            if (found == _equipment->end()) {
                LogToCout(equipname + " is not a valid equipment. (Perhaps it hasn't been loaded yet?)");
                return false;
            }

            _currentStartingEquipment.push_back(found->second);
            return true;
        }

        bool LoadDataVisitor::CloseSettings() {
            // Base case - Settings must be started first.
            // NOTE THE ! BELOW
            if (!_inSettings) {
                LogToCout("Settings attempted to close before being initialized properly.");
                return false;
            }

            _defaultEquipmentSchema->clear();
            for (const EquipmentTypeCountMap::value_type& eqitr : _currentDefaultEquipmentSchema) {
                _defaultEquipmentSchema->insert(eqitr);
            }

            _inclinationAttackingStats->clear();
            for (const DamageInclinationStatListMap::value_type& atitr : _currentInclinationAttackingStats) {
                _inclinationAttackingStats->insert(atitr);
            }

            _inclinationDefendingStats->clear();
            for (const DamageInclinationStatListMap::value_type& dfitr : _currentInclinationDefendingStats) {
                _inclinationDefendingStats->insert(dfitr);
            }

            _currentDefaultEquipmentSchema.clear();
            _currentInclinationAttackingStats.clear();
            _currentInclinationDefendingStats.clear();
            _inSettings = false;
            return true;
        }

        bool LoadDataVisitor::CloseSkill() {
            // Base case - Damage must be closed first.
            // NOTE THE ! BELOW
            if (!ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("Skills cannot be closed before their damage elements are closed.");
                return false;
            }

            // If this pointer is not reset, then we have a dangling damage element, which is bad.
            if (_currentBaseDamage) {
                LogToCout("Dangling base damage pointer! How did this happen??");
                return false;
            }

            _skills->insert(std::make_pair(_currentSkillName, Skill_shptr(_currentDamages
                ? new Skill(_currentSkillName, *_currentDamages, _currentSkillTextureIndex, _currentSkillSoundFilename)
                : new Skill(_currentSkillName, _currentSkillTextureIndex, _currentSkillSoundFilename))));

            _currentSkillSoundFilename = "";
            _currentSkillTextureIndex = 0;
            _currentSkillName = "";
            _currentDamages.reset();
            return true;
        }

        bool LoadDataVisitor::CloseDamage() {
            // Base case - Damage needs an inclination, even if the inclination is just 'AUTO'
            if (ABRV::INVALID.Equals(_currentDamageInclination)) {
                LogToCout("Damage needs an inclination, even if the inclination is just AUTO.");
                return false;
            }

            if (!_currentBaseDamage) {
                _currentBaseDamage = std::make_unique<SkillBaseDamage>(SkillBaseDamage(0, _nullBaseDamageInclination));
            }

            if (!_currentDamages) {
                _currentDamages = std::make_unique<std::vector<SkillDamage>>(std::vector<SkillDamage>());
            }

            if (_currentStatScalings && _currentElementBindings) {
                _currentDamages->push_back(SkillDamage(*_currentBaseDamage, _currentDamageInclination.AsLong(), *_currentStatScalings, *_currentElementBindings));
            } else if (_currentStatScalings) {
                LogToCout("Damage stat scalings were provided without corresponding element bindings.");
                return false;
            } else if (_currentElementBindings) {
                LogToCout("Damage element bindings were provided without corresponding stat scalings.");
                return false;
            } else {
                _currentDamages->push_back(SkillDamage(*_currentBaseDamage, _currentDamageInclination.AsLong()));
            }

            _currentDamageInclination = ABRV::INVALID;
            _currentBaseDamage.reset();
            _currentStatScalings.reset();
            _currentElementBindings.reset();
            return true;
        }

        bool LoadDataVisitor::CloseEquipment() {
            // Base case - Equipment needs to be started.
            if (_currentEquipmentName.empty() || ABRV::INVALID.Equals(_currentEquipmentType)) {
                LogToCout("Equipment tried to close without being properly initialized.");
                return false;
            }
            // Base case - Element conversions need to be started.
            // NOTE THE ! BELOW
            if (!_currentElementConversions) {
                LogToCout("Equipment need at least one element conversion.");
                return false;
            }

            // It's possible for a weapon to provide no bonus stats.
            if (!_currentStats) {
                _currentStats = std::make_unique<BattlerStatValues>(BattlerStatValues());
            }

            DamageResistances _;
            DamageSourceMap fullDamageSources = CreateFullDamageSources(_lov->damageInclinations(), _lov->damageTypes());
            _equipment->insert(std::make_pair(_currentEquipmentName, std::make_shared<Equipment>(Equipment(
                _currentEquipmentName,
                _,
                fullDamageSources,
                _lov->equipmentTypes().at(_currentEquipmentType.AsLong()),
                *_skills,
                *_currentStats,
                *_currentElementConversions
            ))));

            _currentEquipmentName = "";
            _currentEquipmentType = ABRV::INVALID;
            _currentElementConversions.reset();
            _currentStats.reset();

            return true;
        }

        bool LoadDataVisitor::CloseBattler() {
            // Base case - Battler needs to be started.
            if (_currentBattlerName.empty()) {
                LogToCout("Battler tried to close without being properly initialized.");
                return false;
            }

            // It's possible for a battler to have no stats.
            if (!_currentStats) {
                _currentStats = std::make_unique<BattlerStatValues>(BattlerStatValues());
            }

            DamageResistances _;
            DamageSourceMap fullDamageSources = CreateFullDamageSources(_lov->damageInclinations(), _lov->damageTypes());
            ElementalAffinities __;
            _battlers->insert(std::make_pair(_currentBattlerName, std::make_shared<Battler>(Battler(
                _currentBattlerName,
                _isCurrentBattlerCharacter,
                _currentBattlerPriority,
                _currentBattlerTextureIndex,
                _currentBattlerTextureType,
                *_currentStats,
                _,
                fullDamageSources,
                __,
                _currentStartingEquipment
            ))));

            _currentBattlerName = "";
            _isCurrentBattlerCharacter = false;
            _currentBattlerPriority = 0;
            _currentBattlerTextureIndex = 0;
            _currentBattlerTextureType = 0;
            _currentStats.reset();
            _currentStartingEquipment.clear();

            return true;
        }

        bool LoadDataVisitor::VisitEnter(const XMLDocument& doc) {
            if (_skills == nullptr) {
                _skills = &_mySkills;
            }
            if (_equipment == nullptr) {
                _equipment = &_myEquipment;
            }
            if (_battlers == nullptr) {
                _battlers = &_myBattlers;
            }
            if (_defaultEquipmentSchema == nullptr) {
                _defaultEquipmentSchema = &_myDefaultEquipmentSchema;
            }
            if (_inclinationAttackingStats == nullptr) {
                _inclinationAttackingStats = &_myInclinationAttackingStats;
            }
            if (_inclinationDefendingStats == nullptr) {
                _inclinationDefendingStats = &_myInclinationDefendingStats;
            }
            return true;
        }

        bool LoadDataVisitor::VisitEnter(const XMLElement& element, const XMLAttribute* firstAttribute) {
            bool success = true;

            const char* name = element.Name();
            if (strcmp(XMLELE_SETTINGS, name) == 0) {
                success = ReadSettings(element);
            } else if (strcmp(XMLELE_DEFAULTEQUIPMENTSLOT, name) == 0) {
                success = ReadDefaultEquipmentSlot(element);
            } else if (strcmp(XMLELE_DAMAGEINCLINATIONSTATS, name) == 0) {
                success = ReadDamageInclinationStats(element);
            } else if (strcmp(XMLELE_SKILL, name) == 0) {
                success = ReadSkill(element);
            } else if (strcmp(XMLELE_DAMAGE, name) == 0) {
                success = ReadDamage(element);
            } else if (strcmp(XMLELE_BASEDAMAGE, name) == 0) {
                success = ReadBaseDamage(element);
            } else if (strcmp(XMLELE_STATSCALING, name) == 0) {
                success = ReadStatScaling(element);
            } else if (strcmp(XMLELE_ELEMENTBINDING, name) == 0) {
                success = ReadElementBinding(element);
            } else if (strcmp(XMLELE_EQUIPMENT, name) == 0) {
                success = ReadEquipment(element);
            } else if (strcmp(XMLELE_ELEMENTCONVERSION, name) == 0) {
                success = ReadElementConversion(element);
            } else if (strcmp(XMLELE_STATS, name) == 0) {
                success = ReadStats(element);
            } else if (strcmp(XMLELE_STATVALUE, name) == 0) {
                success = ReadStatValue(element);
            } else if (strcmp(XMLELE_BATTLER, name) == 0) {
                success = ReadBattler(element);
            } else if (strcmp(XMLELE_STARTINGEQUIPMENT, name) == 0) {
                success = ReadStartingEquipment(element);
            }

            if (!success) { _isFailed = true; }

            return success;
        }

        bool LoadDataVisitor::VisitExit(const XMLElement& element) {
            bool success = true;

            const char* name = element.Name();
            if (strcmp(XMLELE_SETTINGS, name) == 0) {
                success = CloseSettings();
            } else if (strcmp(XMLELE_SKILL, name) == 0) {
                success = CloseSkill();
            } else if (strcmp(XMLELE_DAMAGE, name) == 0) {
                success = CloseDamage();
            } else if (strcmp(XMLELE_EQUIPMENT, name) == 0) {
                success = CloseEquipment();
            } else if (strcmp(XMLELE_BATTLER, name) == 0) {
                success = CloseBattler();
            }

            if (!success) { _isFailed = true; }

            return success;
        }

        bool LoadDataVisitor::isFailed() const { return _isFailed; }

        const SkillMap& LoadDataVisitor::mySkills() const { return _mySkills; }
        const EquipmentMap& LoadDataVisitor::myEquipment() const { return _myEquipment; }
        const BattlerMap& LoadDataVisitor::myBattlers() const { return _myBattlers; }
        const EquipmentTypeCountMap& LoadDataVisitor::myDefaultEquipmentSlotSchema() const { return _myDefaultEquipmentSchema; }
        const DamageInclinationStatListMap& LoadDataVisitor::myInclinationAttackingStats() const { return _myInclinationAttackingStats; }
        const DamageInclinationStatListMap& LoadDataVisitor::myInclinationDefendingStats() const { return _myInclinationDefendingStats; }
    }
}
