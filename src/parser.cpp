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
    if ((find(s, env)).get() != nullptr) {
        return Expr(new Var(s));
    }
    env = extend(s, Value(nullptr), env);
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
    // std::cout << "list size: " << stxs.size() << std::endl;
    // for (const auto& stx : stxs) {
    //     stx->show(std::cout);
    //     std::cout << " ";
    // }
    //std::cout << std::endl;
    if (stxs.empty()) {
        return Expr(new MakeVoid());
       //throw RuntimeError("nothing here"); 
    }
    Identifier* id = dynamic_cast<Identifier*>(stxs[0].get());
    ExprType eptype = stxs[0]->get_type();
    //Assoc env1 = env;
    //Expr front = stxs[0]->parse(env1);
    string w_num = "incorrect number of parameterssss";


    if(id != nullptr){
        bool usedf=0;
        for(auto i = env; i.get() != nullptr; i = i->next)
            if(i->x == id->s) { usedf = 1;  break; }
        if(usedf){
            // std::cout<<"used function "<< id->s << std::endl;
            // std::cout << "syxs size" << stxs.size() << std::endl;
            vector<Expr> expr;
            for(int i = 1; i < stxs.size(); ++i) {
                Assoc env1 = env;
                expr.push_back(stxs[i]->parse(env1));
            } 
            Assoc env1 = env;
            return Expr(new Apply(stxs[0]->parse(env1), expr));
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
        //std::cout << "in cons" << std::endl;
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
        //std::cout << "pair" << std::endl;
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
        //std::cout << "llllllam" << std::endl;
        if (stxs.size() != 3) { //std::cout << "hey" << std::endl;
        throw RuntimeError(w_num); }
        if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("invalid type"); }

        Assoc e = env;
        List* paras = dynamic_cast<List*>(stxs[1].get());
        vector<string> xs;
        for (int i = 0; i < paras->stxs.size(); ++i) {
            Identifier* id = dynamic_cast<Identifier*>(paras->stxs[i].get());
            if (id == nullptr) { throw RuntimeError("invalid type 1"); }
            xs.push_back(id->s); 
            e = extend(id->s, Value(nullptr), e);
        }
        return Expr(new Lambda(xs, stxs[2]->parse(e)));
    }
    if (eptype == E_LET || eptype == E_LETREC) {
        if (stxs.size() != 3) { throw RuntimeError(w_num); }

        if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
        List* paras = dynamic_cast<List*>(stxs[1].get());
        vector< pair<string, Expr>> vec;

        Assoc e = env;
        for (int i = 0; i < paras->stxs.size(); ++i) {
            if (paras->stxs[i]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
            List* var = dynamic_cast<List*>(paras->stxs[i].get());
            if (var->stxs.size() != 2) { throw RuntimeError(w_num); }
            //a var
            Identifier* name = dynamic_cast<Identifier*>(var->stxs[0].get());
            if (name == nullptr) { //std::cout << paras->stxs[0]->get_type() << std::endl; 
            throw RuntimeError("Not A Var"); }
            Assoc env2 = env;
            Expr expr = var->stxs[1]->parse(env2);
            vec.push_back(mp(name->s, expr));
            e = extend(name->s, Value(nullptr), e);
        }
        Expr body = stxs[2]->parse(e);
        if (eptype == E_LET) { return Expr(new Let(vec, body)); }
        return Expr(new Letrec(vec, body));
    }
    //可以像上面一样一块写
    // if (eptype == E_LETREC) {
    //     if (stxs.size() != 3) { 
    //         return Expr(static_cast<ExprBase *>(new Quote(new List())));
    //         //throw RuntimeError(w_num); 
    //     }
    //     if (stxs[1]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
    //     List* paras = dynamic_cast<List*>(stxs[1].get());
    //     vector< pair<string, Expr>> vec;
    //     Assoc e = env;
    //     for (int i = 0; i < paras->stxs.size(); ++i) {
    //         if (paras->stxs[i]->get_type() != E_LIST) { throw RuntimeError("Not A List"); }
    //         List* var = dynamic_cast<List*>(paras->stxs[i].get());
    //         if (var->stxs.size() != 2) { throw RuntimeError(w_num); }
    //         //a var
    //         Identifier* name = dynamic_cast<Identifier*>(var->stxs[0].get());
    //         if (name == nullptr) { throw RuntimeError("Not A Var"); }
    //         e = extend(name->s, Value(nullptr), e);
    //     }
    //     for (int i = 0; i < paras->stxs.size(); ++i) {
    //         List* var = dynamic_cast<List*>(paras->stxs[i].get());
    //         Identifier* name = dynamic_cast<Identifier*>(var->stxs[0].get());
    //         Assoc env2 = e;
    //         Expr expr = var->stxs[1]->parse(env2);
    //         vec.push_back(mp(name->s, expr));
    //         //env = extend(name->s, expr->eval(e), env);
    //     }
    //     //std::cout << stxs[2]->get_type() << std::endl;
    //     Expr body = stxs[2]->parse(e);
        
    //     return Expr(new Letrec(vec, body));
    // }

    vector<Expr> expr;
    for (int i = 1; i < stxs.size(); ++i) {
        expr.push_back(stxs[i]->parse(env));
    }
    return Expr(new Apply(stxs[0]->parse(env), expr));
}

#endif