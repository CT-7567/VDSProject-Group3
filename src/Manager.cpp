#include <iostream>
#include "Manager.h"

namespace ClassProject {

BDD_ID Manager::createVar(const std::string &label)
{
    BDD_ID newID = Tabel.size();
    Tabel.insert( { newID, Node{ 1, 0, newID, label } } );
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
    if(f == 0 or f == 1){
        return true;
    }
    return false;
}

bool Manager::isVariable(BDD_ID x)
{
    if( Tabel.at(x).Label.size() == 1 ){
        return true;
    }
    return false;
}

BDD_ID Manager::topVar(BDD_ID f)
{
    return Tabel.at(f).TopVar;
}

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e)
{
    if(isConstant(i)){
        return i;
    }
    BDD_ID x = topVar(i);
    BDD_ID rHigh, rLow;
    //BDD_ID rHigh = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));
    if(coFactorTrue(i, x) == 1){
        rHigh = coFactorTrue(t, x);
    }else{
        rHigh = coFactorTrue(e, x);
    }
    //BDD_ID rLow = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));
    if(coFactorFalse(i, x) == 1){
        rLow = coFactorFalse(t, x);
    }else{
        rLow = coFactorFalse(e, x);
    }

    if(rHigh == rLow){
        return rHigh;
    }

    for(int j = 0; j < Tabel.size(); j++){
        if(Tabel.at(j).TopVar == x and Tabel.at(j).Low == rLow and Tabel.at(j).High == rHigh){
            return x;
        }
    }

    BDD_ID newID = Tabel.size();
    Tabel.insert({newID, {rHigh, rLow, x, "test"}});
    return newID;
}

BDD_ID Manager::coFactorTrue(BDD_ID f, BDD_ID x)
{
        // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (topVar(f) == x) {
        return Tabel.at(f).High;
    }

    auto true_case = coFactorFalse(Tabel.at(f).High, x);
    auto false_case = coFactorFalse(Tabel.at(f).Low, x);

    return ite(topVar(f), true_case, false_case);
}

BDD_ID Manager::coFactorFalse(BDD_ID f, BDD_ID x)
{
    // Terminal cases
    if (isConstant(f) || isConstant(x) || topVar(f) > x) {
        return f;
    }

    if (topVar(f) == x) {
        return Tabel.at(f).Low;
    }

    auto true_case = coFactorFalse(Tabel.at(f).High, x);
    auto false_case = coFactorFalse(Tabel.at(f).Low, x);

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
    return 0;
}

BDD_ID Manager::and2(BDD_ID a, BDD_ID b)
{
    auto a_and_b = createVar(( "("+Tabel.at(a).Label+" * "+Tabel.at(b).Label+")" ));

    Tabel.at(a_and_b).TopVar = ite( a, FALSE_ID, b );

    Tabel.at(a_and_b).High = ite( coFactorTrue(a, a), coFactorTrue(FALSE_ID, a), coFactorTrue(b, a) );
    Tabel.at(a_and_b).Low = ite( coFactorFalse(a, a), coFactorFalse(FALSE_ID, a), coFactorFalse(b, a) );

    return a_and_b;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b)
{
    auto a_or_b = createVar(( "("+Tabel.at(a).Label+" + "+Tabel.at(b).Label+")" ));

    Tabel.at(a_or_b).TopVar = ite( a, 1, b );

    Tabel.at(a_or_b).High = ite( coFactorTrue(a, a), coFactorTrue(TRUE_ID, a), coFactorTrue(b, a) );
    Tabel.at(a_or_b).Low = ite( coFactorFalse(a, a), coFactorFalse(TRUE_ID, a), coFactorFalse(b, a) );

    return a_or_b;

}

BDD_ID Manager::xor2(BDD_ID a, BDD_ID b)
{
}

BDD_ID Manager::nand2(BDD_ID a, BDD_ID b)
{
}

BDD_ID Manager::nor2(BDD_ID a, BDD_ID b)
{
}

BDD_ID Manager::xnor2(BDD_ID a, BDD_ID b)
{
}

std::string Manager::getTopVarName(const BDD_ID &root)
{
    return Tabel.at(topVar(root)).Label;
}

void Manager::findNodes(const BDD_ID &root, std::set<BDD_ID> &nodes_of_root)
{
}

void Manager::findVars(const BDD_ID &root, std::set<BDD_ID> &vars_of_root)
{
}

size_t Manager::uniqueTableSize()
{
}

}; // namespace ClassProject