#pragma once
#include <string>
#include "messageformats.h"
#include "../abrv/abrv.h"
#include "../models/battlerstat.h"
#include "../models/damageinclination.h"
#include "../models/damagetype.h"
#include "../models/equipmenttype.h"
#include "../models/skill.h"
#include "../models/skillelement.h"
#include "../models/skillelementgroup.h"

namespace AWE {
    enum class LoadDataError {
        LOAD_OK,
        LOAD_ERR_NOFILE,
        LOAD_ERR_BADFORMAT,
        LOAD_ERR_NOEND,
        LOAD_ERR_DUPKEY,
        LOAD_ERR_BADINSERT,
        LOAD_ERR_BADKEY
    };

    static std::string const LOAD_DEFAULT_DELIM = "  ";
    static std::string const LOAD_RES_LOC = "../res";
    static std::string const LOAD_BATTLERSTAT_NAME = "battlerstat.txt";
    static std::string const LOAD_DAMAGEINCLINATION_NAME = "damageinclination.txt";
    static std::string const LOAD_DAMAGETYPE_NAME = "damagetype.txt";
    static std::string const LOAD_EQUIPMENTTYPE_NAME = "equipmenttype.txt";
    static std::string const LOAD_SKILLELEMENT_NAME = "skillelement.txt";
    static std::string const LOAD_SKILLELEMENTGROUP_NAME = "skillelementgroup.txt";

    LoadDataError LoadBattlerStats(BattlerStatMap &output, std::string location = LOAD_RES_LOC, std::string delim = LOAD_DEFAULT_DELIM, std::string filename = LOAD_BATTLERSTAT_NAME);
    LoadDataError LoadDamageInclinations(DamageInclinationMap &output, std::string location = LOAD_RES_LOC, std::string delim = LOAD_DEFAULT_DELIM, std::string filename = LOAD_DAMAGEINCLINATION_NAME);
    LoadDataError LoadDamageTypes(DamageTypeMap &output, std::string location = LOAD_RES_LOC, std::string delim = LOAD_DEFAULT_DELIM, std::string filename = LOAD_DAMAGETYPE_NAME);
    LoadDataError LoadEquipmentTypes(EquipmentTypeMap& output, std::string location = LOAD_RES_LOC, std::string delim = LOAD_DEFAULT_DELIM, std::string filename = LOAD_EQUIPMENTTYPE_NAME);
    LoadDataError LoadSkillElements(SkillElementMap &output, std::string location = LOAD_RES_LOC, std::string delim = LOAD_DEFAULT_DELIM, std::string filename = LOAD_SKILLELEMENT_NAME);
    LoadDataError LoadElementGroups(
        SkillElementGroupMap &output,
        SkillElementMap* elementMap,
        std::string location = LOAD_RES_LOC,
        std::string delim = LOAD_DEFAULT_DELIM,
        std::string filename = LOAD_SKILLELEMENTGROUP_NAME
    );
    LoadDataError LoadCharTruthy(bool &output, char val);

    static ABRV const LOAD_FILE_END_ABRV = ABRV('<', '@', '!', '>');
}
