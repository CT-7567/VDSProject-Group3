#include "Manager.h"

#include <iomanip>
#include <iostream>

namespace ClassProject {

BDD_ID Manager::createVar(const std::string &label)
{
    BDD_ID newID = Table.size();
    Table.insert({newID, Node{1, 0, newID, label}});
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
        } else
            return e;
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
    } else
        x = TopA;

    rHigh = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));

    rLow = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));

    if (rHigh == rLow) {
        return rHigh;
    }

    for (int j = 0; j < Table.size(); j++) {
        if (Table.at(j).TopVar == x and Table.at(j).Low == rLow and Table.at(j).High == rHigh) {
            return j;
        }
    }
    BDD_ID newID = Table.size();
    std::string new_label = "test";
    if (rHigh == TRUE_ID) {
        new_label = "(" + Table.at(Table.at(x).TopVar).Label + "+" + Table.at(rLow).Label + ")";
    } else if (rHigh == FALSE_ID and rLow == TRUE_ID) {
        new_label = "~" + Table.at(Table.at(x).TopVar).Label;
    } else if (rLow == TRUE_ID) {
        new_label = "(~" + Table.at(Table.at(x).TopVar).Label + +"+" + Table.at(rHigh).Label + ")";
    } else if (rHigh == FALSE_ID) {
        new_label = "(~" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rLow).Label + ")";
    } else if (rLow == FALSE_ID) {
        new_label = "(" + Table.at(Table.at(x).TopVar).Label + "*" + Table.at(rHigh).Label + ")";
    }
    Table.insert({newID, {rHigh, rLow, x, new_label}});
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
    Table.at(temp).Label = "~" + Table.at(a).Label;
    return temp;
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b)
{

    auto a_and_b = ite(a, b, FALSE_ID);

    Table.at(a_and_b).Label = "(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";

    return a_and_b;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b)
{

    auto a_or_b = ite(a, TRUE_ID, b);

    Table.at(a_or_b).Label = "(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";

    return a_or_b;
}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b)
{

    auto a_xor_b = or2(and2(neg(a), b), and2(a, neg(b)));

    Table.at(a_xor_b).Label = "(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";

    return a_xor_b;
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b)
{
    auto a_nand_b = neg(and2(a, b));

    Table.at(a_nand_b).Label = "~(" + Table.at(a).Label + "*" + Table.at(b).Label + ")";

    return a_nand_b;
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b)
{
    auto a_nor_b = neg(or2(a, b));

    Table.at(a_nor_b).Label = "~(" + Table.at(a).Label + "+" + Table.at(b).Label + ")";

    return a_nor_b;
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b)
{

    auto a_xnor_b = neg(xor2(a, b));

    Table.at(a_xnor_b).Label = "~(" + Table.at(a).Label + "⊕" + Table.at(b).Label + ")";

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

        // nodes_of_root.insert( Tabel.at(root).TopVar );

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

void Manager::printTable()
{
    std::cout << std::setw(10) << "ID" << std::setw(25) << "Label" << std::setw(10) << "High" << std::setw(10) << "Low"
              << std::setw(10) << "TopVar" << std::endl;

    for (int i = 0; i < Table.size(); i++) {
        auto const &node = Table.at(i);
        std::cout << std::setw(10) << i << std::setw(25) << node.Label << std::setw(10) << node.High << std::setw(10)
                  << node.Low << std::setw(10) << node.TopVar << std::endl;
    }
}

}; // namespace ClassProject