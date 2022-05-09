#pragma once
#include <string>
#include "abrv.h"

namespace AWE {
    class AbbreviatedKey {
    protected:
        std::string _name;
        ABRV _abrv;
    public:
        AbbreviatedKey();
        AbbreviatedKey(std::string name, ABRV abrv);

        const std::string& name() const;
        std::string abrvstr() const;
        ABRV_long abrvlong() const;

        virtual bool Equals(const AbbreviatedKey& other) const;
    };
}
