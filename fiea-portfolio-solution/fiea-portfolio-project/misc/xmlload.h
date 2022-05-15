#pragma once
#include <memory>
#include <string>
#include <vector>
#include "../store/gamelovstorage.h"
#include "../external/tinyxml/tinyxml2.h"
#include "../models/battler.h"
#include "../models/equipment.h"
#include "../models/skill.h"

namespace AWE {
    /// <summary>
    /// This namespace primarily exists to isolate usage of the tinyxml2 library and to reduce confusion with GameXLOStorage. GameXLOStorage should be the main source of truth
    /// in regards to all XML-loaded objects.
    /// </summary>
    namespace XML_LOAD_PRIVATE {
        using namespace tinyxml2;

        /// <summary>
        /// Subclass of XMLVisitor made to read the XML file in the `res` folder.
        /// </summary>
        class LoadDataVisitor : public XMLVisitor {
        private:
            static const std::string XML_ATTRLIST_DELIM;

            SkillMap* _skills;
            EquipmentMap* _equipment;
            BattlerMap* _battlers;
            EquipmentTypeCountMap* _defaultEquipmentSchema;
            DamageInclinationStatListMap* _inclinationAttackingStats;
            DamageInclinationStatListMap* _inclinationDefendingStats;
            const GameLOVStorage* _lov;

            SkillMap _mySkills;
            EquipmentMap _myEquipment;
            BattlerMap _myBattlers;
            EquipmentTypeCountMap _myDefaultEquipmentSchema;
            DamageInclinationStatListMap _myInclinationAttackingStats;
            DamageInclinationStatListMap _myInclinationDefendingStats;

            DamageInclination_shptr _nullBaseDamageInclination;

            bool _isFailed;

            bool _inSettings;
            EquipmentTypeCountMap _currentDefaultEquipmentSchema;
            DamageInclinationStatListMap _currentInclinationAttackingStats;
            DamageInclinationStatListMap _currentInclinationDefendingStats;

            std::string _currentSkillName;
            unsigned int _currentSkillTextureIndex;
            std::string _currentSkillSoundFilename;
            std::unique_ptr<SkillBaseDamage> _currentBaseDamage;
            ABRV _currentDamageInclination;
            std::unique_ptr<std::vector<SkillDamage>> _currentDamages;
            std::unique_ptr<std::vector<SkillStatScaling>> _currentStatScalings;
            std::unique_ptr<std::vector<SkillElementBinding>> _currentElementBindings;

            std::string _currentEquipmentName;
            ABRV _currentEquipmentType;
            std::unique_ptr<SkillElementGroupConversionMap> _currentElementConversions;

            std::unique_ptr<BattlerStatValues> _currentStats;

            std::string _currentBattlerName;
            bool _isCurrentBattlerCharacter;
            unsigned short _currentBattlerPriority;
            unsigned int _currentBattlerTextureIndex;
            unsigned int _currentBattlerTextureType;
            EquipmentList _currentStartingEquipment;

            void LogToCout(const std::string& message) const;

            bool ReadABRVList(std::vector<ABRV>& output, const char* text, const std::string& delim = XML_ATTRLIST_DELIM);
            bool ReadABRV(ABRV& output, const char* text);

            bool ReadSettings(const XMLElement&);
            bool ReadDefaultEquipmentSlot(const XMLElement&);
            bool ReadDamageInclinationStats(const XMLElement&);
            bool CloseSettings();

            bool ReadSkill(const XMLElement&);
            bool ReadDamage(const XMLElement&);
            bool ReadBaseDamage(const XMLElement&);
            bool ReadStatScaling(const XMLElement&);
            bool ReadElementBinding(const XMLElement&);
            bool CloseSkill();
            bool CloseDamage();

            bool ReadEquipment(const XMLElement&);
            bool ReadElementConversion(const XMLElement&);
            bool CloseEquipment();

            bool ReadStats(const XMLElement&);
            bool ReadStatValue(const XMLElement&);

            bool ReadBattler(const XMLElement&);
            bool ReadStartingEquipment(const XMLElement&);
            bool CloseBattler();

        public:
            LoadDataVisitor(const GameLOVStorage&, DamageInclination_shptr);
            LoadDataVisitor(SkillMap*, EquipmentMap*, BattlerMap*, EquipmentTypeCountMap*, DamageInclinationStatListMap* atk, DamageInclinationStatListMap* def, const GameLOVStorage&, DamageInclination_shptr);

            bool VisitEnter(const XMLDocument& /*doc*/) override;
            bool VisitEnter(const XMLElement& /*element*/, const XMLAttribute* /*firstAttribute*/) override;
            bool VisitExit(const XMLElement& /*element*/) override;

            bool isFailed() const;

            const SkillMap& mySkills() const;
            const EquipmentMap& myEquipment() const;
            const BattlerMap& myBattlers() const;
            const EquipmentTypeCountMap& myDefaultEquipmentSlotSchema() const;
            const DamageInclinationStatListMap& myInclinationAttackingStats() const;
            const DamageInclinationStatListMap& myInclinationDefendingStats() const;
        };
    }
}
