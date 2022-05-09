#pragma once
#include <map>
#include <memory>
#include <vector>
#include "../models/battler.h"
#include "../models/skill.h"

namespace AWE {
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
    typedef std::map<unsigned short, BattlerDecisionList> BattlerDecisionOrdering;

    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering*);
    bool CleanTopBattlerDecisionOrdering(BattlerDecisionOrdering*, BattlerDecisionOrdering::const_iterator& output);

    void FindBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&, bool& foundOrderingOutput, bool& foundListOutput);
    void FindBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&, bool& foundOrderingOutput, BattlerDecisionOrdering::iterator& orderingOutput, bool& foundListOutput, BattlerDecisionList::iterator& listOutput);

    std::unique_ptr<BattlerDecision> RemoveBattlerDecision(BattlerDecisionOrdering&, const BattlerInstance_shptr&);
}
