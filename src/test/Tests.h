//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include <gtest/gtest.h>
#include "../Manager.h"

class ManagerFixture : public ::testing::Test {
protected:
    ManagerFixture() {
        // Build up the example from the document:
        var_a = manager.createVar("a");
        var_b = manager.createVar("b");
        var_c = manager.createVar("c");
        var_d = manager.createVar("d");

        auto a_or_b = manager.or2(var_a, var_b);
        auto c_and_d = manager.and2(var_c, var_d);

        function = manager.and2(a_or_b, c_and_d);
    }

    ClassProject::Manager manager;

    ClassProject::BDD_ID var_a;
    ClassProject::BDD_ID var_b;
    ClassProject::BDD_ID var_c;
    ClassProject::BDD_ID var_d;
    ClassProject::BDD_ID function;
};

TEST(createVarTest, IDTeset)
{
    ClassProject::Manager table = ClassProject::Manager();
    const std::string test =  "a";
    ClassProject::BDD_ID test_ID = table.createVar(test);
    EXPECT_EQ(test_ID, 2);
    const std::string test2 = "b";
    ClassProject::BDD_ID test_ID2 = table.createVar(test2);
    EXPECT_EQ(test_ID2, 3);

}

TEST(isConstantTest, leafTest)
{
    ClassProject::Manager table = ClassProject::Manager();
    bool test1 = table.isConstant(0);
    bool test2 = table.isConstant(1);
    EXPECT_EQ(test2, true);
    EXPECT_EQ(test1, true);
    for(int i = 2; i < 10; i++){
        EXPECT_EQ(table.isConstant(i), false);
    }
}
TEST(isConstantTest, noLeafTest)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID id = table.createVar("a");
    bool test1 = table.isConstant(id);
    EXPECT_EQ(test1, false);

}

TEST_F(ManagerFixture, FalseTest)
{
    auto false_id = manager.False();
    EXPECT_EQ(false_id, 0);

    auto false_node = manager.Tabel.at(false_id);
    EXPECT_EQ(false_node.High, 0);
    EXPECT_EQ(false_node.Low, 0);
    EXPECT_EQ(false_node.TopVar, 0);
}

TEST_F(ManagerFixture, TopVarTest)
{
    for (auto const &[node_id, node] : manager.Tabel) {
        EXPECT_EQ(manager.topVar(node_id), node.TopVar);
    }
}

TEST_F(ManagerFixture, CoFactorFalseTest)
{
    EXPECT_EQ(manager.coFactorFalse(function), var_b);
    EXPECT_EQ(manager.coFactorFalse(function, var_a), var_b);

    auto a_and_c = manager.and2(var_a, var_c);
    auto a_and_c_and_d = manager.and2(a_and_c, var_d);
    EXPECT_EQ(manager.coFactorFalse(function, var_b), a_and_c_and_d);

    EXPECT_EQ(manager.coFactorFalse(function, var_c), manager.False());
    EXPECT_EQ(manager.coFactorFalse(function, var_d), manager.False());

    EXPECT_EQ(manager.coFactorFalse(var_a), manager.False());
    EXPECT_EQ(manager.coFactorFalse(var_b), manager.False());

    EXPECT_EQ(manager.coFactorFalse(var_a, var_b), var_a);
    EXPECT_EQ(manager.coFactorFalse(var_b, var_a), var_b);

    EXPECT_EQ(manager.coFactorFalse(a_and_c), manager.False());
    EXPECT_EQ(manager.coFactorFalse(a_and_c, var_b), a_and_c);

    auto a_or_b = manager.or2(var_a, var_b);
    EXPECT_EQ(manager.coFactorFalse(a_or_b), var_b);
    EXPECT_EQ(manager.coFactorFalse(a_or_b, var_b), var_a);
    EXPECT_EQ(manager.coFactorFalse(a_or_b, var_c), a_or_b);

    // Test terminal cases
    EXPECT_EQ(manager.coFactorFalse(function, manager.False()), function);
    EXPECT_EQ(manager.coFactorFalse(var_b, function), function);
    EXPECT_EQ(manager.coFactorFalse(manager.False(), var_a), manager.False());
    EXPECT_EQ(manager.coFactorFalse(manager.True(), var_a), manager.True());
}

TEST_F(ManagerFixture, TrueTest)
{
    auto true_id = manager.True();
    EXPECT_EQ(true_id, 1);

    auto true_node = manager.Tabel.at(true_id);
    EXPECT_EQ(true_node.High, 1);
    EXPECT_EQ(true_node.Low, 1);
    EXPECT_EQ(true_node.TopVar, 1);
}


TEST_F(ManagerFixture, isVarTest)
{
    //Note Leaf test
    auto isVarLeaf = manager.isVariable( manager.True() );
    EXPECT_EQ(isVarLeaf, false);

    //Variable test
    auto isVarVariable = manager.isVariable( var_b );
    EXPECT_EQ(isVarVariable, true);

    //Function test
    auto a_or_b = manager.or2(var_a, var_b);
    auto isVarFunction = manager.isVariable( a_or_b );
    EXPECT_EQ(isVarFunction, false);

}











#endif
