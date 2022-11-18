// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"
#include <vector>
#include <unordered_map>

namespace ClassProject {

class Manager:public ManagerInterface{

    static constexpr BDD_ID FALSE_ID = 0;
    static constexpr BDD_ID TRUE_ID = 1;

    public:

    struct Node { 

        BDD_ID High;
        BDD_ID Low;
        BDD_ID TopVar;
        std::string Label;
    };


    std::unordered_map <BDD_ID, Node> Tabel;

    Manager(){
        Tabel.insert( { 0, Node{ 0, 0, 0, "False" } } );

        Tabel.insert( { 1, Node{ 1, 1, 1, "True" } } );
    }

    virtual BDD_ID createVar(const std::string &label);

    virtual const BDD_ID &True();

    virtual const BDD_ID &False();

    virtual bool isConstant(BDD_ID f);

    virtual bool isVariable(BDD_ID x);

    virtual BDD_ID topVar(BDD_ID f);

    virtual BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e);

    virtual BDD_ID coFactorTrue(BDD_ID f, BDD_ID x);

    virtual BDD_ID coFactorFalse(BDD_ID f, BDD_ID x);

    virtual BDD_ID coFactorTrue(BDD_ID f);

    virtual BDD_ID coFactorFalse(BDD_ID f);

    virtual BDD_ID neg(BDD_ID a);

    virtual BDD_ID and2(BDD_ID a, BDD_ID b);

    virtual BDD_ID or2(BDD_ID a, BDD_ID b);

    virtual BDD_ID xor2(BDD_ID a, BDD_ID b);

    virtual BDD_ID nand2(BDD_ID a, BDD_ID b);

    virtual BDD_ID nor2(BDD_ID a, BDD_ID b);

    virtual BDD_ID xnor2(BDD_ID a, BDD_ID b);

    virtual std::string getTopVarName(const BDD_ID &root);

    virtual void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root);

    virtual void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root);

    virtual size_t uniqueTableSize();
};

} // namespace ClassProject

#endif
