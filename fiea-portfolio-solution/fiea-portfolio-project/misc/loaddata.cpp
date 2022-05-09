#include "loaddata.h"
#include <iostream>
#include <fstream>
#include "messageformats.h"
#include "stringutils.h"

namespace AWE {
    namespace Load {
        template <typename T>
        LoadDataError LoadFromFile_Standard(std::unordered_map<ABRV_long, T> &output, MessageFormats &messages, std::string delim, std::string fileLocName, T (*create)(std::string, ABRV), const ABRV* badKey = nullptr) {
            std::ifstream file;
            file.open(fileLocName, std::ios::in);

            if (!file.is_open()) {
                messages.failure(messages.failure().append(" does not exist."));
                return LoadDataError::LOAD_ERR_NOFILE;
            }

            std::string line;
            bool noEndfile = true;
            while (std::getline(file, line)) {
                line = string_trim(line);

                if (line.length() < 1) {
                    continue;
                }

                LoadDataError helperError = CreateFromLine_Standard<T>(output, messages, noEndfile, line, delim, create);

                if (helperError != LoadDataError::LOAD_OK) {
                    file.close();
                    return helperError;
                }

                if (!noEndfile) {
                    file.close();
                    break;
                }
            }

            if (noEndfile) {
                messages.failure(messages.failure().append(" does not have the end-file indicator \"").append(LOAD_FILE_END_ABRV.AsString()).append("\"."));
                file.close();
                return LoadDataError::LOAD_ERR_NOEND;
            } else {
                messages.success(messages.success().append(" - load complete."));
                messages.isSuccess(true);
            }

            return LoadDataError::LOAD_OK;
        }

