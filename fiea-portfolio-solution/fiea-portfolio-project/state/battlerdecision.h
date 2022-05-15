#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../models/battler.h"
#include "../models/skill.h"

namespace AWE {
    /// <summary>
    /// Represents a battler's decision in combat. `source` is using `skill` on `target`.
    /// </summary>
    class BattlerDecision {
    private:
        Skill_shptr _skill;
        BattlerInstance_shptr _source;
        BattlerInstance_shptr _target;

    public:
        BattlerDecision(Skill_shptr, BattlerInstance_shptr source, BattlerInstance_shptr target);

        const Skill_shptr& skill() const;
        const BattlerInstance_shptr& source() const;
        const BattlerInstance_shptr& target() const;
    };

    typedef std::vector<BattlerDecision> BattlerDecisionList;
    /// <summary>
    /// Map which orders battler decisions based on priority. Battlers with a lower priority go earlier in the turn.
    /// Some battlers may share priority values, so a first-come-first-served list is used to resolve those conflicts.
    /// </summary>
    typedef std::map<unsigned short, BattlerDecisionList> BattlerDecisionOrdering;

    /// <summary>
    /// Searches for a battler decision in the list with the lowest priority value. If a priority value is found with an empty list, that element is removed from the ordering and the search continues.
    /// </summary>
    /// <param name="">The ordering to search. Note this is not const, so the ordering may be modified.</param>
    /// <returns>true if a list with at least one element was found in the ordering, false otherwise.</returns>
    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering*);
    /// <summary>
    /// Searches for a battler decision in the list with the lowest priority value. If a priority value is found with an empty list, that element is removed from the ordering and the search continues.
    /// </summary>
    /// <param name="BattlerDecisionOrdering*">Pointer to the ordering to search. Note this is not const, so the ordering may be modified.</param>
    /// <param name="output">If a battler decision is found, this ref will contain that object.</param>
    /// <returns>true if a list with at least one element was found in the ordering, false otherwise.</returns>
    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering*, BattlerDecisionOrdering::const_iterator& output);

    /// <summary>
    /// Attempts to find a decision in the given ordering whose source is equivalent to the given battler instance.
    /// </summary>
    /// <param name="BattlerDecisionOrdering">The ordering to search.</param>
    /// <param name="BattlerInstance_shptr">const reference to the battler instance to search for.</param>
    /// <param name="foundOrderingOutput">true if an element in the ordering map corresponding to the battler's priority was found, false otherwise.</param>
    /// <param name="foundListOutput">true if the battler was found in the respective list, false otherwise. If foundOrderingOutput is false, this will necessarily also be false.</param>
    void FindBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&, bool& foundOrderingOutput, bool& foundListOutput);
    /// <summary>
    /// Attempts to find a decision in the given ordering whose source is equivalent to the given battler instance.
    /// </summary>
    /// <param name="BattlerDecisionOrdering">reference to the ordering to search. The reference is non-const so that the returned iterators can also be non-const.</param>
    /// <param name="BattlerInstance_shptr">const reference to the battler instance to search for.</param>
    /// <param name="foundOrderingOutput">true if an element in the ordering map corresponding to the battler's priority was found, false otherwise.</param>
    /// <param name="orderingOutput">if foundOrderingOutput is true, this ref will contain the iterator to the map element.</param>
    /// <param name="foundListOutput">true if the battler was found in the respective list, false otherwise. If foundOrderingOutput is false, this will necessarily also be false.</param>
    /// <param name="listOutput">if foundListOutput is true, this ref will contain the iterator to the list element.</param>
    void FindBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&, bool& foundOrderingOutput, BattlerDecisionOrdering::iterator& orderingOutput, bool& foundListOutput, BattlerDecisionList::iterator& listOutput);

    /// <summary>
    /// Attempts to remove from the given ordering the first decision found with the given battler instance as a source.
    /// </summary>
    /// <returns>unique_ptr to removed decision.</returns>
    std::unique_ptr<BattlerDecision> RemoveBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&);
}
