#pragma once

#include <L3.h>

namespace L3{
    namespace Tiles{
        struct Tile{
            const static cost = 1;
        };

// Atomic tiles
        // rhs <- lhs
        struct Assignment:
            public Tile{
            
        };

        // rhs op lhs
        struct Binop
            : public Tile {

        };

        struct Load
            : public Tile{

        };

        struct Store
                                : public Tile{

        };

        struct Goto
                                : public Tile {

        };

        struct Cjump
            : public Tile{

        };

        struct Call
            : public Tile{

        };

        struct Val_Return
            : public Tile{

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
