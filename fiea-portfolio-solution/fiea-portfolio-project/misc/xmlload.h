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
            /// <summary>
            /// Delimiter used to determine if a list has been given in an XML attribute.
            /// </summary>
            static const std::string XML_ATTRLIST_DELIM;

            /*
             * The following fields are pointers which point to the collections this visitor will load.
             * These may simply be the "_my" fields defined below, OR they could be collections outside
             * this visitor entirely. Which is determined by the constructor used.
             */
            SkillMap* _skills;
            EquipmentMap* _equipment;
            BattlerMap* _battlers;
            EquipmentTypeCountMap* _defaultEquipmentSchema;
            DamageInclinationStatListMap* _inclinationAttackingStats;
            DamageInclinationStatListMap* _inclinationDefendingStats;
            const GameLOVStorage* _lov;

            /*
             * These can be thought of as the "default" location of the pointers defined above. If no
             * external destination is given, the pointers will automatically point to these internal
             * collections.
             */
            SkillMap _mySkills;
            EquipmentMap _myEquipment;
            BattlerMap _myBattlers;
            EquipmentTypeCountMap _myDefaultEquipmentSchema;
            DamageInclinationStatListMap _myInclinationAttackingStats;
            DamageInclinationStatListMap _myInclinationDefendingStats;

            /// <summary>
            /// Damage inclination to be assigned to base damage when none is given.
            /// </summary>
            DamageInclination_shptr _nullBaseDamageInclination;

            bool _isFailed;

            /*
             * From here on, many fields will be defined that begin with "_current". These are all so
             * that the visitor can "remember" state while its traveling through the XML hierarchy.
             * Each XML element (that is, something in XML which begins with `<` and ends with `>`)
             * results in a purely independent call to "VisitEnter", with the only exception being
             * elements which begin or end with `/`, which results in a call to "VisitExit". Since each
             * of these calls are independent of each other, the visitor needs to track state on its
             * own using fields like this.
             *
             * Another solution I could have used would have been a generic map. That would have been
             * worse from a data-type-integrity standpoint, but perhaps would have been a bit cleaner
             * to look at in the final object.
             */

            /// <summary>
            /// Does the visitor currently think it's in a settings element?
            /// </summary>
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

            /*
             * Most of the following functions are all extremely similar, so I'll just summarize them here.
             *
             * VisitEnter examines the name of the XML element it's given, and depending on that name it passes
             * it off to one of these functions. They will then proceed to look for specific attributes that
             * should be on that element, and assuming they find them they will set one of these "_current"
             * fields defined above.
             *
             * If it's a "Close" function, it will check to make sure all of the "_current" values it needs are
             * set. Assuming so, a new object is born into the world, passed to its destination, and the
             * corresponding "_current" fields are all cleared. Hooray
             *
             * Each function will return true if everything is successful. If something is NOT successful, it
             * will return false, and the load will be considered failed, which will result in the entire
             * program terminating early.
             */

            /// <summary>
            /// Takes in a string, and parses from it a list of ABRVs.
            /// </summary>
            bool ReadABRVList(std::vector<ABRV>& output, const char* text, const std::string& delim = XML_ATTRLIST_DELIM);
            /// <summary>
            /// Reads a string, and if it's a valid ABRV that's put into the reference.
            /// </summary>
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
            /// <summary>
            /// Constructor. Uses the LOV storage for lookup of ABRV values, and the damage inclination will be the "null damage inclination".
            /// </summary>
            LoadDataVisitor(const GameLOVStorage&, DamageInclination_shptr);
            /// <summary>
            /// Constructor. Allows the collection destinations to be externalized.
            /// Uses the LOV storage for lookup of ABRV values, and the damage inclination will be the "null damage inclination".
            /// </summary>
            LoadDataVisitor(SkillMap*, EquipmentMap*, BattlerMap*, EquipmentTypeCountMap*, DamageInclinationStatListMap* atk, DamageInclinationStatListMap* def, const GameLOVStorage&, DamageInclination_shptr);

            /// <summary>
            /// Enters a XML document.
            /// </summary>
            bool VisitEnter(const XMLDocument& /*doc*/) override;
            /// <summary>
            /// Enters an XML element.
            /// </summary>
            bool VisitEnter(const XMLElement& /*element*/, const XMLAttribute* /*firstAttribute*/) override;
            /// <summary>
            /// Exits an XML element.
            /// </summary>
            bool VisitExit(const XMLElement& /*element*/) override;

            /// <returns>Did the load fail?</returns>
            bool isFailed() const;

            /// <returns>const reference to the internal skill map. Note this may not be the collection which was populated by the load.</returns>
            const SkillMap& mySkills() const;
            /// <returns>const reference to the internal equipment map. Note this may not be the collection which was populated by the load.</returns>
            const EquipmentMap& myEquipment() const;
            /// <returns>const reference to the internal battler map. Note this may not be the collection which was populated by the load.</returns>
            const BattlerMap& myBattlers() const;
            /// <returns>const reference to the internal default equipment slot schema. Note this may not be the collection which was populated by the load.</returns>
            const EquipmentTypeCountMap& myDefaultEquipmentSlotSchema() const;
            /// <returns>const reference to the internal inclination attacking stats. Note this may not be the collection which was populated by the load.</returns>
            const DamageInclinationStatListMap& myInclinationAttackingStats() const;
            /// <returns>const reference to the internal inclination defending stats. Note this may not be the collection which was populated by the load.</returns>
            const DamageInclinationStatListMap& myInclinationDefendingStats() const;
        };
    }
}
