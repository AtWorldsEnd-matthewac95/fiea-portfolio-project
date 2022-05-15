#include "skillelementgroup.h"

namespace AWE {
    SkillElementGroup::SkillElementGroup() : AbbreviatedKey(), _isGroups(false) {}

    SkillElementGroup::SkillElementGroup(std::string name, ABRV abrv) : AbbreviatedKey(name, abrv), _isGroups(false) {}

    SkillElementGroup::SkillElementGroup(std::string name, ABRV abrv, SkillElementList elements)
        : AbbreviatedKey(name, abrv), _isGroups(false), _elements(std::move(elements)) {}

    SkillElementGroup::SkillElementGroup(std::string name, ABRV abrv, SkillElementGroupList groups)
            : AbbreviatedKey(name, abrv), _isGroups(true), _groups(std::move(std::make_unique<SkillElementGroupList>(groups))) {
        SkillElementList elements;
        bool keepElements = true;

        for (SkillElementGroup_shptr group : groups) {
            if (!group->IsValid()) {
                keepElements = false;
                break;
            }
            for (SkillElement_shptr ele : group->_elements) {
                elements.push_back(ele);
            }
        }

        if (keepElements) {
            _elements = std::move(elements);
        }
    }

    const SkillElementGroup SkillElementGroup::INVALID = SkillElementGroup();

    bool SkillElementGroup::isGroups() const { return _isGroups; }

    bool SkillElementGroup::IsValid() const { return !_elements.empty(); }

    bool SkillElementGroup::Initialize() {
        if (IsValid()) {
            return true;
        }
        if (!_isGroups || !_groups) {
            return false;
        }

        SkillElementList elements;

        for (SkillElementGroup_shptr group : *_groups) {
            if (!group->IsValid()) {
                return false;
            }
            for (SkillElement_shptr ele : group->_elements) {
                elements.push_back(ele);
            }
        }

        _elements = std::move(elements);
        return true;
    }

    bool SkillElementGroup::Initialize(SkillElementList elements) {
        if (IsValid()) {
            return false;
        }

        _elements = std::move(elements);
        _isGroups = false;

        return true;
    }

    bool SkillElementGroup::Initialize(SkillElementGroupList groups) {
        if (IsValid()) {
            return false;
        }

        _groups = std::move(std::make_unique<SkillElementGroupList>(groups));
        _isGroups = true;

        SkillElementList elements;

        for (const SkillElementGroup_shptr& group : groups) {
            if (!group->IsValid()) {
                return false;
            }
            for (const SkillElement_shptr& ele : group->_elements) {
                elements.push_back(ele);
            }
        }

        _elements = std::move(elements);
        return true;
    }

    const SkillElementList& SkillElementGroup::elements() const { return _elements; }
    const SkillElementGroupList& SkillElementGroup::groups() const { return IsValid() && _isGroups ? *_groups : SkillElementGroupList(); }

    bool SkillElementGroup::Equals(const AbbreviatedKey& other) const {
        bool isEqual = false;

        const SkillElementGroup* converted = dynamic_cast<const SkillElementGroup*>(&other);
        if (converted) {
            isEqual = Equals(*converted);
        }

        return isEqual;
    }

    bool SkillElementGroup::Equals(const SkillElementGroup& other) const {
        // Determine whether one or both group is invalid.
        if (!IsValid()) {
            if (!other.IsValid()) {
                return true;
            }
            return false;
        } else if (!other.IsValid()) {
            return false;
        }

        // Determine if these two groups are the same kind of group.
        if (_isGroups != other._isGroups) {
            return false;
        }

        // By this point, we know both groups are valid and are of the same type.
        if (_isGroups) {
            if (_groups->size() != other._groups->size()) {
                return false;
            }
            size_t gsize = _groups->size();
            for (int g = 0; g < gsize; g++) {
                if (!(*_groups)[g]->Equals(*(*other._groups)[g])) {
                    return false;
                }
            }
        } else {
            if (_elements.size() != other._elements.size()) {
                return false;
            }
            size_t esize = _elements.size();
            for (int e = 0; e < esize; e++) {
                if (!_elements[e]->Equals(*(other._elements[e]))) {
                    return false;
                }
            }
        }

        return _abrv.Equals(other._abrv);
    }

    // iterator

    SkillElementGroup::const_iterator::const_iterator(const SkillElementList& list) : _itr(std::begin(list)) {}
    SkillElementGroup::const_iterator::const_iterator(const SkillElementList::const_iterator& itr) : _itr(itr) {}

    SkillElementGroup::const_iterator::reference SkillElementGroup::const_iterator::operator*() const { return _itr.operator*(); }
    SkillElementGroup::const_iterator::pointer SkillElementGroup::const_iterator::operator->() { return _itr.operator->(); }
    SkillElementGroup::const_iterator& SkillElementGroup::const_iterator::operator++() { _itr++; return *this; }
    SkillElementGroup::const_iterator SkillElementGroup::const_iterator::operator++(int) { const_iterator prev = *this; this->operator++(); return prev; }

    SkillElementGroup::const_iterator SkillElementGroup::begin() const { return const_iterator(_elements); }
    SkillElementGroup::const_iterator SkillElementGroup::end() const { return const_iterator(std::end(_elements)); }
}
