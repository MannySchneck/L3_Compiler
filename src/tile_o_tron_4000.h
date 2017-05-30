#pragma once

#include <L3.h>
#include <ostream>

namespace Tile{
        class Tile;
}

namespace L3{

        class Tile_O_Tron_4000 :
                public AST_Item_Visitor{
        public:
                void visit(Program* item)     override;
                void visit(Function* item)    override;
                void visit(Assignment* item)  override;
                void visit(Binop* item)       override;
                void visit(Load* item)        override;
                void visit(Store* item)       override;
                void visit(Goto* item)        override;
                void visit(Cjump* item)       override;
                void visit(Call* item)        override;
                void visit(Val_Return* item)  override;
                void visit(Void_Return* item) override;

                void visit(Var* item)         override;
                void visit(Label* item)       override;
                void visit(Int_Literal* item) override;
                void visit(Runtime_Fun* item) override;

                void print_l2_fun(std::ostream out);

                std::vector<std::shared_ptr<Tile::Tile>> possible_tiles;

                std::vector<std::shared_ptr<Tile::Tile>> tiled_floor;
        };
}
