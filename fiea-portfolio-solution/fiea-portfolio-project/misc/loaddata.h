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
    /// <summary>
    /// Enumerates all known errors which can occur doing .txt LOV load.
    /// </summary>
    enum class LoadDataError {
        LOAD_OK,
        LOAD_ERR_NOFILE,
        LOAD_ERR_BADFORMAT,
        LOAD_ERR_NOEND,
        LOAD_ERR_DUPKEY,
        LOAD_ERR_BADINSERT,
        LOAD_ERR_BADKEY
    };

    static std::string const LOAD_DEFAULT_DELIM = "  ";                             // Default delimiter. Two spaces, so that names using a single space can be accepted.
    static std::string const LOAD_RES_LOC = "../res";                               // Default location of the .txt files.
    static std::string const LOAD_BATTLERSTAT_NAME = "battlerstat.txt";             // Default name of the file which contains battler stats.
    static std::string const LOAD_DAMAGEINCLINATION_NAME = "damageinclination.txt"; // Default name of the file which contains damage inclinations.
    static std::string const LOAD_DAMAGETYPE_NAME = "damagetype.txt";               // Default name of the file which contains damage types.
    static std::string const LOAD_EQUIPMENTTYPE_NAME = "equipmenttype.txt";         // Default name of the file which contains equipment types.
    static std::string const LOAD_SKILLELEMENT_NAME = "skillelement.txt";           // Default name of the file which contains skill elements.
    static std::string const LOAD_SKILLELEMENTGROUP_NAME = "skillelementgroup.txt"; // Default name of the file which contains skill element groups.

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
    /// <summary>
    /// Interprets a single character as a boolean. ('y' 'Y' 't' 'T' and '1') all evaluate to true. ('n' 'N' 'f' 'F' and '0') all evaluate to false. All other characters are invalid.
    /// </summary>
    /// <param name="output">Results of the parse.</param>
    /// <param name="val">Character to parse.</param>
    /// <returns>LOAD_OK if everything went well, an error otherwise.</returns>
    LoadDataError LoadCharTruthy(bool &output, char val);

    /// <summary>
    /// Four-letter symbol which is at the end of every file to indicate completion. Should be at the beginning of a new line by itself. Its absence will result in a load error.
    /// </summary>
    static ABRV const LOAD_FILE_END_ABRV = ABRV('<', '@', '!', '>');
}
