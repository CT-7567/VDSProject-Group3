//
// Created by tobias on 21.11.16.
//

#ifndef VDSPROJECT_TESTS_H
#define VDSPROJECT_TESTS_H

#include "../Manager.h"

#include <gtest/gtest.h>

class ManagerFixture : public ::testing::Test
{
protected:
    ManagerFixture()
    {
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
    const std::string test = "a";
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
    for (int i = 2; i < 10; i++) {
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
    ClassProject::BDD_ID a_or_b = table.ite(idA, 1, idB);
    EXPECT_EQ(table.coFactorTrue(a_or_b), table.True());
    EXPECT_EQ(table.coFactorFalse(a_or_b), idB);

    table.printTable();
}

TEST(ite, AND_Case)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID a_and_b = table.ite(idA, idB, 0);
    EXPECT_EQ(table.coFactorTrue(a_and_b), idB);
    EXPECT_EQ(table.coFactorFalse(a_and_b), table.False());

    table.printTable();
}

TEST(ite, node_exist_already)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID a_and_b = table.ite(idA, idB, 0);
    ClassProject::BDD_ID a_and_b2 = table.ite(idA, idB, 0);
    EXPECT_EQ(a_and_b, a_and_b2);
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
    ClassProject::BDD_ID test = table.neg(idA);
    EXPECT_EQ(table.coFactorFalse(idA), table.coFactorTrue(test));
    EXPECT_EQ(table.coFactorTrue(idA), table.coFactorFalse(test));
    // two times neg should be the original
    EXPECT_EQ(table.neg(test), idA);
    table.printTable();
}
TEST(neg, withFunction) {
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID idC = table.createVar("c");
    ClassProject::BDD_ID idD = table.createVar("d");
    ClassProject::BDD_ID idA_and_B = table.and2(idA, idB);
    ClassProject::BDD_ID idC_and_D = table.and2(idC, idD);
    ClassProject::BDD_ID idF = table.or2(idA_and_B, idC_and_D);
    ClassProject::BDD_ID id_not_F = table.neg(idF);
    /*      a   b   c   d   |f
     * 01:  0   0   0   0   |1
     * --------------------------
     *      a   b   c   d   |f
     * 02:  0   0   0   1   |1
     * ---------------------------
     *      a   b   c   d   |f
     * 03:  0   0   1   0   |1
     * ---------------------------
     *      a   b   c   d   |f
     * 04:  0   0   1   1   |0
     * ---------------------------
     *      a   b   c   d   |f
     * 05:  0   1   0   0   |1
     * ----------------------------
     *      a   b   c   d   |f
     * 06:  0   1   0   1   |1
     * ----------------------------
     *      a   b   c   d   |f
     * 07:  0   1   1   0   |1
     * ----------------------------
     *      a   b   c   d   |f
     * 08:  0   1   1   1   |0
     * ----------------------------
     *      a   b   c   d   |f
     * 09:  1   0   0   0   |1
     * ----------------------------
     *      a   b   c   d   |f
     * 10:  1   0   0   1   |1
     * ----------------------------
     *      a   b   c   d   |f
     * 11:  1   0   1   0   |1
     *  ----------------------------
     *      a   b   c   d   |f
     * 12:  1   0   1   1   |0
     *  ----------------------------
     *      a   b   c   d   |f
     * 13:  1   1   0   0   |0
     *  ----------------------------
     *      a   b   c   d   |f
     * 14:  1   1   0   1   |0
     *  ----------------------------
     *      a   b   c   d   |f
     * 15:  1   1   1   0   |0
     *  ----------------------------
     *      a   b   c   d   |f
     * 16:  1   1   1   1   |0
     */
    EXPECT_EQ(table.coFactorTrue(table.coFactorTrue(table.coFactorTrue(table.coFactorTrue(id_not_F)))), table.False());
    EXPECT_EQ(table.coFactorTrue(table.coFactorTrue(table.coFactorTrue(table.coFactorFalse(id_not_F)))), table.False());
    EXPECT_EQ(table.coFactorTrue(table.coFactorTrue(table.coFactorFalse(table.coFactorTrue(id_not_F)))), table.False());
    EXPECT_EQ(table.coFactorTrue(table.coFactorFalse(table.coFactorTrue(table.coFactorFalse(id_not_F)))), table.True());
    EXPECT_EQ(table.coFactorTrue(table.coFactorFalse(table.coFactorFalse(table.coFactorTrue(id_not_F)))), table.True());
    EXPECT_EQ(table.coFactorFalse(table.coFactorFalse(table.coFactorFalse(table.coFactorFalse(id_not_F)))), table.True());
}



