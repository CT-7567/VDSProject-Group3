#ifndef VDSPROJECT_REACHABILITY_TESTS_H
#define VDSPROJECT_REACHABILITY_TESTS_H

#include <gtest/gtest.h>
#include <stdexcept>
#include "Reachability.h"

using namespace ClassProject;

struct ReachabilityTest : testing::Test {

    std::unique_ptr<ClassProject::ReachabilityInterface> fsm2 = std::make_unique<ClassProject::Reachability>(2);

    std::vector<BDD_ID> stateVars2 = fsm2->getStates();
    std::vector<BDD_ID> transitionFunctions;

};

struct ComplexStateMachine : testing::Test {
    ComplexStateMachine() : fsm(3)
    {
        BDD_ID s0 = stateVars2.at(0);
        BDD_ID s1 = stateVars2.at(1);
        BDD_ID s2 = stateVars2.at(2);

        transitionFunctions.push_back(fsm.False()); // s0' = 0
        transitionFunctions.push_back(fsm.neg(s1)); // s1' = not(s1)
        transitionFunctions.push_back(fsm.False());
        fsm.setTransitionFunctions(transitionFunctions);

        fsm.setInitState({false,false, false});
    }
    
    Reachability fsm;
    std::vector<BDD_ID> stateVars2 = fsm.getStates();
    std::vector<BDD_ID> transitionFunctions;
};

TEST_F(ReachabilityTest, HowTo_Example) { /* NOLINT */

    BDD_ID s0 = stateVars2.at(0);
    BDD_ID s1 = stateVars2.at(1);

    transitionFunctions.push_back(fsm2->neg(s0)); // s0' = not(s0)
    transitionFunctions.push_back(fsm2->neg(s1)); // s1' = not(s1)
    fsm2->setTransitionFunctions(transitionFunctions);

    fsm2->setInitState({false,false});

    ASSERT_TRUE(fsm2->isReachable({false, false}));
    ASSERT_FALSE(fsm2->isReachable({false, true}));
    ASSERT_FALSE(fsm2->isReachable({true, false}));
    ASSERT_TRUE(fsm2->isReachable({true, true}));
}

TEST_F(ReachabilityTest, SetInitialStateThrow) { /* NOLINT */
    // Bits does not match
    EXPECT_THROW(fsm2->setInitState({}), std::runtime_error);
    EXPECT_THROW(fsm2->setInitState({false}), std::runtime_error);
    EXPECT_THROW(fsm2->setInitState({true, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, IsReachableThrow) { /* NOLINT */
    // Bits does not match
    EXPECT_THROW(fsm2->isReachable({}), std::runtime_error);
    EXPECT_THROW(fsm2->isReachable({false}), std::runtime_error);
    EXPECT_THROW(fsm2->isReachable({true, false, false}), std::runtime_error);
}

TEST_F(ReachabilityTest, TransistionFunctionsThrow) { /* NOLINT */
    // Bits does not match
    EXPECT_THROW(fsm2->setTransitionFunctions(transitionFunctions), std::runtime_error);
    
    transitionFunctions.push_back(123);
    transitionFunctions.push_back(324);
    // Unkown IDs
    EXPECT_THROW(fsm2->setTransitionFunctions(transitionFunctions), std::runtime_error);
}

TEST(Reachability, ConstructorThrow) { /* NOLINT */
    EXPECT_THROW(Reachability(0), std::runtime_error);
}

TEST_F(ComplexStateMachine, ReachableStates) { /* NOLINT */
    ASSERT_TRUE(fsm.isReachable({false, false, false}));
    ASSERT_TRUE(fsm.isReachable({false, true, false}));
    ASSERT_FALSE(fsm.isReachable({true, false, false}));
    ASSERT_FALSE(fsm.isReachable({true, true, false}));
    ASSERT_FALSE(fsm.isReachable({false, false, true}));
    ASSERT_FALSE(fsm.isReachable({false, true, true}));
    ASSERT_FALSE(fsm.isReachable({true, false, true}));
    ASSERT_FALSE(fsm.isReachable({true, true, true}));
}

#endif
