#include "Def.hpp"
#include "value.hpp"
#include "expr.hpp"
#include "RE.hpp"
#include "syntax.hpp"
#include <cstring>
#include <vector>
#include <utility>
#include <map>

extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

Value Let::eval(Assoc &env) {
    //std::cout << body->e_type << std::endl;//+
    Assoc local = env;
    std::vector < std::pair<std::string, Value>> vec;
    for (int i = 0; i < bind.size(); ++i) {
        Assoc env1 = env;
        local = extend(bind[i].first, bind[i].second->eval(env1), local);
    }
    return body->eval(local);
} // let expression

Value Lambda::eval(Assoc &env) {
    return ClosureV(x, e, env);
} // lambda expression

Value Apply::eval(Assoc &e) {
    //std::cout << "hellll" << std::endl;
    Assoc e1 = e;
    Value cl = rator->eval(e1);
    //std::cout << rator->e_type << std::endl;
    if (cl->v_type != V_PROC) {
        throw RuntimeError("not a function");
    }
    Closure* clos = dynamic_cast<Closure*>(cl.get());
    if (clos->parameters.size() != rand.size() ){
        // std::cout << "clospara_size: " << clos->parameters.size();
        // std::cout << " rand_size: " << rand.size() << std::endl;
        throw RuntimeError("incorrect number of paras"); 
    }
    Assoc local = clos->env;
    std::vector<Value> v_to_bind;
    for (int i = 0; i < rand.size(); ++i) {
        Assoc e2 = e;
        v_to_bind.push_back(rand[i]->eval(e2));
    }
    for (int i = 0; i < clos->parameters.size(); ++i) {
        local = extend(clos->parameters[i], v_to_bind[i], local);
    }
    //clos->env = local;
    return (clos->e)->eval(local);
} // for function calling

Value Letrec::eval(Assoc &env) {
    Assoc local = env;
    for (int i = 0; i < bind.size(); ++i) {
        local = extend(bind[i].first, NullV(), local);
    }
    std::vector< std::pair<std::string, Value>> value_to_bind;
    for (int i = 0; i < bind.size(); ++i) {
        Assoc tmp = local;
        value_to_bind.push_back(std::make_pair(bind[i].first, bind[i].second->eval(tmp)));
    }
    for (int i = 0; i < value_to_bind.size(); ++i) {
        modify(value_to_bind[i].first, value_to_bind[i].second, local);
    }
    return body->eval(local);
} // letrec expression

Value Var::eval(Assoc &e) {
    Value v = find(x, e);
    if (v.get() == nullptr) { //std::cout << x << std::endl;
    throw RuntimeError("undefined var"); }
    //update(v, e);
    return v;
} // evaluation of variable

Value Fixnum::eval(Assoc &e) {
    return IntegerV(n);
} // evaluation of a fixnum

Value If::eval(Assoc &e) {
    //std::cout << "in if" << std::endl;
    Value condition = cond->eval(e);
    if (condition->v_type == V_BOOL) {
        Boolean* bo = dynamic_cast<Boolean*>(condition.get());
        if (bo->b) {
            //std::cout << "evaluating conseq" << std::endl;
            return conseq->eval(e);
        }
        return alter->eval(e);
    }
    return conseq->eval(e);
} // if expression

Value True::eval(Assoc &e) {
    return BooleanV(true);
} // evaluation of #t

Value False::eval(Assoc &e) {
    return BooleanV(false);
} // evaluation of #f

Value Begin::eval(Assoc &e) {
    for (int i = 0; i < es.size() - 1; ++i) {
        Assoc e1 = e;
        es[i]->eval(e1);
    }
    return es[es.size() - 1]->eval(e);
} // begin expression

Value Quote::eval(Assoc &e) {
    if (s->get_type() == E_TRUE) {
        return BooleanV(true);
    }
    if (s->get_type() == E_FALSE) {
        return BooleanV(false);
    }
    if (s->get_type() == E_FIXNUM) {
        Number* num = dynamic_cast<Number*>(s.get());
        return IntegerV(num->n);
    }
    if (s->get_type() == E_LIST) {
        List* list = dynamic_cast<List*>(s.get());
        if (list->stxs.empty()) {
            return NullV();
        }
        int len = list->stxs.size();
        //std::cout << len << std::endl;
        //with dot in list (whether simplified or not)
        if (len >= 3 && list->stxs[len - 2]->get_type() == E_DOT) {
            //std::cout << "hello" << std::endl;
            Value v = Quote(list->stxs[len - 1]).eval(e);
            for (int i = len - 3; i >= 0; --i) {
                v = PairV(Quote(list->stxs[i]).eval(e), v);
            }
            return v;
        }
        //without dot in list
        Value v1 = NullV();
        for (int i = len - 1; i >= 0; --i) {
            v1 = PairV(Quote(list->stxs[i]).eval(e), v1);
        }
        return v1;
    }//with()
    Identifier* id = dynamic_cast<Identifier*>(s.get());
    return SymbolV(id->s);
} // quote expression