        template <typename T>
        LoadDataError CreateFromLine_Standard(std::unordered_map<ABRV_long, T> &output, MessageFormats &messages, bool &noEndfile, std::string line, std::string delim, T (*create)(std::string, ABRV), const ABRV* badKey = nullptr) {
            size_t pos = 0;
            bool noDelim = ((pos = line.find(delim)) == std::string::npos);

            std::string token;
            if (noDelim) {
                token = line;
            } else {
                token = line.substr(0, pos);
            }

            if (token.length() != ABRV::SIZE) {
                messages.failure(messages.failure().append(" - \"").append(token).append("\" is not a four-letter abbreviation."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            ABRV abrv(token[0], token[1], token[2], token[3]);

            if (LOAD_FILE_END_ABRV.Equals(abrv)) {
                noEndfile = false;
                return LoadDataError::LOAD_OK;
            } else if (noDelim) {
                messages.failure(messages.failure().append(" Line \"").append(line).append("\" has no delimiter (").append(delim).append(")."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            if (badKey != nullptr && badKey->Equals(abrv)) {
                messages.failure(messages.failure().append(" - Key \"").append(token).append("\" is not allowed for ").append(messages.other()));
                return LoadDataError::LOAD_ERR_BADKEY;
            }

            line.erase(0, pos + delim.length());

            if (line.length() < 1) {
                messages.failure(messages.failure().append(" - \"").append(token).append("\" has no name."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            ABRV_long abrvLong = abrv.AsLong();

            if (output.find(abrvLong) != output.end()) {
                messages.failure(messages.failure().append(" - Key \"").append(token).append("\" is already in use."));
                return LoadDataError::LOAD_ERR_DUPKEY;
            }

            output.insert(std::make_pair(abrvLong, create(line, abrv)));

            if (output.find(abrvLong) == output.end()) {
                messages.failure(messages.failure().append(" - (").append(line).append(", ").append(token).append(") failed to insert."));
                return LoadDataError::LOAD_ERR_BADINSERT;
            } else {
                std::cout << messages.other() << ": Loaded \"" << line << "\".\n";
            }

            return LoadDataError::LOAD_OK;
        }

        template <typename T>
        LoadDataError CreateFromLine_IgnoreEndfile(std::unordered_map<ABRV_long, T> &output, MessageFormats &messages, ABRV_long &abrvLong, std::string line, std::string delim, bool printCreate, T (*create)(std::string, ABRV)) {
            size_t pos = 0;

            if ((pos = line.find(delim)) == std::string::npos) {
                messages.failure(messages.failure().append(" Line \"").append(line).append("\" is missing a delimiter (").append(delim).append(")."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            std::string token = line.substr(0, pos);

            if (token.length() != ABRV::SIZE) {
                messages.failure(messages.failure().append(" - \"").append(token).append("\" is not a four-letter abbreviation."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            ABRV abrv(token[0], token[1], token[2], token[3]);
            line.erase(0, pos + delim.length());

            if (line.length() < 1) {
                messages.failure(messages.failure().append(" - \"").append(token).append("\" has no name."));
                return LoadDataError::LOAD_ERR_BADFORMAT;
            }

            abrvLong = abrv.AsLong();

            if (output.find(abrvLong) != output.end()) {
                messages.failure(messages.failure().append(" - Key \"").append(token).append("\" is already in use."));
                return LoadDataError::LOAD_ERR_DUPKEY;
            }

            output.insert(std::make_pair(abrvLong, create(line, abrv)));

            if (output.find(abrvLong) == output.end()) {
                messages.failure(messages.failure().append(" - (").append(line).append(", ").append(token).append(") failed to insert."));
                return LoadDataError::LOAD_ERR_BADINSERT;
            } else if (printCreate) {
                std::cout << messages.other() << ": Loaded \"" << line << "\".\n";
            }

            return LoadDataError::LOAD_OK;
        }

        template <typename T>
        LoadDataError ParseAbrvLine_CheckEndfile(std::vector<T>& output, MessageFormats& messages, std::unordered_map<ABRV_long, T>* map, bool &noEndfile, std::string line, std::string delim) {
            size_t pos = 0;
            std::string token;
            ABRV* abrv = nullptr;
            ABRV_long abrvLong = 0;
            bool moreAbrvs = true;

            while (moreAbrvs) {
                if ((pos = line.find(delim)) == std::string::npos) {
                    token = line;
                    moreAbrvs = false;
                } else {
                    token = line.substr(0, pos);
                }

                if (token.length() != ABRV::SIZE) {
                    messages.failure(messages.failure().append(" - \"").append(token).append("\" is not a four-letter abbreviation.\n"));
                    return LoadDataError::LOAD_ERR_BADFORMAT;
                }

                abrv = new ABRV(token[0], token[1], token[2], token[3]);

                if (abrv->Equals(LOAD_FILE_END_ABRV)) {
                    noEndfile = false;
                    break;
                }

                abrvLong = abrv->AsLong();

                if (map->find(abrvLong) == map->end()) {
                    messages.failure(messages.failure().append(" - \"").append(token).append("\" does not match the key of any known "));
                    return LoadDataError::LOAD_ERR_BADKEY;
                }

                output.push_back(map->at(abrvLong));
                line.erase(0, pos + delim.length());
            }

            return LoadDataError::LOAD_OK;
        }
    }

    LoadDataError LoadBattlerStats(BattlerStatMap &output, std::string location, std::string delim, std::string filename) {
        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadBattlerStats";
        MessageFormats messages(std::string(funcName).append(": ").append(filename), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        LoadDataError ret = Load::LoadFromFile_Standard<BattlerStat_shptr>(output, messages, delim, fileLocName, [](std::string name, ABRV abrv) {
            return BattlerStat_shptr(new BattlerStat(name, abrv));
        });

        std::cout << (ret == LoadDataError::LOAD_OK ? messages.success() : messages.failure()) << "\n";

        return ret;
    }

    LoadDataError LoadDamageInclinations(DamageInclinationMap &output, std::string location, std::string delim, std::string filename) {
        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadDamageInclinations";
        MessageFormats messages(std::string(funcName).append(": ").append(filename), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        LoadDataError ret = Load::LoadFromFile_Standard<DamageInclination_shptr>(output, messages, delim, fileLocName, [](std::string name, ABRV abrv) {
            return DamageInclination_shptr(new DamageInclination(name, abrv));
        }, &DamageInclination::AUTO_KEY);

        std::cout << (ret == LoadDataError::LOAD_OK ? messages.success() : messages.failure()) << "\n";

        return ret;
    }

    LoadDataError LoadDamageTypes(DamageTypeMap &output, std::string location, std::string delim, std::string filename) {
        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadDamageTypes";
        MessageFormats messages(std::string(funcName).append(": ").append(filename), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        LoadDataError ret = Load::LoadFromFile_Standard<DamageType_shptr>(output, messages, delim, fileLocName, [](std::string name, ABRV abrv) {
            return DamageType_shptr(new DamageType(name, abrv));
        });

        std::cout << (ret == LoadDataError::LOAD_OK ? messages.success() : messages.failure()) << "\n";

        return ret;
    }

    LoadDataError LoadEquipmentTypes(EquipmentTypeMap &output, std::string location, std::string delim, std::string filename) {
        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadEquipmentTypes";
        MessageFormats messages(std::string(funcName).append(": ").append(filename), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        LoadDataError ret = Load::LoadFromFile_Standard<EquipmentType_shptr>(output, messages, delim, fileLocName, [](std::string name, ABRV abrv) {
            return EquipmentType_shptr(new EquipmentType(name, abrv));
        });

        std::cout << (ret == LoadDataError::LOAD_OK ? messages.success() : messages.failure()) << "\n";

        return ret;
    }

    LoadDataError LoadSkillElements(SkillElementMap &output, std::string location, std::string delim, std::string filename) {
        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadSkillElements";
        MessageFormats messages(std::string(funcName).append(": ").append(filename), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        LoadDataError ret = Load::LoadFromFile_Standard<SkillElement_shptr>(output, messages, delim, fileLocName, [](std::string name, ABRV abrv) {
            return SkillElement_shptr(new SkillElement(name, abrv));
        });

        std::cout << (ret == LoadDataError::LOAD_OK ? messages.success() : messages.failure()) << "\n";

        return ret;
    }

    LoadDataError LoadElementGroups(SkillElementGroupMap& output, SkillElementMap* elementMap, std::string location, std::string delim, std::string filename) {
        if (elementMap == nullptr || elementMap->size() < 1) {
            std::cout << "LoadElementGroups was given an empty element map. Elements must be loaded before element groups.\n";
            return LoadDataError::LOAD_ERR_BADFORMAT;
        }

        LoadDataError ret = LoadDataError::LOAD_OK;

        std::string fileLocName = location.append("/").append(filename);
        std::string funcName = "LoadElementGroups";
        MessageFormats messages(std::string(funcName).append(": ").append(filename).append(" - load complete."), std::string(funcName).append(" FAILED!: File (").append(fileLocName).append(")"), funcName);

        std::ifstream file;
        file.open(fileLocName, std::ios::in);

        if (!file.is_open()) {
            messages.failure(messages.failure().append(" does not exist.\n"));
            std::cout << messages.failure();
            return LoadDataError::LOAD_ERR_NOFILE;
        }

        size_t pos;
        std::string line;
        std::string token;
        ABRV_long abrvLong = 0;
        SkillElementGroup_shptr current;
        std::unique_ptr<SkillElementList> elements;
        std::unique_ptr<SkillElementGroupList> groups;
        bool noDelim = false;
        bool noEndfile = true;
        bool isGroups = false;
        while (std::getline(file, line)) {
            line = string_trim(line);

            if (line.length() < 1) {
                continue;
            }

            noDelim = ((pos = line.find(delim)) == std::string::npos);

            if (noDelim) {
                token = line;
            } else {
                token = line.substr(0, pos);
            }

            if (token.length() == ABRV::SIZE) {
                if (isGroups) {
                    if (!groups) {
                        messages.failure(messages.failure().append(" - Attempted to insert ").append(token).append(" into a null vector.\n"));
                        file.close();
                        ret = LoadDataError::LOAD_ERR_BADFORMAT;
                        break;
                    }

                    ret = Load::ParseAbrvLine_CheckEndfile(*groups, messages, &output, noEndfile, line, delim);

                    if (ret != LoadDataError::LOAD_OK) {
                        if (ret == LoadDataError::LOAD_ERR_BADKEY) {
                            messages.failure(messages.failure().append("Skill Element Group.\n"));
                        }

                        file.close();
                        break;
                    }
                } else {
                    if (!elements) {
                        messages.failure(messages.failure().append(" - Attempted to insert ").append(token).append(" into a null vector.\n"));
                        file.close();
                        ret = LoadDataError::LOAD_ERR_BADFORMAT;
                        break;
                    }

                    ret = Load::ParseAbrvLine_CheckEndfile(*elements, messages, elementMap, noEndfile, line, delim);

                    if (ret != LoadDataError::LOAD_OK) {
                        if (ret == LoadDataError::LOAD_ERR_BADKEY) {
                            messages.failure(messages.failure().append("Skill Element.\n"));
                        }

                        file.close();
                        break;
                    }
                }
            }

            if (!noEndfile || token.length() != ABRV::SIZE) {
                if (noEndfile && token.length() != 1) {
                    if (noDelim) {
                        messages.failure(messages.failure().append(" Line \"").append(line).append("\" has no delimiter (").append(delim).append(").\n"));
                        file.close();
                        ret = LoadDataError::LOAD_ERR_BADFORMAT;
                        break;
                    } else {
                        messages.failure(messages.failure().append(" - \"").append(token).append("\" is not a flag indicating whether this is a group of groups. (Valid values: [ 0, f, F, n, N ] for false or [ 1, t, T, y, Y ] for true)\n"));
                        file.close();
                        ret = LoadDataError::LOAD_ERR_BADFORMAT;
                        break;
                    }
                }

                // At this point, it is guaranteed that we are starting a new group. Therefore, now is the moment we finalize the current group.
                if (current) {
                    if (isGroups && groups && groups->size() >= 2) {
                        current->Initialize(*groups);
                        std::cout << messages.other() << ": Loaded \"" << current->name() << "\".\n";
                    } else if (!isGroups && elements && elements->size() >= 2) {
                        current->Initialize(*elements);
                        std::cout << messages.other() << ": Loaded \"" << current->name() << "\".\n";
                    } else {
                        messages.failure(messages.failure().append(" - Skill Element Group \"").append(current->name()).append("\" was given less than two members. Each group must have at least two members.\n"));
                        file.close();
                        ret = LoadDataError::LOAD_ERR_BADFORMAT;
                        break;
                    }
                }

                if (!noEndfile) {
                    file.close();
                    break;
                }

                if (LoadCharTruthy(isGroups, token[0]) != LoadDataError::LOAD_OK) {
                    messages.failure(messages.failure().append(" - \"").append(token).append("\" is not a flag indicating whether this is a group of groups. (Valid values: [ 0, f, F, n, N ] for false or [ 1, t, T, y, Y ] for true)\n"));
                    file.close();
                    ret = LoadDataError::LOAD_ERR_BADFORMAT;
                    break;
                }

                line.erase(0, pos + delim.length());

                if (line.length() < 1) {
                    messages.failure(messages.failure().append(" - A groups flag was provided without a key or name.\n"));
                    file.close();
                    ret = LoadDataError::LOAD_ERR_BADFORMAT;
                    break;
                }

                ret = Load::CreateFromLine_IgnoreEndfile<SkillElementGroup_shptr>(output, messages, abrvLong, line, delim, false, [](std::string _name, ABRV _abrv) {
                    return SkillElementGroup_shptr(new SkillElementGroup(_name, _abrv));
                });

                if (ret != LoadDataError::LOAD_OK) {
                    break;
                }

                current = output.at(abrvLong);

                if (isGroups) {
                    elements.reset(nullptr);
                    groups.reset(new SkillElementGroupList());
                } else {
                    elements.reset(new SkillElementList());
                    groups.reset(nullptr);
                }
            }
        }

        if (ret == LoadDataError::LOAD_OK && noEndfile) {
            messages.failure(messages.failure().append(" does not have the end-file indicator \"").append(LOAD_FILE_END_ABRV.AsString()).append("\"."));
            file.close();
            return LoadDataError::LOAD_ERR_NOEND;
        } else {
            if (ret == LoadDataError::LOAD_OK) {
                std::cout << messages.success() << "\n";
                messages.isSuccess(true);
            } else {
                std::cout << messages.failure() << "\n";
            }
        }

        return ret;
    }

    LoadDataError LoadCharTruthy(bool& output, char val) {
        switch (val) {
        case '1':
        case 't':
        case 'T':
        case 'y':
        case 'Y':
            output = true;
            break;

        case '0':
        case 'f':
        case 'F':
        case 'n':
        case 'N':
            output = false;
            break;

        default:
            return LoadDataError::LOAD_ERR_BADFORMAT;
        }

        return LoadDataError::LOAD_OK;
    }
}
