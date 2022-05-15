#pragma once
#include <string>

namespace AWE {
    /// <summary>
    /// Every ABRV is made of four `char`s, each having 8 bits. C++ `unsigned long` is a 32-bit number, which conveniently has just enough room to fit all four `char`s in bit form.
    /// </summary>
    typedef unsigned long ABRV_long;

    /// <summary>
    /// See ABRV typedef definition.
    /// </summary>
    class FourLetterAbbreviation {
    private:
        char _first;
        char _second;
        char _third;
        char _fourth;
        ABRV_long _long;

    public:
        /// <summary>
        /// Default constructor. Initializes an invalid ABRV object with an `unsigned long` of 0 and all `char`s as the null character.
        /// </summary>
        FourLetterAbbreviation();
        FourLetterAbbreviation(char first, char second, char third, char fourth);

        /// <param name="index">Should be 0, 1, 2, or 3. Passing any other index will result in a null character.</param>
        /// <returns>The respective `char`, or a null character if a bad index is passed.</returns>
        char operator[](int index) const;

        /// <summary>
        /// Any ABRV which is equivalent to this object is not valid.
        /// </summary>
        static const FourLetterAbbreviation INVALID;

        /// <summary>
        /// 4. Hopefully this is obvious.
        /// </summary>
        static const size_t SIZE = 4;

        /// <returns>The first `char`.</returns>
        char first() const;
        /// <returns>The second `char`.</returns>
        char second() const;
        /// <returns>The third `char`.</returns>
        char third() const;
        /// <returns>The fourth `char`.</returns>
        char fourth() const;

        /// <summary>
        /// Example:
        /// ABRV ex1 = { first = 'A', second = 'B', third = 'C', fourth = 'D' };
        /// ex1.AsString() results in "ABCD".
        /// Not very complicated stuff.
        /// </summary>
        /// <returns>All `char`s concatenated in order.</returns>
        std::string AsString() const;

        /// <summary>
        /// The exact method used to create this number is not that important. What is important is this: Every ABRV created with the same `char`s in the same positions will always have the same ABRV_long.
        /// </summary>
        /// <returns>All four 8-bit `char`s bitshifted into a single 32-bit number.</returns>
        ABRV_long AsLong() const;

        /// <param name="other">The other ABRV object.</param>
        /// <returns>Equivalence to the other ABRV.</returns>
        bool Equals(FourLetterAbbreviation other) const;
    };

    /// <summary>
    /// FourLetterAbbreviation (abbreviated as ABRV) provides a way to give data-driven list-of-values entries a key which is at once
    /// indexable in maps at O(1) speed,
    /// easily constructable on the spot,
    /// while also being human-distinguishable.
    /// Every ABRV object has four `char`s which come together into a string that can be easily distinguished by a human (and hopefully also readable).
    /// Upon creation of the object, the four `char`s will immediately be copied via bitshifting into a 32-bit number that will be used for indexing and comparison purposes.
    /// Since lists of values loaded into the game from a text file cannot be directly converted into a C++ `enum` without forward planning, ABRV provides a mechanism which aims to have similar
    /// indexing, readability, and comparison advantages normally enjoyed by `enum`s.
    /// </summary>
    typedef FourLetterAbbreviation ABRV;

    /// <summary>
    /// The `unsigned long` value of an invalid ABRV.
    /// </summary>
    static ABRV_long const INVALID_ABRV_LONG = 0;
}
