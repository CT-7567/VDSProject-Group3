// A minimalistic BDD library, following Wolfgang Kunz lecture slides
//
// Created by Markus Wedler 2014

#ifndef VDSPROJECT_MANAGER_H
#define VDSPROJECT_MANAGER_H

#include "ManagerInterface.h"

#include <unordered_map>
#include <vector>

namespace ClassProject {

class Manager : public ManagerInterface
{
    static constexpr BDD_ID FALSE_ID = 0;
    static constexpr BDD_ID TRUE_ID = 1;

    struct Node
    {

        BDD_ID High;
        BDD_ID Low;
        BDD_ID TopVar;
        std::string Label;
    };

    struct BDDTriple
    {
        BDD_ID f;
        BDD_ID g;
        BDD_ID h;

        bool operator==(const BDDTriple &other) const
        {
            return (f == other.f && g == other.g && h == other.h);
        }
    };

    struct BDDTripleHasher
    {
        std::size_t operator()(const BDDTriple &k) const
        {
            using std::hash;
            return ((hash<BDD_ID>()(k.f) ^ (hash<BDD_ID>()(k.g) << 1)) >> 1) ^ (hash<BDD_ID>()(k.h) << 1);
        }
    };

    std::vector<Node> Table;
    std::unordered_map<BDDTriple, BDD_ID, BDDTripleHasher> ComputedTable;
    std::unordered_map<BDDTriple, BDD_ID, BDDTripleHasher> SubGraphTable;

public:
    Manager();

    BDD_ID createVar(const std::string &label) override;
    const BDD_ID &True() override;
    const BDD_ID &False() override;
    bool isConstant(BDD_ID f) override;
    bool isVariable(BDD_ID x) override;
    BDD_ID topVar(BDD_ID f) override;
    BDD_ID ite(BDD_ID i, BDD_ID t, BDD_ID e) override;
    BDD_ID coFactorTrue(BDD_ID f, BDD_ID x) override;
    BDD_ID coFactorFalse(BDD_ID f, BDD_ID x) override;
    BDD_ID coFactorTrue(BDD_ID f) override;
    BDD_ID coFactorFalse(BDD_ID f) override;
    BDD_ID neg(BDD_ID a) override;
    BDD_ID and2(BDD_ID a, BDD_ID b) override;
    BDD_ID or2(BDD_ID a, BDD_ID b) override;
    BDD_ID xor2(BDD_ID a, BDD_ID b) override;
    BDD_ID nand2(BDD_ID a, BDD_ID b) override;
    BDD_ID nor2(BDD_ID a, BDD_ID b) override;
    BDD_ID xnor2(BDD_ID a, BDD_ID b) override;
    std::string getTopVarName(const BDD_ID &root) override;
    void findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root) override;
    void findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root) override;
    size_t uniqueTableSize() override;

    void printTable() const;
    void printTruthTable(BDD_ID f);
};

} // namespace ClassProject

#endif
