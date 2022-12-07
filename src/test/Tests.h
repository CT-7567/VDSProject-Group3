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

    table.printTable();

}

TEST(ite, AND_Case) {
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID x = table.ite(idA, idB, 0);
    EXPECT_EQ(table.Tabel.at(x).Low, 0);
    EXPECT_EQ(table.Tabel.at(x).High, idB);


    table.printTable();

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
    EXPECT_EQ(table.Tabel.at(test).Low, 1);
    EXPECT_EQ(table.Tabel.at(test).High, 0);
    //two times neg should be the original
    EXPECT_EQ(table.neg(test), idA);
}

TEST(neg, complexCase)
{
    ClassProject::Manager table = ClassProject::Manager();
    ClassProject::BDD_ID idA = table.createVar("a");
    ClassProject::BDD_ID idB = table.createVar("b");
    ClassProject::BDD_ID idAn = table.createVar("an");
    table.Tabel.at(idAn).Low = 1;
    table.Tabel.at(idAn).High = 0;
    ClassProject::BDD_ID idBn = table.createVar("bn");
    table.Tabel.at(idBn).Low = 1;
    table.Tabel.at(idBn).High = 0;
    ClassProject::BDD_ID or_id = table.ite(idA, 1, idB);
    ClassProject::BDD_ID or_not_id = table.neg(or_id);
    std::cout<<"ID or not : "<<  or_not_id << std::endl;
    EXPECT_EQ(table.Tabel.at(or_not_id).High, 0);
    ClassProject::BDD_ID idB_not = table.Tabel.at(or_not_id).Low;
    std::cout<<"ID b not : "<<  idB_not << std::endl;
    EXPECT_EQ(table.Tabel.at(or_not_id).Low, idB_not);
    EXPECT_EQ(table.Tabel.at(idB_not).Low, 1);
    EXPECT_EQ(table.Tabel.at(idB_not).High, 0);
    table.printTable();
}

TEST(neg, withFunction){
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
    //13,14,15,16
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(id_not_F).High).High, 0);
    //1,2,5,6
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(id_not_F).Low).Low, 1);
    //4,8
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(table.Tabel.at(id_not_F).Low).High).High, 0);
    //3, 7
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(table.Tabel.at(id_not_F).Low).High).Low, 1);
    //9, 10
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(table.Tabel.at(id_not_F).High).Low).Low, 1);
    //11
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(table.Tabel.at(table.Tabel.at(id_not_F).High).Low).High).Low, 1);
    //12
    EXPECT_EQ(table.Tabel.at(table.Tabel.at(table.Tabel.at(table.Tabel.at(id_not_F).High).Low).High).High, 0);
    table.printTable();
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

    EXPECT_EQ( A_or_B.Label , "(a+b)");
    EXPECT_EQ( A_or_B.High, manager.True() );
    EXPECT_EQ( A_or_B.Low, var_b);
    EXPECT_EQ( A_or_B.TopVar, var_a);

    manager.printTable();

}

TEST_F(ManagerFixture, AndTest)
{ 
    auto a_and_b = manager.and2(var_a, var_b);
    auto A_and_B = manager.Tabel.at(a_and_b);

    EXPECT_EQ( A_and_B.Label , "(a*b)");
    EXPECT_EQ( A_and_B.High, var_b);
    EXPECT_EQ( A_and_B.Low, manager.False());
    EXPECT_EQ( A_and_B.TopVar, var_a);

    manager.printTable();

}

TEST_F(ManagerFixture, CoFacTrueTest)
{ 

    EXPECT_EQ( manager.coFactorTrue(var_a, var_a), manager.True() );
    EXPECT_EQ( manager.coFactorTrue(var_a), manager.True());
    EXPECT_EQ( manager.coFactorTrue( manager.True(), var_a), manager.True());
    EXPECT_EQ( manager.coFactorTrue(var_b, var_a), var_b);

    EXPECT_EQ( manager.coFactorTrue(var_c, var_c), manager.True());
    EXPECT_EQ( manager.coFactorTrue(var_c), manager.True());
    EXPECT_EQ( manager.coFactorTrue(var_d, var_c), var_d);
    EXPECT_EQ( manager.coFactorTrue( manager.False(), var_c), manager.False());

    auto a_or_b = manager.or2(var_a, var_b);
    EXPECT_EQ(manager.coFactorTrue(a_or_b), manager.True() );
    EXPECT_EQ(manager.coFactorTrue(a_or_b, var_b), manager.True());
    //EXPECT_EQ(manager.coFactorTrue(a_or_b, var_c), a_or_b);



    auto a_and_b = manager.and2(var_a, var_b);

    EXPECT_EQ(manager.coFactorTrue(a_and_b), var_b );
    EXPECT_EQ(manager.coFactorTrue(a_and_b, var_b), var_a);
    //EXPECT_EQ(manager.coFactorTrue(a_and_b, var_c), a_and_b);

    manager.printTable();


}

