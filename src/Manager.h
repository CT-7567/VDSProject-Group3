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
private:
    static constexpr BDD_ID FALSE_ID = 0;
    static constexpr BDD_ID TRUE_ID = 1;

    struct LowEdge
    {
        bool complemented;
        BDD_ID node;

        bool operator==(const LowEdge &other) const
        {
            return (other.complemented == complemented && other.node == node);
        }
    };

    struct LowEdgeHasher
    {
        std::size_t operator()(const LowEdge &e) const
        {
            using std::hash;

            return hash<bool>()(e.complemented) << 1 ^ (hash<BDD_ID>()(e.node) << 1);
        }
    };

    struct Node
    {
        // Indicates that this is a complemented root node
        bool complemented;

        BDD_ID High;
        LowEdge Low;
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
        LowEdge low;
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

            return ((hash<BDD_ID>()(k.topVar) ^ (LowEdgeHasher()(k.low) << 1)) >> 1) ^ (hash<BDD_ID>()(k.high) << 1);
        }
    };

    std::unordered_map<SubGraphTableEntry, BDD_ID, SubGraphTableEntryHasher> SubGraphTable;

    BDD_ID negativeReference(BDD_ID f) {
        if (NegativeReferenceTable.find(f) != NegativeReferenceTable.end())
        {
            return NegativeReferenceTable.at(f);
        }

        BDD_ID newID = Table.size();
        Table.insert({newID, Node{true, 0, LowEdge{false, f}, topVar(f), ""}});
        NegativeReferenceTable.insert({f, newID});
        NegativeReferenceTable.insert({newID, f});
        return newID;
    }

    std::unordered_map<BDD_ID, BDD_ID> NegativeReferenceTable;

public:
    Manager()
    {
        Table.insert({0, Node{true, 0, LowEdge{false, 1}, 0, "False"}});
        // Table.insert({0, Node{false, 0, LowEdge{false, 0}, 0, "False"}});
        Table.insert({1, Node{false, 1, LowEdge{false, 1}, 1, "True"}});
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

    void printTable() const;
    void printTruthTable(BDD_ID f);
};

} // namespace ClassProject

#endif
