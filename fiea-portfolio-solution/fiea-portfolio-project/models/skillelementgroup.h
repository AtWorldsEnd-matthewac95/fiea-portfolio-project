#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../abrv/abbreviatedkey.h"
#include "../abrv/abrv.h"
#include "skillelement.h"

namespace AWE {
    class SkillElementGroup;

    /// <summary>
    /// Represents a group of skill elements. Can also represent a group of groups - in this case, this group's elements will be a union of all of its child groups.
    /// Note this object actually has a list which contains all of its elements.
    /// </summary>
    typedef std::shared_ptr<SkillElementGroup> SkillElementGroup_shptr;
    typedef std::vector<SkillElement_shptr> SkillElementList;
    typedef std::vector<SkillElementGroup_shptr> SkillElementGroupList;

    /// <summary>
    /// Represents a group of skill elements. Can also represent a group of groups - in this case, this group's elements will be a union of all of its child groups.
    /// Note this object actually has a list which contains all of its elements.
    /// </summary>
    class SkillElementGroup : public AbbreviatedKey {
    private:
        bool _isGroups;
        SkillElementList _elements;
        std::unique_ptr<SkillElementGroupList> _groups;

    public:
        /// <summary>
        /// Default constructor. Initializes the object with an empty name and an invalid ABRV.
        /// </summary>
        SkillElementGroup();
        /// <summary>
        /// Constructor. Initialize must be used on this object to give it elements.
        /// </summary>
        SkillElementGroup(std::string name, ABRV abrv);
        /// <summary>
        /// Constructor. Initializes the group using the given element list. IsValid can be used to confirm the initialization was successful.
        /// </summary>
        SkillElementGroup(std::string name, ABRV abrv, SkillElementList elements);
        /// <summary>
        /// Constructor. Initializes the group using the given group list. IsValid can be used to confirm the initialization was successful.
        /// </summary>
        SkillElementGroup(std::string name, ABRV abrv, SkillElementGroupList groups);

        /// <summary>
        /// Any SkillElementGroup which is equivalent to this object is invalid.
        /// </summary>
        static const SkillElementGroup INVALID;

        /// <returns>Is this a group of groups?</returns>
        bool isGroups() const;
        /// <returns>const reference to internal skill element list of this group.</returns>
        const SkillElementList& elements() const;
        /// <returns>If this is a group of groups, const pointer to internal group list. Otherwise, returns nullptr.</returns>
        const SkillElementGroupList* groups() const;

        /// <returns>Was this group properly initialized?</returns>
        bool IsValid() const;
        /// <summary>
        /// Initializes the group using the internal group list. This overload of this method is only useable on groups of groups.
        /// </summary>
        /// <returns>True if initialization was successful, false otherwise.</returns>
        bool Initialize();
        /// <summary>
        /// Initializes the group using the given element list.
        /// </summary>
        /// <returns>True if initialization was successful, false otherwise.</returns>
        bool Initialize(SkillElementList elements);
        /// <summary>
        /// Initializes the group using the given group list.
        /// </summary>
        /// <returns>True if initialization was successful, false otherwise.</returns>
        bool Initialize(SkillElementGroupList groups);
        /// <param name="other">The other AbbreviatedKey object.</param>
        /// <returns>Equivalence to the other AbbreviatedKey.</returns>
        bool Equals(const AbbreviatedKey& other) const override;
        /// <param name="other">The other SkillElementGroup object.</param>
        /// <returns>Equivalence to the other SkillElementGroup.</returns>
        bool Equals(const SkillElementGroup& other) const;

        class const_iterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = SkillElementList::value_type;
            using difference_type = SkillElementList::difference_type;
            using pointer = SkillElementList::const_pointer;
            using reference = SkillElementList::const_reference;

            const_iterator(const SkillElementList&);
            const_iterator(const SkillElementList::const_iterator&);

            reference operator*() const;
            pointer operator->();
            const_iterator& operator++();
            const_iterator operator++(int);

            friend bool operator==(const const_iterator& it1, const const_iterator& it2) { return it1._itr == it2._itr; }
            friend bool operator!=(const const_iterator& it1, const const_iterator& it2) { return it1._itr != it2._itr; }

        private:
            SkillElementList::const_iterator _itr;
        };

        /// <returns>const iterator to the first element in this group.</returns>
        const_iterator begin() const;
        /// <returns>const iterator to the spot in memory behind the last element in this group. Attempting access from this address will result in undefined behavior.</returns>
        const_iterator end() const;
    };

    /// <summary>
    /// Used to index SkillElementGroup objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long SkillElementGroupKey;
    typedef std::unordered_map<SkillElementGroupKey, SkillElementGroup_shptr> SkillElementGroupMap;
}
