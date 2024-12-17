#ifndef PARSER 
#define PARSER

// parser of myscheme 

#include "RE.hpp"
#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include <map>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#define mp make_pair
using std :: string;
using std :: vector;
using std :: pair;

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Expr Syntax :: parse(Assoc &env) {
    return (*this)->parse(env);
}

Expr Number :: parse(Assoc &env) {
    return Expr(new Fixnum(n));
}
ExprType Number :: get_type() { return E_FIXNUM; }

Expr Identifier :: parse(Assoc &env) {
    return Expr(new Var(s));
}
ExprType Identifier :: get_type() { 
    if (primitives.find(s) != primitives.end()) { return primitives[s]; }
    if (reserved_words.find(s) != reserved_words.end()) { return reserved_words[s]; }
    return E_VAR;
}

Expr TrueSyntax :: parse(Assoc &env) {
    return Expr(new True());
}
ExprType TrueSyntax :: get_type() { return E_TRUE; }

Expr FalseSyntax :: parse(Assoc &env) {
    return Expr(new False());
}
ExprType FalseSyntax :: get_type() { return E_FALSE; }

ExprType List :: get_type() { return E_LIST; }
Expr List :: parse(Assoc &env) {
    if (stxs.empty()) {
       throw RuntimeError("nothing here"); 
    }
    Identifier* id = dynamic_cast<Identifier*>(stxs[0].get());
    ExprType eptype = stxs[0]->get_type();
    string w_num = "incorrect number of parameters";
    //if input is in primitives
    if (eptype == E_EXIT) { return Expr(new Exit()); }
    if (eptype == E_PLUS) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Plus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if (eptype == E_MINUS) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Minus(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if (eptype == E_MUL) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Mult(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if (eptype == E_VOID) {
        if (stxs.size() != 1) { throw RuntimeError(w_num); }
        return Expr(new MakeVoid());
    }
    if (eptype == E_CONS) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Cons(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// construct a pair
    if (eptype == E_CAR) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new Car(stxs[1]->parse(env)));
    }// first
    if (eptype == E_CDR) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new Cdr(stxs[1]->parse(env)));
    }// second
    if (eptype == E_BEGIN) {
        vector<Expr> es;
        for (int i = 1; i < stxs.size(); ++i) {
            es.push_back(stxs[i]->parse(env));
        }
        return Expr(new Begin(es));
    }
    if (eptype == E_LT) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Less(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// <
    if (eptype == E_LE) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new LessEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// <=
    if (eptype == E_EQ) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Equal(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// =
    if (eptype == E_GT) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new Greater(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// >
    if (eptype == E_GE) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new GreaterEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }// >=
    if (eptype == E_EQQ) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        return Expr(new IsEq(stxs[1]->parse(env), stxs[2]->parse(env)));
    }
    if (eptype == E_NOT) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new Not(stxs[1]->parse(env)));
    }
    if (eptype == E_INTQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsFixnum(stxs[1]->parse(env)));
    }//fixnum?
    if (eptype == E_NULLQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsNull(stxs[1]->parse(env)));
    }
    if (eptype == E_PAIRQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsPair(stxs[1]->parse(env)));
    }
    if (eptype == E_SYMBOLQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsSymbol(stxs[1]->parse(env)));
    }
    if (eptype == E_BOOLQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsBoolean(stxs[1]->parse(env)));
    }
    
    //if input is in reservedwords
    if (eptype == E_QUOTE) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new Quote(stxs[1]));
    }
    if (eptype == E_IF) {
        if (stxs.size() != 4) { throw RuntimeError(w_num); }
        return Expr(new If(stxs[1]->parse(env), stxs[2]->parse(env), stxs[3].parse(env)));
    }
    if (eptype == E_LAMBDA) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("invalid type"); }
        List* paras = dynamic_cast<List*>(stxs[1].get());
        //collect all parameters
        // Assoc space = empty();
        // for (auto i = env; i.get() != nullptr; i = i->next) {
        //     std::cout << "hello" << std::endl;
        //     space  = extend(i->x, i->v, space);
        // }
        vector<string> xs;
        for (int i = 0; i < paras->stxs.size(); ++i) {
            Identifier* id = dynamic_cast<Identifier*>(paras->stxs[i].get());
            if (id == nullptr) { throw RuntimeError("invalid type 1"); }
            xs.push_back(id->s);
            env = extend(id->s, NullV(), env);
        }
        return Expr(new Lambda(xs, stxs[2]->parse(env)));
    }
    vector<Expr> expr;
    for (int i = 1; i < stxs.size(); ++i) {
        expr.push_back(stxs[i]->parse(env));
    }
    //std::cout << "hi" << std::endl;
    return Expr(new Apply(stxs[0]->parse(env), expr));
}

#endif