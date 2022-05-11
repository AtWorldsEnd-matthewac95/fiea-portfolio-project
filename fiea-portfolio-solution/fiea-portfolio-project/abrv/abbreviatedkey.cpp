#include "abbreviatedkey.h"

namespace AWE {
    AbbreviatedKey::AbbreviatedKey() : _name(""), _abrv(ABRV::INVALID) {}
    AbbreviatedKey::AbbreviatedKey(std::string name, ABRV abrv)
        : _name(std::move(name)), _abrv(std::move(abrv)) {}

    const std::string& AbbreviatedKey::name() const { return _name; }
    std::string AbbreviatedKey::abrvstr() const { return _abrv.AsString(); }
    unsigned long AbbreviatedKey::abrvlong() const { return _abrv.AsLong(); }

    bool AbbreviatedKey::Equals(const AbbreviatedKey& other) const { return _abrv.Equals(other._abrv); }
}
