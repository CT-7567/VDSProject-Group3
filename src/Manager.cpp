#include "Manager.h"

#include <iomanip>
#include <iostream>
#include <algorithm>

namespace ClassProject {

BDD_ID Manager::createVar(const std::string &label)
{
    BDD_ID newID = Table.size();
    Table.insert({newID, Node{false, 1, LowEdge{true, 1}, newID, label}});

    SubGraphTable.emplace(SubGraphTableEntry{newID, 0, 1}, newID);
    NegativeReferenceTable.emplace(FALSE_ID, TRUE_ID);
    NegativeReferenceTable.emplace(TRUE_ID, FALSE_ID);

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
    if (f == 0 or f == 1) {
        return true;
    }
    return false;
}

bool Manager::isVariable(BDD_ID x)
{
    if ((Table.at(x).TopVar == x) && (x != TRUE_ID) && (x != FALSE_ID)) {
        return true;
    }
    return false;
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

    if (t == False() && e == True()) {
        return negativeReference(i);
    }

    if (ComputedTable.find({i, t, e}) != ComputedTable.end()) {
        return ComputedTable.at({i, t, e});
    }

    BDD_ID topVariable;
    auto TopA = topVar(i);
    auto TopB = topVar(t);
    auto TopC = topVar(e);
    if (!isConstant(TopB) and !isConstant(TopC)) {
        topVariable = std::min(TopA, std::min(TopB, TopC));
    } else if (!isConstant(TopB) and isConstant(TopC)) {
        topVariable = std::min(TopB, TopA);
    } else if (isConstant(TopB) and !isConstant(TopC)) {
        topVariable = std::min(TopA, TopC);
    } else {
        topVariable = TopA;
    }

    BDD_ID rHigh = ite(coFactorTrue(i, topVariable), coFactorTrue(t, topVariable), coFactorTrue(e, topVariable));
    BDD_ID rLow = ite(coFactorFalse(i, topVariable), coFactorFalse(t, topVariable), coFactorFalse(e, topVariable));
    
    bool complementLow = false;
    bool complementNode = false;

    if (Table.at(rLow).complemented)
    {
        complementLow = true;
        rLow = Table.at(rLow).Low.node;
    }

    if (Table.at(rHigh).complemented)
    {
        complementNode = true;
        complementLow = !complementLow;
        rHigh = Table.at(rHigh).Low.node;
    }

    if (rHigh == rLow) {
        return rHigh;
    }

    if (SubGraphTable.find({topVariable, rLow, rHigh}) != SubGraphTable.end()) {
        return SubGraphTable.at({topVariable, rLow, rHigh});
    }

    BDD_ID newID = Table.size();

    // std::string new_label;
    // if (rHigh == TRUE_ID) {
    //     new_label = "(" + Table.at(Table.at(x).TopVar).Label + "+" + Table.at(rLow).Label + ")";
    // } else if (rHigh == FALSE_ID and rLow == TRUE_ID) {
    //     new_label = "~" + Table.at(Table.at(x).TopVar).Label;
    // } else if (rLow == TRUE_ID) {
    //     new_label = "(~" + Table.at(Table.at(x).TopVar).Label + +"+" + Table.at(rHigh).Label + ")";
    // } else if (rHigh == FALSE_ID) {
    //     new_label = "(~" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rLow).Label + ")";
    // } else if (rLow == FALSE_ID) {
    //     new_label = "(" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rHigh).Label + ")";
    // }
    
    Table.insert({newID, Node{false, rHigh, LowEdge{complementLow, rLow}, topVariable, ""}});

    if (complementNode) {
        newID = negativeReference(newID);
    }

    ComputedTable.emplace(ComputedTableEntry{i, t, e}, newID);
    SubGraphTable.emplace(SubGraphTableEntry{topVariable, rLow, rHigh}, newID);

    return newID;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x)
{
    // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (Table.at(f).complemented) {
        BDD_ID tempNode = Table.at(f).Low.node;
        if (topVar(f) == x) {
            return negativeReference(Table.at(tempNode).High);
        }

        BDD_ID false_case;
        BDD_ID true_case = coFactorTrue(Table.at(Table.at(f).Low.node).High, x);

        if (Table.at(tempNode).Low.complemented) {
            false_case = negativeReference(coFactorTrue(Table.at(tempNode).Low.node, x));
        } else {
            false_case = coFactorTrue(Table.at(tempNode).Low.node, x);
        }

        auto result = ite(topVar(f), true_case, false_case);
        return negativeReference(result);
    }

    if (topVar(f) == x) {
        return Table.at(f).High;
    }

