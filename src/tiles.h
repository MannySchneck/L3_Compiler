#pragma once

#include <L3.h>

#include <boost/optional/optional.hpp>

namespace L3{
namespace Tile{


struct Tile{
     const static int64_t cost = 1;


     using tile_ptr = std::shared_ptr<Tile>;

     std::vector<std::shared_ptr<Tile>> children;
     virtual std::string to_L2() = 0;
};


using tile_ptr = Tile::tile_ptr;

// Atomic tiles
// rhs <- lhs
struct Atom_Assignment:
          public Tile{

     Atom_Assignment(L3_ptr<L3::AST_Item> lhs,
                     L3_ptr<L3::AST_Item> rhs_atom);

     const static int size = 3;
     ast_ptr lhs;
     L3_ptr<L3::AST_Item> rhs_atom; // Must be: Var | Label | Int Literal

     std::string to_L2() override;

};

struct Load_Assignment:
          public Tile{

     Load_Assignment(L3_ptr<L3::AST_Item> lhs,
                     L3_ptr<L3::AST_Item> rhs);

     const static int size = 4;

     ast_ptr  lhs;
     ast_ptr  rhs; // must be load!


     std::string to_L2() override;
};

struct Store_Assignment:
          public Tile{
     Store_Assignment(L3::ast_ptr lhs,
                      L3::ast_ptr rhs);
     const static int size = 4;
     ast_ptr lhs; // must be load!
     ast_ptr rhs;


     std::string to_L2() override;
};

struct Binop_Assignment
     : public Tile {

     Binop_Assignment(L3::ast_ptr lhs, L3::ast_ptr rhs);
     const static int size = 5;

     L3_ptr<L3::AST_Item> rhs;
     L3_ptr<L3::AST_Item> lhs;

     std::string to_L2() override;
};

struct Call_Assignment
     : public Tile {

     Call_Assignment(L3::ast_ptr lhs,
                     L3::ast_ptr rhs,
                     std::function<std::string()> name_gen);

     const static int size = 5;

     L3_ptr<L3::AST_Item> rhs;
     L3_ptr<L3::AST_Item> lhs;

     std::string to_L2() override;
};


struct Goto
     : public Tile{

     Goto(ast_ptr target);

     const static int size = 1;
     L3_ptr<L3::AST_Item> target;

     std::string to_L2() override;
};

struct Cjump
     : public Tile{
     Cjump(ast_ptr cmp_result, ast_ptr t_target, ast_ptr f_target);

     const static int size = 4;

     ast_ptr cmp_result;
     ast_ptr t_target;
     ast_ptr f_target;

     std::string to_L2() override;
};

struct Val_Return
     : public Tile{
     Val_Return(ast_ptr result);
     L3_ptr<AST_Item> result;
     std::string to_L2() override;
};

struct Void_Return
     : public Tile{
     Void_Return();
     std::string to_L2() override;
};

struct Call
     : public Tile{
     Call(ast_ptr target, std::vector<ast_ptr> args, L3::Label retlab);

     const static int size = 1; // depends on the size doesn't it?

     L3_ptr<AST_Item> target;
     std::vector<L3_ptr<L3::AST_Item>> args;
     L3::Label retlab;

     std::string to_L2() override;
};

// Atom tiles :
struct Var
     : public Tile{
};

struct Label
     : public Tile{

     Label(ast_ptr lab);

     std::string to_L2() override;

     ast_ptr lab;
};

struct Int_Literal
     : public Tile{

};

struct Runtime_Fun
     : public Tile{

};



tile_ptr match_me_bro(L3::ast_ptr item, std::function<std::string()> name_gen);

}
}
