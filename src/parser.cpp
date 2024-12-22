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
    bool flag = 0;
    for (auto i = env; i.get() != nullptr; i = i->next) {
        if (i->x == s) { flag = 1;  break; }
    }
    if (!flag) {
        env = extend(s, NullV(), env, 0);
    }
    return Expr(new Var(s));
}
ExprType Identifier :: get_type() { 
    if (primitives.find(s) != primitives.end()) { return primitives[s]; }
    if (reserved_words.find(s) != reserved_words.end()) { return reserved_words[s]; }
    if (s == ".") { return E_DOT; }
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

    if(id != nullptr && primitives.find(id->s) == primitives.end()){
        bool usedf=0;
        for(auto i = env; i.get() != nullptr; i = i->next)
            if(i->x == id->s) { usedf = 1;  break; }
        if(usedf){
            //std::cout<<"used function "<< id->s << std::endl;
            vector<Expr> expr;
            for(int i = 1; i < stxs.size(); ++i)
            expr.push_back(stxs[i]->parse(env));
            return Expr(new Apply(stxs[0]->parse(env), expr));
        }
    }
    //std::cout << "wwwwwwwwwww   " << id->s << std::endl;

    // if (eptype == E_VAR || eptype == E_DOT) {
    //     return Expr(new Var(id->s));
    // }
    //if input is in primitives
    if (eptype == E_EXIT) {
        if (stxs.size() == 1) { return Expr(new Exit()); }
        throw RuntimeError("invalid expression");
    }
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
    if (eptype == E_PROCQ) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new IsProcedure(stxs[1]->parse(env)));
    }
    
    //if input is in reservedwords
    if (eptype == E_QUOTE) {
        if (stxs.size() != 2) { throw RuntimeError(w_num); }
        return Expr(new Quote(stxs[1]));
    }
    if (eptype == E_IF) {
        if (stxs.size() != 4) { throw RuntimeError(w_num); }
        return Expr(new If(stxs[1]->parse(env), stxs[2]->parse(env), stxs[3]->parse(env)));
    }
    if (eptype == E_LAMBDA) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }
        if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("invalid type"); }
        List* paras = dynamic_cast<List*>(stxs[1].get());
        vector<string> xs;
        for (int i = 0; i < paras->stxs.size(); ++i) {
            Identifier* id = dynamic_cast<Identifier*>(paras->stxs[i].get());
            if (id == nullptr) { throw RuntimeError("invalid type 1"); }
            xs.push_back(id->s);
            env = extend(id->s, Value(nullptr), env, 0);
        }
        return Expr(new Lambda(xs, stxs[2]->parse(env)));
    }
    if (eptype == E_LET || eptype == E_LETREC) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }

        if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
        List* paras = dynamic_cast<List*>(stxs[1].get());
        vector< pair<string, Expr>> vec;

        for (int i = 0; i < paras->stxs.size(); ++i) {
            if (paras->stxs[i]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
            List* var = dynamic_cast<List*>(paras->stxs[i].get());
            if (var->stxs.size() != 2) { throw RuntimeError(w_num); }
            //a var
            Identifier* name = dynamic_cast<Identifier*>(var->stxs[0].get());
            if (name == nullptr) { //std::cout << paras->stxs[0]->get_type() << std::endl; 
            throw RuntimeError("Not A Var"); }
            // if (eptype == E_LETREC) {
            //     bool usedf = 0;
            //     for(auto i = env; i.get() != nullptr; i = i->next)
            //         if(i->x == name->s) { usedf = 1;  break; }
            //     if(!usedf){
            //         env = extend(name->s, NullV(), env, 0);
            //     }
            // }
            Expr expr = var->stxs[1]->parse(env);
            vec.push_back(mp(name->s, expr));
        }

        if (stxs[2]->get_type() == E_LIST) {
            List* es = dynamic_cast<List*>(stxs[2].get());
            if (es->stxs.empty()) {
                if (eptype == E_LET)  return Expr(new Let(vec, new MakeVoid()));
                return Expr(new Letrec(vec, new MakeVoid()));
            }
            if (es->stxs[0]->get_type() == E_LET || es->stxs[0]->get_type() == E_LETREC) {
                //std::cout << "jump" << std::endl;
                goto jump;
            }
            Identifier* id = dynamic_cast<Identifier*>(es->stxs[0].get());
            if (id == nullptr) { throw RuntimeError("invalid expression"); }

            Expr var = Expr(nullptr);
            if (reserved_words.find(id->s) == reserved_words.end()) {
                var = new Var(id->s);
            } else {
                if (eptype == E_LET)  return Expr(new Let(vec, es->parse(env)));
                return Expr(new Letrec(vec, es->parse(env)));
            }
            
            vector<Expr> expr;
            for (int i = 1; i < es->stxs.size(); ++i) {
                expr.push_back(es->stxs[i]->parse(env));
            }
            if (eptype == E_LET)  return Expr(new Let(vec, new Apply(var, expr)));
            return Expr(new Letrec(vec, new Apply(var, expr)));
        }
        jump:
        Expr body = stxs[2].parse(env);
        if (eptype == E_LET)  return Expr(new Let(vec, body));
        return Expr(new Letrec(vec, body));
    }
    vector<Expr> expr;
    for (int i = 1; i < stxs.size(); ++i) {
        expr.push_back(stxs[i]->parse(env));
    }
    //std::cout << "apply" << std::endl;
    return Expr(new Apply(stxs[0]->parse(env), expr));
}

#endif