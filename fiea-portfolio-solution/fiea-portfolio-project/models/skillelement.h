#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    class SkillElement : public AbbreviatedKey {
    public:
        SkillElement();
        SkillElement(std::string name, ABRV abrv);

        static const SkillElement INVALID;

        bool Equals(const AbbreviatedKey& other) const override;
        bool Equals(const SkillElement& other) const;
    };

    typedef ABRV_long SkillElementKey;
    typedef std::shared_ptr<SkillElement> SkillElement_shptr;
    typedef std::unordered_map<ABRV_long, SkillElement_shptr> SkillElementMap;
}
