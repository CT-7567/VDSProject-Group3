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
    explicit Reachability(unsigned int stateSize) : ReachabilityInterface(stateSize)
    {
        for (std::size_t i = 0; i < 2; i++) {
            for (std::size_t j = 0; j < stateSize; j++) {
                std::string label = i == 0 ? "s" : "s'";
                label += std::to_string(j);

                BDD_ID var = createVar(label);
                stateVars.push_back(var);
            }
        }
    }

    const std::vector<BDD_ID> &getStates() const override
    {
        return stateVars;
    }

    bool isReachable(const std::vector<bool> &stateVector) override
    {
        BDD_ID transitionRelation = calculateTransitonRelation();
    }

    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override
    {
        this->transitionFunctions = transitionFunctions;
    }

    void setInitState(const std::vector<bool> &stateVector) override
    {
        BDD_ID characteristicFunction = True();

        for (std::size_t i = 0; i < stateVector.size(); i++) {
            BDD_ID clause = xnor2(stateVars[i], stateVector[i]);
            characteristicFunction = and2(clause, characteristicFunction);
        }

        initialState = characteristicFunction;
    }

private:
    BDD_ID calculateTransitonRelation()
    {
        unsigned stateSize = stateVars.size();
        BDD_ID transitionRelation = True();
        
        for (std::size_t i = stateSize / 2; i < stateVars.size(); i++) {
            BDD_ID clause = xnor2(stateVars[i], transitionFunctions[i]);
            transitionRelation = and2(clause, transitionRelation);
        }

        return transitionRelation;
    }

    std::vector<BDD_ID> stateVars;
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID initialState;
};

} // namespace ClassProject
#endif
