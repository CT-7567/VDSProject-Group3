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

BDD_ID Manager::ite(BDD_ID i, BDD_ID t, BDD_ID e) {
    if (isConstant(i)) {
        if (i == 1) {
            return t;
        } else return e;
    }
    BDD_ID rHigh, rLow, x;
    auto TopA = topVar(i);
    auto TopB = topVar(t);
    auto TopC = topVar(e);
    if (!isConstant(TopB) and !isConstant(TopC)) {
        x = std::min(TopA, std::min(TopB, TopC));
    } else if (!isConstant(TopB) and isConstant(TopC)){
        x = std::min(TopB, TopA);
    } else if (isConstant(TopB) and !isConstant(TopC)){
        x = std::min(TopA, TopC);
    } else x = TopA;


    rHigh = ite(coFactorTrue(i, x), coFactorTrue(t, x), coFactorTrue(e, x));

    rLow = ite(coFactorFalse(i, x), coFactorFalse(t, x), coFactorFalse(e, x));


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

    auto true_case = coFactorTrue(Tabel.at(f).High, x);
    auto false_case = coFactorTrue(Tabel.at(f).Low, x);

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
    auto a_and_b = createVar(( "("+Tabel.at(a).Label+"*"+Tabel.at(b).Label+")" ));

    auto TopA = topVar(a);
    auto TopB = topVar(b);
    BDD_ID Top;

    if(TopA <= TopB)
    {
        Top = TopA;
    }
    else
    {
        Top = TopB;
    }
        
    Tabel.at(a_and_b).TopVar = Top;

    Tabel.at(a_and_b).High = ite( coFactorTrue(a, Top), coFactorTrue(FALSE_ID, Top), coFactorTrue(b, Top) );
    Tabel.at(a_and_b).Low = ite( coFactorFalse(a, Top), coFactorFalse(FALSE_ID, Top), coFactorFalse(b, Top) );

    return a_and_b;
}

BDD_ID Manager::or2(BDD_ID a, BDD_ID b)
{
    auto a_or_b = createVar(( "("+Tabel.at(a).Label+"+"+Tabel.at(b).Label+")" ));

    auto TopA = topVar(a);
    auto TopB = topVar(b);
    BDD_ID Top;

    if(TopA <= TopB)
    {
        Top = TopA;
    }
    else
    {
        Top = TopB;
    }
        
    Tabel.at(a_or_b).TopVar = Top;


    Tabel.at(a_or_b).High = ite( coFactorTrue(a, Top), coFactorTrue(TRUE_ID, Top), coFactorTrue(b, Top) );
    Tabel.at(a_or_b).Low = ite( coFactorFalse(a, Top), coFactorFalse(TRUE_ID, Top), coFactorFalse(b, Top) );

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



void Manager::printTable()
{

    for( int i=0; i < Tabel.size(); i++ )
    {

        std::cout<<"ID: "<<  i  << "     |   Label: " << Tabel.at(i).Label << "     |   High: " << Tabel.at(i).High << "        |   Low: " <<  Tabel.at(i).Low << "     |   TopVar: "<< Tabel.at(i).TopVar << std::endl;  

    }

}


}; // namespace ClassProject