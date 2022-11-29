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

TEST(ite, terminalCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    EXPECT_EQ(0, table.ite(0, 3, 0));
    EXPECT_EQ(1, table.ite(1, 1, 3));
}

TEST(ite, OR_Case)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    EXPECT_EQ(table.Tabel.at(table.ite(idA,table.True(),idB)).Low, idB);
    EXPECT_EQ(table.Tabel.at(table.ite(idA,table.True(),idB)).High, 1);
}

TEST(ite, AND_Case) {
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID x = table.ite(idA, idB, 0);
    EXPECT_EQ(table.Tabel.at(x).Low, 0);
    EXPECT_EQ(table.Tabel.at(x).High, idB);
}

TEST(neg, terminalCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    EXPECT_EQ(table.neg(table.True()), 0);
    EXPECT_EQ(table.neg(table.False()), 1);
}

TEST(neg, simpleCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("aNot");
    table.Tabel.at(idB).High = 0;
    table.Tabel.at(idB).Low = 1;
    EXPECT_EQ(table.neg(idA), idB);
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

    EXPECT_EQ(manager.topVar(var_a), var_a);
    EXPECT_EQ(manager.topVar(var_b), var_b);
    EXPECT_EQ(manager.topVar(var_c), var_c);
    EXPECT_EQ(manager.topVar(var_d), var_d);

    EXPECT_EQ(manager.topVar(function), var_a);

    auto a_or_b = manager.or2(var_a, var_b);
    auto c_and_d = manager.and2(var_c, var_d);
    EXPECT_EQ(manager.topVar(a_or_b), var_a);
    EXPECT_EQ(manager.topVar(c_and_d), var_c);
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

TEST_F(ManagerFixture, TopVarNameTest)
{
    EXPECT_EQ(manager.getTopVarName(var_a), "a");
    EXPECT_EQ(manager.getTopVarName(var_b), "b");
    EXPECT_EQ(manager.getTopVarName(var_c), "c");
    EXPECT_EQ(manager.getTopVarName(var_d), "d");

    EXPECT_EQ(manager.getTopVarName(function), "a");
    
    auto a_or_b = manager.or2(var_a, var_b);
    auto c_and_d = manager.and2(var_c, var_d);
    EXPECT_EQ(manager.getTopVarName(a_or_b), "a");
    EXPECT_EQ(manager.getTopVarName(c_and_d), "c");
}

TEST_F(ManagerFixture, OrTest)
{ 
    auto a_or_b = manager.or2(var_a, var_b);
    auto A_or_B = manager.Tabel.at(a_or_b);

    std::cout << "Label: " << A_or_B.Label << std::endl;
    std::cout << "High: " << A_or_B.High << std::endl;
    std::cout << "Low: " << A_or_B.Low << std::endl;
    std::cout << "TopVa: " << A_or_B.TopVar << std::endl;

    EXPECT_EQ( A_or_B.Label , "(a + b)");
    EXPECT_EQ( A_or_B.High, 1);
    EXPECT_EQ( A_or_B.Low, 3);
    EXPECT_EQ( A_or_B.TopVar, 2);

}

TEST_F(ManagerFixture, AndTest)
{ 
    auto a_or_b = manager.and2(var_a, var_b);
    auto A_or_B = manager.Tabel.at(a_or_b);

    EXPECT_EQ( A_or_B.Label , "(a * b)");
    EXPECT_EQ( A_or_B.High, 3);
    EXPECT_EQ( A_or_B.Low, 0);
    EXPECT_EQ( A_or_B.TopVar, 2);

}

TEST_F(ManagerFixture, CoFacTrueTest)
{ 

    EXPECT_EQ( manager.coFactorTrue(var_a, var_a), 1);
    EXPECT_EQ( manager.coFactorTrue(var_a), 1);
    EXPECT_EQ( manager.coFactorTrue( 1, var_a), 1);
    EXPECT_EQ( manager.coFactorTrue(var_b, var_a), var_b);

    EXPECT_EQ( manager.coFactorTrue(var_c, var_c), 1);
    EXPECT_EQ( manager.coFactorTrue(var_c), 1);
    EXPECT_EQ( manager.coFactorTrue(var_d, var_c), var_d);
    EXPECT_EQ( manager.coFactorTrue( 0, var_c), 0);

    auto a_or_b = manager.or2(var_a, var_b);
    EXPECT_EQ(manager.coFactorTrue(a_or_b), var_a);
    EXPECT_EQ(manager.coFactorTrue(a_or_b, var_b), var_b);
    EXPECT_EQ(manager.coFactorTrue(a_or_b, var_c), var_c);



    auto a_and_b = manager.and2(var_a, var_b);

    EXPECT_EQ(manager.coFactorFalse(a_and_b), manager.True() );
    EXPECT_EQ(manager.coFactorFalse(a_and_b, var_b), var_a);

}




















#endif
