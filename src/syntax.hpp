#ifndef SYNTAX 
#define SYNTAX

#include <cstring>
#include <vector>
#include "Def.hpp"
#include "shared.hpp"

struct SyntaxBase {
  virtual Expr parse(Assoc &) = 0;
  virtual void show(std::ostream &) = 0;
  virtual ExprType get_type() = 0;
  virtual ~SyntaxBase() = default;
};

struct Syntax {
    SharedPtr<SyntaxBase> ptr;
    // std :: shared_ptr<SyntaxBase> ptr;
    Syntax(SyntaxBase *);
    SyntaxBase* operator -> () const;
    SyntaxBase& operator * ();
    SyntaxBase* get() const;
    Expr parse(Assoc &);
};

struct Number : SyntaxBase {
    int n;
    Number(int);
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual ExprType get_type() override;
};

struct TrueSyntax : SyntaxBase {
    // TrueSyntax();
    virtual Expr parse(Assoc &) override;
    virtual void show(std :: ostream &) override;
    virtual ExprType get_type() override;
};

struct FalseSyntax : SyntaxBase {
    // FalseSyntax();
    virtual Expr parse(Assoc &) override;
    virtual void show(std :: ostream &) override;
    virtual ExprType get_type() override;
};

struct Identifier : SyntaxBase {
    std::string s;
    Identifier(const std::string &);
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual ExprType get_type() override;
};

struct List : SyntaxBase {
    std :: vector<Syntax> stxs;
    List();
    virtual Expr parse(Assoc &) override;
    virtual void show(std::ostream &) override;
    virtual ExprType get_type() override;
};

Syntax readSyntax(std::istream &);

std::istream &operator>>(std::istream &, Syntax);
#endif