#include "Def.hpp"
#include "expr.hpp"
#include "value.hpp"
#include <vector>

std :: map<std :: string, ExprType> primitives;
std :: map<std :: string, ExprType> reserved_words;

void initPrimitives()
{
    // primitives stores all procedures in library, mapping them to ExprTypes
    primitives["*"] = E_MUL;
    primitives["-"] = E_MINUS;
    primitives["+"] = E_PLUS;
    primitives["<"] = E_LT;
    primitives["<="] = E_LE;
    primitives["="] = E_EQ;
    primitives[">="] = E_GE;
    primitives[">"] = E_GT;
    primitives["void"] = E_VOID;
    primitives["eq?"] = E_EQQ;
    primitives["boolean?"] = E_BOOLQ;
    primitives["fixnum?"] = E_INTQ;
    primitives["null?"] = E_NULLQ;
    primitives["pair?"] = E_PAIRQ;
    primitives["procedure?"] = E_PROCQ;
    primitives["symbol?"] = E_SYMBOLQ;
    primitives["cons"] = E_CONS;
    primitives["not"] = E_NOT;
    primitives["car"] = E_CAR;
    primitives["cdr"] = E_CDR;
    primitives["exit"] = E_EXIT;
}

void initReservedWords()
{
    // reserved_words stores all reserved words, mapping them to bools
    reserved_words["let"] = E_LET;
    reserved_words["lambda"] = E_LAMBDA;
    reserved_words["letrec"] = E_LETREC;
    reserved_words["if"] = E_IF;
    reserved_words["begin"] = E_BEGIN;
    reserved_words["quote"] = E_QUOTE;
}

void initfunctions(Assoc& g) {
    //primitives as variebles
    g = extend("a", NullV(), g, 0);
    g = extend("b", NullV(), g, 0);
    std::vector<std::string> bi = {"a", "b"};
    std::vector<std::string> un = {"a"};
    for (const auto& i : primitives) {
        switch (i.second) {
            case E_MUL:
                g = extend(i.first, ClosureV(bi, Expr(new Mult(new Var("a"), new Var("b"))), g), g, 1);
                break;
            case E_MINUS: 
                g = extend(i.first, ClosureV(bi, Expr(new Minus(new Var("a"), new Var("b"))), g), g, 1);
                break;
            case E_PLUS:
                g = extend(i.first, ClosureV(bi, Expr(new Plus(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_LT:
                g = extend(i.first, ClosureV(bi, Expr(new Less(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_LE:
                g = extend(i.first, ClosureV(bi, Expr(new LessEq(new Var("a"), new Var("b"))), g), g, 1);
                break;
            case E_EQ:
                g = extend(i.first, ClosureV(bi, Expr(new Equal(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_GE:
                g = extend(i.first, ClosureV(bi, Expr(new GreaterEq(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_GT:
                g = extend(i.first, ClosureV(bi, Expr(new Greater(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_VOID:
                g = extend(i.first, ClosureV(un, Expr(new MakeVoid()), g), g, 1);
                break;
            case E_EQQ:
                g = extend(i.first, ClosureV(bi, Expr(new IsEq(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_BOOLQ:
                g = extend(i.first, ClosureV(un, Expr(new IsBoolean(new Var("a"))), g), g, 1);
                break; 
            case E_INTQ:
                g = extend(i.first, ClosureV(un, Expr(new IsFixnum(new Var("a"))), g), g, 1);
                break; 
            case E_NULLQ:
                g = extend(i.first, ClosureV(un, Expr(new IsNull(new Var("a"))), g), g, 1);
                break; 
            case E_PAIRQ:
                g = extend(i.first, ClosureV(un, Expr(new IsPair(new Var("a"))), g), g, 1);
                break; 
            case E_PROCQ:
                g = extend(i.first, ClosureV(un, Expr(new IsProcedure(new Var("a"))), g), g, 1);
                break; 
            case E_SYMBOLQ: 
                g = extend(i.first, ClosureV(un, Expr(new IsSymbol(new Var("a"))), g), g, 1);
                break;
            case E_CONS:
                g = extend(i.first, ClosureV(bi, Expr(new Cons(new Var("a"), new Var("b"))), g), g, 1);
                break; 
            case E_NOT:
                g = extend(i.first, ClosureV(un, Expr(new Not(new Var("a"))), g), g, 1);
                break; 
            case E_CAR:
                g = extend(i.first, ClosureV(un, Expr(new Car(new Var("a"))), g), g, 1);
                break; 
            case E_CDR:
                g = extend(i.first, ClosureV(un, Expr(new Cdr(new Var("a"))), g), g, 1);
                break; 
            case E_EXIT:
                g = extend(i.first, ClosureV(un, Expr(new Exit()), g), g, 1);
                break; 
            default: break;
        }
    }
}
