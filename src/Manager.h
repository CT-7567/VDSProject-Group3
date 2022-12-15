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

public:
    struct Node
    {

        BDD_ID High;
        BDD_ID Low;
        BDD_ID TopVar;
        std::string Label;
    };

    std::unordered_map<BDD_ID, Node> Table;

    struct ComputedTableEntry
    {
        BDD_ID f;
        BDD_ID g;
        BDD_ID h;

        bool operator==(const ComputedTableEntry &other) const
        {
            return (f == other.f && g == other.g && h == other.h);
        }
    };

    struct ComputedTableEntryHasher
    {
        std::size_t operator()(const ComputedTableEntry &k) const
        {
            using std::hash;
            using std::size_t;

            return ((hash<BDD_ID>()(k.f) ^ (hash<BDD_ID>()(k.g) << 1)) >> 1) ^ (hash<BDD_ID>()(k.h) << 1);
        }
    };

    std::unordered_map<ComputedTableEntry, BDD_ID, ComputedTableEntryHasher> ComputedTable;

    struct SubGraphTableEntry
    {
        BDD_ID topVar;
        BDD_ID low;
        BDD_ID high;

        bool operator==(const SubGraphTableEntry &other) const
        {
            return (topVar == other.topVar && low == other.low && high == other.high);
        }
    };

    struct SubGraphTableEntryHasher
    {
        std::size_t operator()(const SubGraphTableEntry &k) const
        {
            using std::hash;
            using std::size_t;

            return ((hash<BDD_ID>()(k.topVar) ^ (hash<BDD_ID>()(k.low) << 1)) >> 1) ^ (hash<BDD_ID>()(k.high) << 1);
        }
    };

    std::unordered_map<SubGraphTableEntry, BDD_ID, SubGraphTableEntryHasher> SubGraphTable;

    Manager()
    {
        Table.insert({0, Node{0, 0, 0, "False"}});
        Table.insert({1, Node{1, 1, 1, "True"}});
    }

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
    void printTable();
};

} // namespace ClassProject

#endif
