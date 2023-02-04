#include "Manager.h"

#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>

namespace ClassProject {

Manager::Manager()
{
    Table.emplace_back(Node{0, 0, 0, "False"});
    Table.emplace_back(Node{1, 1, 1, "True"});
}

BDD_ID Manager::createVar(const std::string &label)
{
    BDD_ID newID = Table.size();
    Table.emplace_back(Node{1, 0, newID, label});

    SubGraphTable.emplace(BDDTriple{newID, 0, 1}, newID);

    return newID;
}

const BDD_ID &Manager::True()
{
    return TRUE_ID;
}

const BDD_ID &Manager::False()
{
    return FALSE_ID;
}

bool Manager::isConstant(BDD_ID f)
{
    return (f == 0 or f == 1);
}

bool Manager::isVariable(BDD_ID x)
{
    return (Table.at(x).TopVar == x) && (x != TRUE_ID) && (x != FALSE_ID);
}

BDD_ID Manager::topVar(BDD_ID f)
{
    return Table.at(f).TopVar;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e)
{
    if (isConstant(i)) {
        if (i == 1) {
            return t;
        }

        return e;
    }

    if (t == 1 && e == 0) {
        return i;
    }

    if (t == e) {
        return t;
    }

    auto computedIt = ComputedTable.find({i, t, e});
    if (computedIt != ComputedTable.end()) {
        return computedIt->second;
    }

    BDD_ID rHigh, rLow, x;
    auto TopA = topVar(i);
    auto TopB = topVar(t);
    auto TopC = topVar(e);
    if (!isConstant(TopB) and !isConstant(TopC)) {
        x = std::min(TopA, std::min(TopB, TopC));
    } else if (!isConstant(TopB) and isConstant(TopC)) {
        x = std::min(TopB, TopA);
    } else if (isConstant(TopB) and !isConstant(TopC)) {
        x = std::min(TopA, TopC);
    } else {
        x = TopA;
    }

    rHigh = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
    rLow = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));

    if (rHigh == rLow) {
        return rHigh;
    }

    auto subGraphIt = SubGraphTable.find({x, rLow, rHigh});
    if (subGraphIt != SubGraphTable.end()) {
        ComputedTable.emplace(BDDTriple{i, t, e}, subGraphIt->second);
        return subGraphIt->second;
    }

    BDD_ID newID = Table.size();

    std::string label;
#ifdef ENABLE_LABELS
    if (rHigh == TRUE_ID) {
        label = "(" + Table.at(Table.at(x).TopVar).Label + "+" + Table.at(rLow).Label + ")";
    } else if (rHigh == FALSE_ID and rLow == TRUE_ID) {
        label = "~" + Table.at(Table.at(x).TopVar).Label;
    } else if (rLow == TRUE_ID) {
        label = "(~" + Table.at(Table.at(x).TopVar).Label + +"+" + Table.at(rHigh).Label + ")";
    } else if (rHigh == FALSE_ID) {
        label = "(~" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rLow).Label + ")";
    } else if (rLow == FALSE_ID) {
        label = "(" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rHigh).Label + ")";
    }
#endif

    Table.emplace_back(Node{rHigh, rLow, x, label});

    ComputedTable.emplace(BDDTriple{i, t, e}, newID);
    SubGraphTable.emplace(BDDTriple{x, rLow, rHigh}, newID);

    return newID;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x)
{
    // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (topVar(f) == x) {
        return Table.at(f).High;
    }

    auto true_case = coFactorTrue(Table.at(f).High, x);
    auto false_case = coFactorTrue(Table.at(f).Low, x);

    return ite(topVar(f), true_case, false_case);
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x)
{
    // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (topVar(f) == x) {
        return Table.at(f).Low;
    }

    auto true_case = coFactorFalse(Table.at(f).High, x);
    auto false_case = coFactorFalse(Table.at(f).Low, x);

    return ite(topVar(f), true_case, false_case);
}

