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
        SkillElementGroup();
        SkillElementGroup(std::string name, ABRV abrv);
        SkillElementGroup(std::string name, ABRV abrv, SkillElementList elements);
        SkillElementGroup(std::string name, ABRV abrv, SkillElementGroupList groups);

        static const SkillElementGroup INVALID;

        bool isGroups() const;
        const SkillElementList& elements() const;
        const SkillElementGroupList& groups() const;

        bool IsValid() const;
        bool Initialize();
        bool Initialize(SkillElementList elements);
        bool Initialize(SkillElementGroupList groups);
        bool Equals(const AbbreviatedKey& other) const override;
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

        const_iterator begin() const;
        const_iterator end() const;
    };

    /// <summary>
    /// Used to index SkillElementGroup objects into a map. Currently is designed to be an ABRV_long, which should correspond to this object's ABRV.
    /// </summary>
    typedef ABRV_long SkillElementGroupKey;
    typedef std::unordered_map<SkillElementGroupKey, SkillElementGroup_shptr> SkillElementGroupMap;
}
