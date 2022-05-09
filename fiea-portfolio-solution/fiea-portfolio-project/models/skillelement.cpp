#include "skillelement.h"

namespace AWE {
    SkillElement::SkillElement() : AbbreviatedKey() {}
    SkillElement::SkillElement(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv) {}

    const SkillElement SkillElement::INVALID = SkillElement();

    bool SkillElement::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const SkillElement* converted = dynamic_cast<const SkillElement*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool SkillElement::Equals(const SkillElement& other) const {
        return _abrv.Equals(other._abrv);
    }
}
