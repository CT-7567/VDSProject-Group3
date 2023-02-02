#ifndef VDSPROJECT_REACHABILITY_H
#define VDSPROJECT_REACHABILITY_H

#include "ManagerInterface.h"
#include "ReachabilityInterface.h"

#include <iostream>
#include <string>

namespace ClassProject {

class Reachability : public ReachabilityInterface
{
public:
    explicit Reachability(unsigned int stateSize);

    bool isReachable(const std::vector<bool> &stateVector) override;

    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override;
    void setInitState(const std::vector<bool> &stateVector) override;
    
    const std::vector<BDD_ID> &getStates() const override;

private:
    BDD_ID calculateTransitonRelation();
    BDD_ID calculateImage(BDD_ID cR, BDD_ID transitionRelation);

    std::vector<BDD_ID> stateVars;
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID initialState;
};

} // namespace ClassProject
#endif
