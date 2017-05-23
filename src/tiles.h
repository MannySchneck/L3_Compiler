#pragma once

#include <L3.h>

namespace L3{
    namespace Tile{

        struct Tile{
            const static int64_t cost = 1;
            std::vector<Tile> children;
        };

// Atomic tiles
        // rhs <- lhs
        struct Atom_Assignment:
            public Tile{
            const static int size = 3;
            L3::Var lhs;
            L3_ptr<L3::AST_Item> rhs_atom; // Must be: Var | Label | Int Literal

        };

        struct Load_Assignment:
            public Tile{
            const static int size = 4;
            L3::Var lhs;
            L3::Load load; // must be load!
        };

        struct Store_Assignment:
            public Tile{
            const static int size = 4;
            L3::Var lhs;
            L3::Load load; // must be load!
        };

        struct Binop_Assignment
            : public Tile {
            L3::Binop::Op op;
            const static int size = 5;
            L3_ptr<L3::AST_Item> rhs;
            L3_ptr<L3::AST_Item> lhs;
        };

        struct Goto
            : public Tile{
            const static int size = 1;
            L3_ptr<L3::Label> target;
        };

        struct Cjump
            : public Tile{
            const static int size = 4;
            L3_ptr<Var> cmp_result;
            L3_ptr<L3::Label> t_target;
            L3_ptr<L3::Label> f_target;
        };

        struct Call
            : public Tile{
            const static int size = 1; // depends on the size doesn't it?
            L3_ptr<AST_Item> target;
            std::vector<L3_ptr<L3::AST_Item>> args;
        };

        struct Val_Return
            : public Tile{
            L3_ptr<AST_Item> result;
        };

        struct Void_Return
            : public Tile{
        };

// Atom tiles :
        struct Var
            : public Tile{
        };

        struct Label
            : public Tile{

        };

        struct Int_Literal
            : public Tile{

        };

        struct Runtime_Fun
            : public Tile{

        };
    }
}
