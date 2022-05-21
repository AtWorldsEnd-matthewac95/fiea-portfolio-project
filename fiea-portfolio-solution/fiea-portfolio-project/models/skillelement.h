#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"

namespace AWE {
    /// <summary>
    /// Represents one of the possible skill elements.
    /// </summary>
    class SkillElement : public AbbreviatedKey {
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        SkillElement();
        SkillElement(std::string name, ABRV abrv);

        /// <summary>
        /// Any SkillElement which is equivalent to this object is invalid.
        /// </summary>
        static const SkillElement INVALID;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other SkillElement object.</param>
        /// <returns>Equivalence to the other SkillElement.</returns>
        bool Equals(const SkillElement& other) const;
    };

    /// <summary>
    /// Used to index SkillElement objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long SkillElementKey;
    /// <summary>
    /// Represents one of the possible skill elements.
    /// </summary>
    typedef std::shared_ptr<SkillElement> SkillElement_shptr;
    typedef std::unordered_map<ABRV_long, SkillElement_shptr> SkillElementMap;
}