    BDD_ID false_case;
    BDD_ID true_case = coFactorTrue(Table.at(f).High, x);

    if (Table.at(f).Low.complemented) {
        false_case = negativeReference(coFactorTrue(Table.at(f).Low.node, x));
    } else {
        false_case = coFactorTrue(Table.at(f).Low.node, x);
    }

    auto result = ite(topVar(f), true_case, false_case);
    return result;
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x)
{
    // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (Table.at(f).complemented) {
        BDD_ID tempNode = Table.at(f).Low.node;

        if (topVar(f) == x) {
            if (Table.at(tempNode).Low.complemented) {
                return Table.at(tempNode).Low.node;
            }

            return negativeReference(Table.at(tempNode).Low.node);
        }

        BDD_ID true_case = coFactorFalse(Table.at(Table.at(f).Low.node).High, x);

        BDD_ID false_case;
        if (Table.at(tempNode).Low.complemented) {
            false_case = negativeReference(coFactorFalse(Table.at(tempNode).Low.node, x));
        } else {
            false_case = coFactorFalse(Table.at(tempNode).Low.node, x);
        }

        auto result = ite(topVar(f), true_case, false_case);
        return negativeReference(result);
    }

    if (topVar(f) == x) {
        BDD_ID terminalCase;

        if (!Table.at(f).Low.complemented) {
            terminalCase = Table.at(f).Low.node;
        } else {
            terminalCase = negativeReference(Table.at(f).Low.node);
        }

        return terminalCase;
    }

    BDD_ID true_case = coFactorFalse(Table.at(f).High, x);
    BDD_ID false_case;

    if (Table.at(f).Low.complemented) {
        false_case = negativeReference(coFactorFalse(Table.at(f).Low.node, x));
    } else {
        false_case = coFactorFalse(Table.at(f).Low.node, x);
    }

    auto result = ite(topVar(f), true_case, false_case);
    return result;
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
    // Table.at(temp).Label = "~" + Table.at(a).Label;
    return temp;
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b)
{

    auto a_and_b = ite(a, b, FALSE_ID);

    // Table.at(a_and_b).Label = "(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";

    return a_and_b;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b)
{

    auto a_or_b = ite(a, TRUE_ID, b);

    // Table.at(a_or_b).Label = "(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";

    return a_or_b;
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b)
{

    auto a_xor_b = ite(a, neg(b), b);

    // Table.at(a_xor_b).Label = "(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";

    return a_xor_b;
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b)
{
    auto a_nand_b = ite(a, neg(b), TRUE_ID);

    // Table.at(a_nand_b).Label = "~(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";

    return a_nand_b;
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b)
{
    auto a_nor_b = ite(a, FALSE_ID, neg(b));

    // Table.at(a_nor_b).Label = "~(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";

    return a_nor_b;
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b)
{

    auto a_xnor_b = ite(a, b, neg(b));

    // Table.at(a_xnor_b).Label = "~(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";

    return a_xnor_b;
}

std::string Manager::getTopVarName(const BDD_ID &root)
{
    return Table.at(topVar(root)).Label;
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root)
{
    // if (Table.at(root).complemented)
    //     return findNodes(Table.at(root).)

    if (!isConstant(root)) {
        nodes_of_root.insert(root);

        // nodes_of_root.insert( Tabel.at(root).TopVar );

        findNodes(Table.at(root).High, nodes_of_root);
        findNodes(Table.at(root).Low.node, nodes_of_root);
    } else {

        nodes_of_root.insert(root);
    }
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root)
{
    if (!isConstant(root)) {
        vars_of_root.insert(topVar(root));

        findVars(Table.at(root).High, vars_of_root);

        findVars(Table.at(root).Low.node, vars_of_root);
    }
}

size_t Manager::uniqueTableSize()
{
    return Table.size();
}

void Manager::printTable()
{
    std::cout << std::setw(10) << "ID" << std::setw(25) << "Label" << std::setw(10) << "High" << std::setw(10) << "Low"
              << std::setw(20) << "LowComplemented" << std::setw(10) << "TopVar" << std::setw(10) << "RefNode"
              << std::endl;

    for (int i = 0; i < Table.size(); i++) {
        auto const &node = Table.at(i);
        std::cout << std::setw(10) << i << std::setw(25) << node.Label << std::setw(10) << node.High << std::setw(10)
                  << node.Low.node << std::setw(20) << node.Low.complemented << std::setw(10) << node.TopVar
                  << std::setw(10) << node.complemented << std::endl;
    }
}

}; // namespace ClassProject