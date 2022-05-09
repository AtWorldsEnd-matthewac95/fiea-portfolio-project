#include "battlerdecision.h"

namespace AWE {
    BattlerDecision::BattlerDecision(Skill_shptr skill, BattlerInstance_shptr source, BattlerInstance_shptr target) : _skill(std::move(skill)), _source(std::move(source)), _target(std::move(target)) {}

    const Skill_shptr& BattlerDecision::skill() const { return _skill; }
    const BattlerInstance_shptr& BattlerDecision::source() const { return _source; }
    const BattlerInstance_shptr& BattlerDecision::target() const { return _target; }

    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering* ordering) { BattlerDecisionOrdering::const_iterator _; return CleanTopBattlerDecisionOrdering(ordering, _); }
    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering* ordering, BattlerDecisionOrdering::const_iterator& output) {
        if (ordering == nullptr) {
            return false;
        }

        do {
            output = ordering->begin();

            if (output == ordering->end()) {
                return false;
            }

            BattlerDecisionList::const_iterator decision;
            if ((decision = output->second.begin()) != output->second.end()) {
                return true;
            }

            ordering->erase(output);
        } while (!ordering->empty());

        output = ordering->end();
        return false;
    }

    void FindBattlerDecision(BattlerDecisionOrdering& ordering, const BattlerInstance_shptr& battler, bool& foundOrderingOutput, bool& foundListOutput) {
        BattlerDecisionOrdering::iterator _;
        BattlerDecisionList::iterator __;
        return FindBattlerDecision(ordering, battler, foundOrderingOutput, _, foundListOutput, __);
    }
    void FindBattlerDecision(
        BattlerDecisionOrdering& ordering,
        const BattlerInstance_shptr& battler,
        bool& foundOrderingOutput,
        BattlerDecisionOrdering::iterator& orderingOutput,
        bool& foundListOutput,
        BattlerDecisionList::iterator& listOutput
    ) {
        foundOrderingOutput = false;
        foundListOutput = false;

        orderingOutput = ordering.find(battler->priority());

        if (orderingOutput != ordering.end()) {
            foundOrderingOutput = true;
        }

        if (foundOrderingOutput) {
            for (BattlerDecisionList::iterator l_itr = orderingOutput->second.begin(); l_itr != orderingOutput->second.end(); l_itr++) {
                if (l_itr->source().get() == battler.get()) {
                    foundListOutput = true;
                    listOutput = l_itr;
                    break;
                }
            }
        }
    }

    std::unique_ptr<BattlerDecision> RemoveBattlerDecision(BattlerDecisionOrdering& ordering, const BattlerInstance_shptr& battler) {
        std::unique_ptr<BattlerDecision> output;
        BattlerDecisionOrdering::iterator orderitr;
        BattlerDecisionList::iterator decision;
        bool foundOrderitr = false, foundDecision = false;

        FindBattlerDecision(ordering, battler, foundOrderitr, orderitr, foundDecision, decision);
        if (foundOrderitr && foundDecision) {
            output = std::make_unique<BattlerDecision>(BattlerDecision(*decision));
            orderitr->second.erase(decision);

            if (orderitr->second.empty()) {
                ordering.erase(orderitr);
            }
        }

        return output;
    }
}
