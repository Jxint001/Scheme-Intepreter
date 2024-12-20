#include "Def.hpp"
#include "syntax.hpp"
#include "expr.hpp"
#include "value.hpp"
#include "RE.hpp"
#include <sstream>
#include <iostream>
#include <map>


extern std :: map<std :: string, ExprType> primitives;
extern std :: map<std :: string, ExprType> reserved_words;

void REPL()
{
    // read - evaluation - print loop
    Assoc global_env = empty();
    while (1)
    {
        #ifndef ONLINE_JUDGE
            std::cout << "scm> ";
        #endif
        Syntax stx = readSyntax(std :: cin); // read
        try
        {
            Expr expr = stx -> parse(global_env); // parse
            //  stx -> show(std :: cout); // syntax print
            //  std::cout << std::endl;
            Value val = expr -> eval(global_env);
            if (val -> v_type == V_TERMINATE) {
                //std::cout << "scan terminate" << std::endl;
                break;
            }
            val -> show(std :: cout); // value print
        }
        catch (const RuntimeError &RE)
        {
            std :: cout << RE.message();
            //std :: cout << "RuntimeError";
        }
        puts("");
    }
    // std::cout << "things in assoclist" << std::endl;
    // for (auto i = global_env; i.get() != nullptr; i = i->next) {
    //     std::cout << i->x << " ";
    //     (i->v)->show(std::cout);
    //     std::cout << std::endl;
    // }
}


int main(int argc, char *argv[]) {
    initPrimitives();
    initReservedWords();
    REPL();
    return 0;
}