TEST_F(ManagerFixture, XorTest)
{ 
    auto a_xor_b = manager.xor2(var_a, var_b);
    auto A_xor_B = manager.Tabel.at(a_xor_b);

    EXPECT_EQ( A_xor_B.Label , "(a⊕b)");

    EXPECT_EQ( A_xor_B.Low, var_b);
    EXPECT_EQ( A_xor_B.TopVar, var_a);

    auto bnot = A_xor_B.High;
    auto Bnot = manager.Tabel.at(bnot);

    EXPECT_EQ( Bnot.High, manager.False() );   
    EXPECT_EQ( Bnot.Low, manager.True() );
    EXPECT_EQ( Bnot.TopVar, var_b);

    manager.printTable();

}

TEST_F(ManagerFixture, NandTest)
{ 
    auto a_nand_b = manager.nand2(var_a, var_b);
    auto A_nand_B = manager.Tabel.at(a_nand_b);

    EXPECT_EQ( A_nand_B.Low, manager.True());
    EXPECT_EQ( A_nand_B.TopVar, var_a);

    auto bnot = A_nand_B.High;
    auto Bnot = manager.Tabel.at(bnot);

    EXPECT_EQ( Bnot.High, manager.False() );   
    EXPECT_EQ( Bnot.Low, manager.True() );
    EXPECT_EQ( Bnot.TopVar, var_b);

    manager.printTable();
}

TEST_F(ManagerFixture, NorTest)
{ 
    auto a_nor_b = manager.nor2(var_a, var_b);
    auto A_nor_B = manager.Tabel.at(a_nor_b);

    EXPECT_EQ( A_nor_B.High, manager.False());
    EXPECT_EQ( A_nor_B.TopVar, var_a);

    auto bnot = A_nor_B.Low;
    auto Bnot = manager.Tabel.at(bnot);

    EXPECT_EQ( Bnot.High, manager.False() );   
    EXPECT_EQ( Bnot.Low, manager.True() );
    EXPECT_EQ( Bnot.TopVar, var_b);

    manager.printTable();  
}

TEST_F(ManagerFixture, XnorTest)
{ 
    auto a_xnor_b = manager.xnor2(var_a, var_b);
    auto A_xnor_B = manager.Tabel.at(a_xnor_b);

    EXPECT_EQ( A_xnor_B.High, var_b);
    EXPECT_EQ( A_xnor_B.TopVar, var_a);

    auto bnot = A_xnor_B.Low;
    auto Bnot = manager.Tabel.at(bnot);

    EXPECT_EQ( Bnot.High, manager.False() );   
    EXPECT_EQ( Bnot.Low, manager.True() );
    EXPECT_EQ( Bnot.TopVar, var_b);

    auto c_and_d = manager.and2( var_d, var_c );

    auto a_xnor_c_and_d = manager.xnor2(var_a, c_and_d);
    auto A_xnor_c_and_d = manager.Tabel.at(a_xnor_c_and_d);

    auto c_and_dT = A_xnor_c_and_d.High;
    auto C_and_D = manager.Tabel.at(c_and_d);

    auto c_and_dT_not = A_xnor_c_and_d.Low;
    auto C_and_D_not = manager.Tabel.at(c_and_dT_not);

    EXPECT_EQ( c_and_d, c_and_dT );

    EXPECT_EQ( C_and_D_not.TopVar, var_c );
    EXPECT_EQ( C_and_D_not.High, var_d);
    EXPECT_EQ( C_and_D_not.Low, manager.True() );


    auto c_nand_d = manager.nand2( var_d, var_c );
    auto C_nand_D = manager.Tabel.at(c_nand_d);

    EXPECT_EQ( C_and_D_not.TopVar, C_nand_D.TopVar );
    EXPECT_EQ( C_and_D_not.Low, C_nand_D.Low );

/*
    auto notd = C_nand_D.High;
    auto notD = manager.Tabel.at(notd);

    EXPECT_EQ( C_and_D_not.High, C_nand_D.High );
    EXPECT_EQ( C_and_D_not.TopVar, C_nand_D.TopVar );
    EXPECT_EQ( C_and_D_not.Low, C_nand_D.Low );
*/

    manager.printTable();
}

