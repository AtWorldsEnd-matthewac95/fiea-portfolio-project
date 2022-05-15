#pragma once
#include <string>
#include "abrv.h"

namespace AWE {
    /// <summary>
    /// Superclass representing an object which has both a "long" name and a four-letter abbreviation.
    /// </summary>
    class AbbreviatedKey {
    protected:
        std::string _name;
        ABRV _abrv;
    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        AbbreviatedKey();
        AbbreviatedKey(std::string name, ABRV abrv);

        const std::string& name() const;

        /// <returns>The internal ABRV object in string form.</returns>
        std::string abrvstr() const;
        /// <returns>The internal ABRV object in `unsigned long` form.</returns>
        ABRV_long abrvlong() const;

        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        virtual bool Equals(const AbbreviatedKey& other) const;
    };
}
