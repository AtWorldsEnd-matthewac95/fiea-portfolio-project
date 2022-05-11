#pragma once
#include <string>

namespace AWE {
    typedef unsigned long ABRV_long;

    class FourLetterAbbreviation {
    private:
        char _first;
        char _second;
        char _third;
        char _fourth;
        ABRV_long _long;

    public:
        FourLetterAbbreviation();
        FourLetterAbbreviation(char first, char second, char third, char fourth);

        char operator[](int index) const;

        static const FourLetterAbbreviation INVALID;
        static const size_t SIZE = 4;

        char first() const;
        char second() const;
        char third() const;
        char fourth() const;

        std::string AsString() const;
        ABRV_long AsLong() const;
        bool Equals(FourLetterAbbreviation other) const;
    };

    typedef FourLetterAbbreviation ABRV;
    static ABRV_long const INVALID_ABRV_LONG = 0;
}
