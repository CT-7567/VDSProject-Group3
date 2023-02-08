#include "Reachability.h"
#include "ManagerInterface.h"

#include <stdexcept>
#include <string>

ClassProject::Reachability::Reachability(unsigned int stateSize) : ReachabilityInterface(stateSize)
{
    if (stateSize == 0) {
        throw std::runtime_error("stateSize should not be zero");
    }

    for (std::size_t i = 0; i < stateSize; i++) {
        std::string label{"s" + std::to_string(i)};
        BDD_ID var = createVar(label);
        stateVars.push_back(var);
    }

    for (std::size_t i = 0; i < stateSize; i++) {
        std::string label{"s'" + std::to_string(i)};
        BDD_ID var = createVar(label);
        nextStateVars.push_back(var);
    }

    // Default initial state
    {
        BDD_ID characteristicFunction = True();
        for (auto stateVar : stateVars) {
            BDD_ID clause = xnor2(stateVar, False());
            characteristicFunction = and2(clause, characteristicFunction);
        }

        initialState = characteristicFunction;
    }

    // Default transition function
    for (auto stateVar : stateVars) {
        transitionFunctions.push_back(stateVar);
    }
}

const std::vector<ClassProject::BDD_ID> &ClassProject::Reachability::getStates() const
{
    return stateVars;
}

bool ClassProject::Reachability::isReachable(const std::vector<bool> &stateVector)
{
    if (stateVector.size() != stateVars.size()) {
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
    for (std::size_t i = 0; i < stateVars.size(); i++) {
        if (stateVector[i]) {
            temp = coFactorTrue(temp, stateVars[i]);
        } else {
            temp = coFactorFalse(temp, stateVars[i]);
        }
    }

    return temp == True();
}

void ClassProject::Reachability::setTransitionFunctions(const std::vector<BDD_ID> &transitionFunctions)
{
    if (transitionFunctions.size() != stateVars.size()) {
        throw std::runtime_error("transitionFunctions does not match state bits");
    }

    for (auto currentId : transitionFunctions) {
        if (currentId > uniqueTableSize() - 1) {
            throw std::runtime_error("Entry in transitionFunctions is not a valid node");
        }
    }

    this->transitionFunctions = transitionFunctions;
}

void ClassProject::Reachability::setInitState(const std::vector<bool> &stateVector)
{
    if (stateVector.size() != stateVars.size()) {
        throw std::runtime_error("stateVector does not match state bits");
    }

    BDD_ID characteristicFunction = True();

    for (std::size_t i = 0; i < stateVector.size(); i++) {
        BDD_ID clause = xnor2(stateVars[i], stateVector[i] ? True() : False());
        characteristicFunction = and2(clause, characteristicFunction);
    }

    initialState = characteristicFunction;
}

ClassProject::BDD_ID ClassProject::Reachability::calculateTransitonRelation()
{
    BDD_ID transitionRelation = True();

    for (std::size_t i = 0; i < nextStateVars.size(); i++) {
        BDD_ID clause = xnor2(nextStateVars[i], transitionFunctions[i]);
        transitionRelation = and2(clause, transitionRelation);
    }

    return transitionRelation;
}

ClassProject::BDD_ID ClassProject::Reachability::calculateImage(BDD_ID cR, BDD_ID transitionRelation)
{
    BDD_ID temp = and2(cR, transitionRelation);

    for (auto stateVar : stateVars) {
        temp = or2(coFactorTrue(temp, stateVar), coFactorFalse(temp, stateVar));
    }

    BDD_ID rename = True();
    for (std::size_t i = 0; i < stateVars.size(); i++) {
        BDD_ID currentState = stateVars[i];
        BDD_ID nextState = nextStateVars[i];
        rename = and2(rename, xnor2(currentState, nextState));
    }

    temp = and2(temp, rename);

    for (auto nextStateVar : nextStateVars) {
        temp = or2(coFactorTrue(temp, nextStateVar), coFactorFalse(temp, nextStateVar));
    }

    return temp;
}
