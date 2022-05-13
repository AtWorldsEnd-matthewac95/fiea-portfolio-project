#include "abrv.h"

namespace AWE {
    FourLetterAbbreviation::FourLetterAbbreviation() : _first(0), _second(0), _third(0), _fourth(0), _long(0) {}

    FourLetterAbbreviation::FourLetterAbbreviation(char first, char second, char third, char fourth)
            : _first(std::move(first)), _second(std::move(second)), _third(std::move(third)), _fourth(std::move(fourth)) {
        _long = 0;
        _long += (unsigned long)fourth;
        _long <<= 8;
        _long += (unsigned long)third;
        _long <<= 8;
        _long += (unsigned long)second;
        _long <<= 8;
        _long += (unsigned long)first;
    }

    char FourLetterAbbreviation::operator[](int index) const {
        switch (index) {
        case 0:
            return _first;
            break;
        case 1:
            return _second;
            break;
        case 2:
            return _third;
            break;
        case 3:
            return _fourth;
            break;
        default:
            return 0;
            break;
        }
    }

    const FourLetterAbbreviation FourLetterAbbreviation::INVALID = FourLetterAbbreviation();

    char FourLetterAbbreviation::first() const { return _first; }
    char FourLetterAbbreviation::second() const { return _second; }
    char FourLetterAbbreviation::third() const { return _third; }
    char FourLetterAbbreviation::fourth() const { return _fourth; }

    std::string FourLetterAbbreviation::AsString() const {
        return std::string(1, _first).append(std::string(1, _second)).append(std::string(1, _third)).append(std::string(1, _fourth));
    }

    ABRV_long FourLetterAbbreviation::AsLong() const { return _long; }

    bool FourLetterAbbreviation::Equals(FourLetterAbbreviation other) const {
        return _long == other._long;
    }
}