TEST_F(ManagerFixture, FalseTest)
{
    auto false_id = manager.False();
    EXPECT_EQ(false_id, 0);
}

TEST_F(ManagerFixture, TopVarTest)
{
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
    auto b_and_c = manager.and2(var_b, var_c);
    auto b_and_c_and_d = manager.and2(b_and_c, var_d);

    EXPECT_EQ(manager.coFactorFalse(function), b_and_c_and_d);
    EXPECT_EQ(manager.coFactorFalse(function, var_a), b_and_c_and_d);

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
    EXPECT_EQ(manager.coFactorFalse(manager.False(), var_a), manager.False());
    EXPECT_EQ(manager.coFactorFalse(manager.True(), var_a), manager.True());
}

TEST_F(ManagerFixture, TrueTest)
{
    auto true_id = manager.True();
    EXPECT_EQ(true_id, 1);

}

TEST_F(ManagerFixture, isVarTest)
{
    // Note Leaf test
    auto isVarLeaf = manager.isVariable(manager.True());
    EXPECT_EQ(isVarLeaf, false);

    // Variable test
    auto isVarVariable = manager.isVariable(var_b);
    EXPECT_EQ(isVarVariable, true);

    // Function test
    auto a_or_b = manager.or2(var_a, var_b);
    auto isVarFunction = manager.isVariable(a_or_b);
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
    
    auto A_or_B = manager.ite(var_a, manager.True(), var_b);
    
    EXPECT_EQ(A_or_B, a_or_b);
    

    manager.printTable();
}

TEST_F(ManagerFixture, AndTest)
{
    auto a_and_b = manager.and2(var_a, var_b);

    auto A_and_B = manager.ite(var_a, var_b, manager.False() );

    EXPECT_EQ(A_and_B, a_and_b);


    manager.printTable();
}

TEST_F(ManagerFixture, CoFacTrueTest)
{
    EXPECT_EQ(manager.coFactorTrue(var_a, var_a), manager.True());
    EXPECT_EQ(manager.coFactorTrue(var_a), manager.True());
    EXPECT_EQ(manager.coFactorTrue(manager.True(), var_a), manager.True());
    EXPECT_EQ(manager.coFactorTrue(var_b, var_a), var_b);

    EXPECT_EQ(manager.coFactorTrue(var_c, var_c), manager.True());
    EXPECT_EQ(manager.coFactorTrue(var_c), manager.True());
    EXPECT_EQ(manager.coFactorTrue(var_d, var_c), var_d);
    EXPECT_EQ(manager.coFactorTrue(manager.False(), var_c), manager.False());

    auto a_or_b = manager.or2(var_a, var_b);
    EXPECT_EQ(manager.coFactorTrue(a_or_b), manager.True());
    EXPECT_EQ(manager.coFactorTrue(a_or_b, var_b), manager.True());
    // EXPECT_EQ(manager.coFactorTrue(a_or_b, var_c), a_or_b);

    auto a_and_b = manager.and2(var_a, var_b);

    EXPECT_EQ(manager.coFactorTrue(a_and_b), var_b);
    EXPECT_EQ(manager.coFactorTrue(a_and_b, var_b), var_a);
    // EXPECT_EQ(manager.coFactorTrue(a_and_b, var_c), a_and_b);

    manager.printTable();
}

TEST_F(ManagerFixture, XorTest)
{
    auto a_xor_b = manager.xor2(var_a, var_b);
    
    auto A_xor_B = manager.ite(var_a, manager.neg(var_b), var_b);

    EXPECT_EQ(A_xor_B, a_xor_b);
}

TEST_F(ManagerFixture, NandTest)
{
    auto a_nand_b = manager.nand2(var_a, var_b);
    
    auto A_nand_B = manager.ite(var_a, manager.neg(var_b), manager.True() );

    EXPECT_EQ(A_nand_B, a_nand_b);
}

TEST_F(ManagerFixture, NorTest)
{
    auto a_nor_b = manager.nor2(var_a, var_b);
    
    auto A_nor_B = manager.ite(var_a, manager.False(), manager.neg(var_b) );

    EXPECT_EQ(A_nor_B, a_nor_b);
}

TEST_F(ManagerFixture, XnorTest)
{
    auto a_xnor_b = manager.xnor2(var_a, var_b);
    
    auto A_xnor_B = manager.ite(var_a, var_b, manager.neg(var_b) );

    EXPECT_EQ(A_xnor_B, a_xnor_b);
}

