#include <iostream>
#include <map>
#include <string>
#include <typeinfo>

#include "rep/r0.h"

using namespace std;
using namespace r0;

// I kept eval separate from the r0 classes because I felt that the interpreter
// should be its own separate entity. Hence, I have to use some nasty casting.
int eval(const E* e, map<string, int> vmap)
{
    if (typeid(*e) == typeid(Num))
    {
        return static_cast<const Num*>(e)->value;
    }
    else if (typeid(*e) == typeid(Read))
    {
        int val;
        cin >> val;
        return val;
    }
    else if (typeid(*e) == typeid(Binop))
    {
        auto b = static_cast<const Binop*>(e);
        int l = eval(b->l, vmap);
        int r = eval(b->r, vmap);
        int result;
        switch (b->op)
        {
            case B_PLUS:
                result = l+ r;
                break;
            default:
                std::cout << "WARN: unknown binary operator: " << b->op << "\n";
                break;
        }
        return result;
    }
    else if (typeid(*e) == typeid(Unop))
    {
        auto u = static_cast<const Unop*>(e);
        int val = eval(u->v, vmap);
        int result;
        switch (u->op)
        {
            case U_NEG:
                result = -val;
                break;
            default:
                std::cout << "WARN: unknown unary operator: " << u->op << "\n";
                break;
        }
        return result;
    }
    else if (typeid(*e) == typeid(Var))
    {
        auto v = static_cast<const Var*>(e);
        return vmap.at(v->name);
    }
    else if (typeid(*e) == typeid(Let))
    {
        auto l = static_cast<const Let*>(e);
        vmap[l->name] = eval(l->ve, vmap);
        return eval(l->be, vmap);
    }
    else
    {
        if (typeid(e) == typeid(const E*))
        {
            cerr << "HAHA";
        }
        cerr << "ERROR: expression type invalid?";
        return 0;
    }
}

int interp(const P &p)
{
    map<string, int> map;
    return eval(p.e, map) ;
}

bool test_interp(const P &p, int expect)
{
    int actual = interp(p);
    return expect == actual;
}