TEST_F(ManagerFixture, findNodesTest)
{ 

    std::set<ClassProject::BDD_ID> allNodesOf_a_and_b, allNodesOfF;

    auto a_and_b = manager.and2(var_a, var_b);

    manager.findNodes(a_and_b, allNodesOf_a_and_b);


    EXPECT_EQ( allNodesOf_a_and_b.size(), 1);
    EXPECT_EQ( allNodesOf_a_and_b.count( a_and_b ), 1);
    EXPECT_EQ( allNodesOf_a_and_b.count( var_a ), 0);
    EXPECT_EQ( allNodesOf_a_and_b.count( var_b ), 0);
    EXPECT_EQ( allNodesOf_a_and_b.count( manager.True() ), 0);
    EXPECT_EQ( allNodesOf_a_and_b.count( manager.False() ), 0);




    manager.findNodes(function, allNodesOfF);    


    EXPECT_EQ( allNodesOfF.size(), 3);
    EXPECT_EQ( allNodesOfF.count( 9 ), 1);
    EXPECT_EQ( allNodesOfF.count( 8 ), 1);
    EXPECT_EQ( allNodesOfF.count( 7 ), 1);
    EXPECT_EQ( allNodesOfF.count( var_a ), 0);
    EXPECT_EQ( allNodesOfF.count( manager.True() ), 0);




    for(auto item : allNodesOfF )
        std::cout<< item << std::endl;
    manager.printTable();

}


TEST_F(ManagerFixture, findVarsTest)
{ 

    std::set<ClassProject::BDD_ID> allVarsOf_a_and_b, allVarsOfF;

    auto a_and_b = manager.and2(var_a, var_b);

    manager.findVars(a_and_b, allVarsOf_a_and_b);

    EXPECT_EQ( allVarsOf_a_and_b.size(), 2);
    EXPECT_EQ( allVarsOf_a_and_b.count( a_and_b ), 0);
    EXPECT_EQ( allVarsOf_a_and_b.count( var_a ), 1);
    EXPECT_EQ( allVarsOf_a_and_b.count( var_b ), 1);
    EXPECT_EQ( allVarsOf_a_and_b.count( manager.True() ), 0);
    EXPECT_EQ( allVarsOf_a_and_b.count( manager.False() ), 0);

    std::cout<<"Var of a*b: "<<std::endl;
    for(auto item : allVarsOf_a_and_b )
        std::cout<< "ID: "<<item<<"     | Label: " <<manager.Tabel.at(item).Label<<"\n\n" << std::endl;


    manager.findVars(function, allVarsOfF);    

    EXPECT_EQ( allVarsOfF.size(), 4);
    EXPECT_EQ( allVarsOfF.count( 9 ), 0);
    EXPECT_EQ( allVarsOfF.count( 8 ), 0);
    EXPECT_EQ( allVarsOfF.count( 7 ), 0);
    EXPECT_EQ( allVarsOfF.count( var_a ), 1);
    EXPECT_EQ( allVarsOfF.count( var_b ), 1);
    EXPECT_EQ( allVarsOfF.count( var_c ), 1);
    EXPECT_EQ( allVarsOfF.count( var_d ), 1);
    EXPECT_EQ( allVarsOfF.count( manager.True() ), 0);

    std::cout<<"Var of fkt: "<<std::endl;
    for(auto item : allVarsOfF )
        std::cout<< "ID: "<<item<<"     | Label: " <<manager.Tabel.at(item).Label<<"\n\n" << std::endl;
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

#endif