Value MakeVoid::eval(Assoc &e) {
    return VoidV();
} // (void)

Value Exit::eval(Assoc &e) {
    return TerminateV();
} // (exit)

Value Binary::eval(Assoc &e) {
    Value first = rand1->eval(e);
    Value second = rand2->eval(e);
    // if (first->v_type == V_PROC) {
    //     Closure* clos = dynamic_cast<Closure*>(first.get());
    //     Value v1 = clos->e->eval(e);
    //     first = v1;
    //     std::cout << "hihihi" << std::endl;
    // }
    // if (second->v_type == V_PROC) {
    //     std::cout << "hihihi" << std::endl;
    //     Closure* clos = dynamic_cast<Closure*>(second.get());
    //     Value v2 = (clos->e)->eval(e);
    //     second = v2;
    // }
    //std::cout << "hey" << std::endl;

    //std::cout << "first: "; first->show(std::cout);std::cout << std::endl;
    //std::cout << "second: "; second->show(std::cout); std::cout << std::endl;
    if (e_type == E_PLUS) {
        Plus* plus = dynamic_cast<Plus*>(this);
        return plus->evalRator(first, second);
    }
    if (e_type == E_MINUS) {
        Minus* minus = dynamic_cast<Minus*>(this);
        return minus->evalRator(first, second);
    }
    if (e_type == E_MUL) {
        Mult* mult = dynamic_cast<Mult*>(this);
        return mult->evalRator(first, second);
    }
    if (e_type == E_CONS) {
        Cons* cons = dynamic_cast<Cons*>(this);
        return cons->evalRator(first, second);
    }
    if (e_type == E_LT) {
        Less* lt = dynamic_cast<Less*>(this);
        return lt->evalRator(first, second);
    }
    if (e_type == E_LE) {
        LessEq* le = dynamic_cast<LessEq*>(this);
        return le->evalRator(first, second);
    }
    if (e_type == E_EQ) {
        Equal* eq = dynamic_cast<Equal*>(this);
        return eq->evalRator(first, second);
    }
    if (e_type == E_GT) {
        Greater* gt = dynamic_cast<Greater*>(this);
        return gt->evalRator(first, second);
    }
    if (e_type == E_GE) {
        GreaterEq* ge = dynamic_cast<GreaterEq*>(this);
        return ge->evalRator(first, second);
    }
    if (e_type == E_EQQ) {
        IsEq* iseq = dynamic_cast<IsEq*>(this);
        return iseq->evalRator(first, second);
    }// eq?
} // evaluation of two-operators primitive

Value Unary::eval(Assoc &e) {
    //std::cout << "unary" << std::endl;
    Value u = rand->eval(e);
    if (e_type == E_CAR) {
        Car* car = dynamic_cast<Car*>(this);
        return car->evalRator(u);
    }// first in a pair
    if (e_type == E_CDR) {
        Cdr* cdr = dynamic_cast<Cdr*>(this);
        return cdr->evalRator(u);
    }// second in a pair
    if (e_type == E_INTQ) {
        IsFixnum* isnum = dynamic_cast<IsFixnum*>(this);
        return isnum->evalRator(u);
    }
    if (e_type == E_NULLQ) {
        IsNull* isnull = dynamic_cast<IsNull*>(this);
        return isnull->evalRator(u);
    }
    if (e_type == E_PAIRQ) {
        IsPair* ispair = dynamic_cast<IsPair*>(this);
        return ispair->evalRator(u);
    }
    if (e_type == E_SYMBOLQ) {
        IsSymbol* iss = dynamic_cast<IsSymbol*>(this);
        return iss->evalRator(u);
    }
    if (e_type == E_BOOLQ) {
        IsBoolean* isbool = dynamic_cast<IsBoolean*>(this);
        return isbool->evalRator(u);
    }
    if (e_type == E_NOT) {
        Not* no = dynamic_cast<Not*>(this);
        return no->evalRator(u);
    }
    if (e_type == E_PROCQ) {
        IsProcedure* isp = dynamic_cast<IsProcedure*>(this);
        return isp->evalRator(u);
    }
    
} // evaluation of single-operator primitive

Value Mult::evalRator(const Value &rand1, const Value &rand2) {
    //std::cout << "in mult" << std::endl;
    (exit);
    // rand1->show(std::cout); std::cout << std::endl;
    // rand2->show(std::cout);std::cout << std::endl;
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invaild type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    //std::cout << "hi" << std::endl;
    return IntegerV(first->n * second->n);
} // *

Value Plus::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invaild type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    return IntegerV(first->n + second->n);
} // +