BDD_ID Manager::coFactorTrue(BDD_ID f)
{
    return coFactorTrue(f, topVar(f));
}

BDD_ID Manager::coFactorFalse(BDD_ID f)
{
    return coFactorFalse(f, topVar(f));
}

BDD_ID Manager::neg(BDD_ID a)
{
    BDD_ID temp = ite(a, 0, 1);
#ifdef ENABLE_LABELS
    Table.at(temp).Label = "~" + Table.at(a).Label;
#endif
    return temp;
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b)
{
    auto a_and_b = ite(a, b, FALSE_ID);
#ifdef ENABLE_LABELS
    Table.at(a_and_b).Label = "(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";
#endif
    return a_and_b;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b)
{
    auto a_or_b = ite(a, TRUE_ID, b);
#ifdef ENABLE_LABELS
    Table.at(a_or_b).Label = "(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";
#endif
    return a_or_b;
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b)
{
    auto a_xor_b = ite(a, neg(b), b);
#ifdef ENABLE_LABELS
    Table.at(a_xor_b).Label = "(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";
#endif
    return a_xor_b;
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b)
{
    auto a_nand_b = ite(a, neg(b), TRUE_ID);
#ifdef ENABLE_LABELS
    Table.at(a_nand_b).Label = "~(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";
#endif
    return a_nand_b;
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b)
{
    auto a_nor_b = ite(a, FALSE_ID, neg(b));
#ifdef ENABLE_LABELS
    Table.at(a_nor_b).Label = "~(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";
#endif
    return a_nor_b;
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b)
{
    auto a_xnor_b = ite(a, b, neg(b));
#ifdef ENABLE_LABELS
    Table.at(a_xnor_b).Label = "~(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";
#endif
    return a_xnor_b;
}

std::string Manager::getTopVarName(const BDD_ID &root)
{
    return Table.at(topVar(root)).Label;
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root)
{
    if (!isConstant(root)) {
        nodes_of_root.insert(root);
        findNodes(Table.at(root).High, nodes_of_root);
        findNodes(Table.at(root).Low, nodes_of_root);
    } else {
        nodes_of_root.insert(root);
    }
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root)
{
    if (!isConstant(root)) {
        vars_of_root.insert(topVar(root));
        findVars(Table.at(root).High, vars_of_root);
        findVars(Table.at(root).Low, vars_of_root);
    }
}

size_t Manager::uniqueTableSize()
{
    return Table.size();
}

void Manager::printTable() const
{
    std::cout << std::setw(10) << "ID" << std::setw(25) << "Label" << std::setw(10) << "High" << std::setw(10) << "Low"
              << std::setw(10) << "TopVar" << std::endl;

    for (int i = 0; i < Table.size(); i++) {
        auto const &node = Table.at(i);
        std::cout << std::setw(10) << i << std::setw(25) << node.Label << std::setw(10) << node.High << std::setw(10)
                  << node.Low << std::setw(10) << node.TopVar << std::endl;
    }
}

void Manager::printTruthTable(BDD_ID f)
{
    std::set<BDD_ID> vars;
    findVars(f, vars);

    for (auto var : vars) {
        auto const &node = Table.at(var);
        std::cout << std::setw(5) << node.Label;
    }

    std::cout << std::setw(5) << "f" << std::endl;

    int level = vars.size();

    std::function<void(BDD_ID, std::vector<int>)> printSubTable;
    printSubTable = [this, level, &printSubTable](BDD_ID f, std::vector<int> subTable) {
        if (subTable.size() == level) {
            subTable.push_back(f);

            for (auto i : subTable) {
                std::cout << std::setw(5) << i;
            }

            std::cout << std::endl;
            return;
        }

        subTable.push_back(0);
        printSubTable(coFactorFalse(f), subTable);

        subTable.pop_back();
        subTable.push_back(1);
        printSubTable(coFactorTrue(f), subTable);
    };

    std::vector<int> subTable;
    printSubTable(f, subTable);
}

}; // namespace ClassProject