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
        if (stateSize == 0) {
            throw std::runtime_error("stateSize should not be zero");
        }

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
        if (stateVector.size() != stateVars.size() / 2) {
            throw std::runtime_error("stateVector does not match state bits");
        }

        BDD_ID transitionRelation = calculateTransitonRelation();

        BDD_ID cR_it = initialState;
        BDD_ID cR;

        do {
            cR = cR_it;
            cR_it = or2(cR, calculateImage(cR, transitionRelation));
        } while (cR != cR_it);

        BDD_ID temp = cR;
        for (std::size_t i = 0; i < stateVars.size() / 2; i++) {
            if (stateVector[i])
            {
                temp = coFactorTrue(temp, stateVars[i]);
            }
            else
            {
                temp = coFactorFalse(temp, stateVars[i]);
            }
        }

        return temp == True();
    }

    void setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions) override
    {
        if (transitionFunctions.size() != stateVars.size() / 2) {
            throw std::runtime_error("transitionFunctions does not match state bits");
        }

        for (auto currentId : transitionFunctions) {
            if (currentId > uniqueTableSize() - 1) {
                throw std::runtime_error("Entry in transitionFunctions is not a valid node");
            }
        }

        this->transitionFunctions = transitionFunctions;
    }

    void setInitState(const std::vector<bool> &stateVector) override
    {
        if (stateVector.size() != stateVars.size() / 2) {
            throw std::runtime_error("stateVector does not match state bits");
        }

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
            BDD_ID clause = xnor2(stateVars[i], transitionFunctions[i - stateSize / 2]);
            transitionRelation = and2(clause, transitionRelation);
        }

        return transitionRelation;
    }

    BDD_ID calculateImage(BDD_ID cR, BDD_ID transitionRelation)
    {
        BDD_ID temp = and2(cR, transitionRelation);

        for (std::size_t i = 0; i < stateVars.size() / 2; i++) {
            temp = or2(coFactorTrue(temp, stateVars[i]), coFactorFalse(temp, stateVars[i]));
        }

        BDD_ID rename = True();
        for (std::size_t i = 0; i < stateVars.size() / 2; i++) {
            BDD_ID currentState = stateVars[i];
            BDD_ID nextState = stateVars[i + stateVars.size() / 2];
            rename = and2(rename, xnor2(currentState, nextState));
        }

        temp = and2(temp, rename);

        for (std::size_t i = stateVars.size() / 2; i < stateVars.size(); i++) {
            temp = or2(coFactorTrue(temp, stateVars[i]), coFactorFalse(temp, stateVars[i]));
        }

        return temp;
    }

    std::vector<BDD_ID> stateVars;
    std::vector<BDD_ID> transitionFunctions;
    BDD_ID initialState;
};

} // namespace ClassProject
#endif
