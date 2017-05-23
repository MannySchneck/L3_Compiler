#pragma once

#include <L3.h>
#include <tiles.h>

namespace L3{

     template<typename T>
     bool is_one_of(L3_ptr<AST_Item> item){
          return dynamic_cast<T*>(item.get());
     }

     template <typename T, typename T2, typename... Args>
     bool is_one_of(L3_ptr<AST_Item> item){
          return dynamic_cast<T*>(item.get()) ||
               is_one_of<T2, Args...>(item);
     }

     inline
     bool is_t(ast_ptr item){
          return is_one_of<Var, Int_Literal>(item);
     }

     inline
     bool is_s(ast_ptr item){
          return is_one_of<Label>(item) || is_t(item);
     }

     inline
     bool is_callable(ast_ptr item){
          return is_one_of<Runtime_Fun, Var, Label>(item);
     }

     inline
     bool is_store(ast_ptr item){
          return is_one_of<Store>(item);
     }

     inline
     bool is_load(ast_ptr item){
          return is_one_of<Load>(item);
     }

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

          std::vector<Tile::Tile> possible_tiles;

          std::vector<Tile::Tile> tiled_floor;
     };
}