Value Minus::evalRator(const Value &rand1, const Value &rand2) {
    // std::cout << "in minus" << std::endl;
    // (exit);
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invaild type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    //  rand1->show(std::cout); std::cout << std::endl;
    // rand2->show(std::cout);std::cout << std::endl;
    //std::cout << "hi" << std::endl;
    return IntegerV(first->n - second->n);
} // -

Value Less::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invalid type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    if (first->n < second->n) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // <

Value LessEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invalid type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    if (first->n <= second->n) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // <=

Value Equal::evalRator(const Value &rand1, const Value &rand2) {
    //std::cout << "in eq" << std::endl;
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { 
        // rand1->show(std::cout); std::cout << std::endl;
        // rand2->show(std::cout);std::cout << std::endl;
        throw RuntimeError("invalid type"); 
    }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    //std::cout << "nice" << std::endl;
    if (first->n == second->n) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // =

Value GreaterEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invalid type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    if (first->n >= second->n) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // >=

Value Greater::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != V_INT || rand2->v_type != V_INT) { throw RuntimeError("invalid type"); }
    Integer* first = dynamic_cast<Integer*>(rand1.get());
    Integer* second = dynamic_cast<Integer*>(rand2.get());
    if (first->n > second->n) {
        return BooleanV(true);
    }
    return BooleanV(false);
} // >

Value IsEq::evalRator(const Value &rand1, const Value &rand2) {
    if (rand1->v_type != rand2->v_type) { return BooleanV(false); }
    if (rand1->v_type == V_NULL || rand1->v_type == V_VOID) { return BooleanV(true); }
    if (rand1->v_type == V_INT) {
        Integer *a = dynamic_cast<Integer*>(rand1.get()), *b = dynamic_cast<Integer*>(rand2.get());
        if (a->n == b->n) { return BooleanV(true); }
        return BooleanV(false);
    }
    if (rand1->v_type == V_BOOL) {
        Boolean*a = dynamic_cast<Boolean*>(rand1.get()), *b = dynamic_cast<Boolean*>(rand2.get());
        if (a->b == b->b) { return BooleanV(true); }
        return BooleanV(false);
    }
    if (rand1->v_type == V_SYM) {
        Symbol* a = dynamic_cast<Symbol*>(rand1.get()), *b = dynamic_cast<Symbol*>(rand2.get());
        if (a->s == b->s) { return BooleanV(true); }
        return BooleanV(false);
    }
    if (rand1.get() == rand2.get()) { return BooleanV(true); }
    return BooleanV(false);
} // eq?

Value Cons::evalRator(const Value &rand1, const Value &rand2) {
    return PairV(rand1, rand2);
} // cons

Value IsBoolean::evalRator(const Value &rand) {
    if (rand->v_type != V_BOOL) { return BooleanV(false); }
    return BooleanV(true);
} // boolean?

Value IsFixnum::evalRator(const Value &rand) {
    if (rand->v_type != V_INT) { return BooleanV(false); }
    return BooleanV(true);
} // fixnum?

Value IsSymbol::evalRator(const Value &rand) {
    if (rand->v_type != V_SYM) { return BooleanV(false); }
    return BooleanV(true);
} // symbol?

Value IsNull::evalRator(const Value &rand) {
    if (rand->v_type != V_NULL) { return BooleanV(false); }
    return BooleanV(true);
} // null?

Value IsPair::evalRator(const Value &rand) {
    if (rand->v_type != V_PAIR) { return BooleanV(false); }
    return BooleanV(true);
} // pair?

Value IsProcedure::evalRator(const Value &rand) {
    if (rand->v_type != V_PROC) { return BooleanV(false); }
    return BooleanV(true);
} // procedure?

Value Not::evalRator(const Value &rand) {
    if (rand->v_type == V_BOOL) {
        Boolean* bo = dynamic_cast<Boolean*>(rand.get());
        if (bo->b) { return BooleanV(false); }
        return BooleanV(true);
    }
    return BooleanV(false);
} // not

Value Car::evalRator(const Value &rand) {
    //std::cout << "Car called with type: " << rand->v_type << std::endl;
    if (rand->v_type != V_PAIR) { 
        // rand->show(std::cout);std::cout << std::endl;
        // std::cout << rand->v_type << std::endl;
        throw RuntimeError("Not A Pair"); }
    Pair* p = dynamic_cast<Pair*>(rand.get());
    //p->car->show(std::cout);std::cout << std::endl;
    return p->car;
} // car

Value Cdr::evalRator(const Value &rand) {
    //std::cout << "Cdr called with type: " << rand->v_type << std::endl;
    if (rand->v_type != V_PAIR) { 
        
        throw RuntimeError("Not A Pair"); }
    Pair* p = dynamic_cast<Pair*>(rand.get());
    //p->cdr->show(std::cout);std::cout << std::endl;
    return p->cdr;
} // cdr