TEST_F(ManagerFixture, findNodesTest)
{
    std::set<ClassProject::BDD_ID> allNodesOf_a_and_b, allNodesOfF;

    auto a_and_b = manager.and2(var_a, var_b);

    manager.findNodes(a_and_b, allNodesOf_a_and_b);

    EXPECT_EQ(allNodesOf_a_and_b.size(), 4);
    EXPECT_EQ(allNodesOf_a_and_b.count(a_and_b), 1);
    EXPECT_EQ(allNodesOf_a_and_b.count(var_a), 0);
    EXPECT_EQ(allNodesOf_a_and_b.count(var_b), 1);
    EXPECT_EQ(allNodesOf_a_and_b.count(manager.True()), 1);
    EXPECT_EQ(allNodesOf_a_and_b.count(manager.False()), 1);

    manager.findNodes(function, allNodesOfF);

    EXPECT_EQ(allNodesOfF.size(), 6);
    EXPECT_EQ(allNodesOfF.count(9), 1);
    EXPECT_EQ(allNodesOfF.count(8), 1);
    EXPECT_EQ(allNodesOfF.count(7), 1);
    EXPECT_EQ(allNodesOfF.count(var_a), 0);
    EXPECT_EQ(allNodesOfF.count(manager.True()), 1);

    for (auto item : allNodesOfF)
        std::cout << item << std::endl;
    manager.printTable();
}

TEST_F(ManagerFixture, findVarsTest)
{
    std::set<ClassProject::BDD_ID> allVarsOf_a_and_b, allVarsOfF;

    auto a_and_b = manager.and2(var_a, var_b);

    manager.findVars(a_and_b, allVarsOf_a_and_b);

    EXPECT_EQ(allVarsOf_a_and_b.size(), 2);
    EXPECT_EQ(allVarsOf_a_and_b.count(a_and_b), 0);
    EXPECT_EQ(allVarsOf_a_and_b.count(var_a), 1);
    EXPECT_EQ(allVarsOf_a_and_b.count(var_b), 1);
    EXPECT_EQ(allVarsOf_a_and_b.count(manager.True()), 0);
    EXPECT_EQ(allVarsOf_a_and_b.count(manager.False()), 0);


    manager.findVars(function, allVarsOfF);

    EXPECT_EQ(allVarsOfF.size(), 4);
    EXPECT_EQ(allVarsOfF.count(9), 0);
    EXPECT_EQ(allVarsOfF.count(8), 0);
    EXPECT_EQ(allVarsOfF.count(7), 0);
    EXPECT_EQ(allVarsOfF.count(var_a), 1);
    EXPECT_EQ(allVarsOfF.count(var_b), 1);
    EXPECT_EQ(allVarsOfF.count(var_c), 1);
    EXPECT_EQ(allVarsOfF.count(var_d), 1);
    EXPECT_EQ(allVarsOfF.count(manager.True()), 0);


    manager.printTable();
}

TEST_F(ManagerFixture, UniqueTableSizeTest)
{
    // Should be 10 as seen on page 6 of the project document.
    const std::size_t START_TABLE_SIZE = 10;
    std::size_t table_size = START_TABLE_SIZE;
    EXPECT_EQ(manager.uniqueTableSize(), table_size);

    // Add some more variables:
    manager.createVar("x1");
    manager.createVar("x2");
    manager.createVar("x3");
    table_size += 3;

    EXPECT_EQ(manager.uniqueTableSize(), table_size);
}




TEST(DrawNegGraphs, simpleCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    
    auto var_A = table.createVar("a");
    auto var_B = table.createVar("b");
    auto var_C = table.createVar("c");
    auto var_D = table.createVar("d");


    auto all_XOR = table.and2( table.and2( var_A, var_B ), table.and2( var_C, var_D ) );

    auto all_NXOR = table.neg( table.and2( table.and2( var_A, var_B ), table.and2( var_C, var_D ) ) );



    table.printTable();


    EXPECT_EQ(1, 0);
}



TEST(DrawNegGraphs2, simpleCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    
    auto var_A = table.createVar("a");
    auto var_B = table.createVar("b");
    auto var_C = table.createVar("c");
    auto var_D = table.createVar("d");


    auto all_XOR = table.or2( table.or2( var_A, var_B ), table.or2( var_C, var_D ) );

    auto all_NXOR = table.neg( table.or2( table.or2( var_A, var_B ), table.or2( var_C, var_D ) ) );



    table.printTable();


    EXPECT_EQ(1, 0);
}



#endif
