#include <tile_o_tron_4000.h>
#include <catch.hpp>
#include <exception>
#include <sstream>

using namespace L3;



void Tile_O_Tron_4000::visit(Program* item)     {
     std::stringstream ss;
     ss << "At " << ":" __FILE__ << __LINE__
        << "You tried to tile a program";
     throw std::logic_error(ss.str());
}

void Tile_O_Tron_4000::visit(Function* item)    {
     for(auto instruction : item->instructions){
          instruction->accept(*this);
     }
}
void Tile_O_Tron_4000::visit(Assignment* item)  {
     
}
void Tile_O_Tron_4000::visit(Binop* item)       {
}
void Tile_O_Tron_4000::visit(Load* item)        {
}
void Tile_O_Tron_4000::visit(Store* item)       {
}
void Tile_O_Tron_4000::visit(Goto* item)        {
}
void Tile_O_Tron_4000::visit(Cjump* item)       {
}
void Tile_O_Tron_4000::visit(Call* item)        {

}
void Tile_O_Tron_4000::visit(Val_Return* item)  {

}
void Tile_O_Tron_4000::visit(Void_Return* item) {

}

void Tile_O_Tron_4000::visit(Var* item)         {

}
void Tile_O_Tron_4000::visit(Label* item)       {

}
void Tile_O_Tron_4000::visit(Int_Literal* item) {

}
void Tile_O_Tron_4000::visit(Runtime_Fun* item) {

}

TEST_CASE("Does the magical variadic caster work?"){
     ast_ptr var_ptr{new Var("V1")};

     ast_ptr int_ptr{new Int_Literal(12)};

     ast_ptr rt_fun_ptr{new Runtime_Fun(Runtime_Fun::print)};

     SECTION("one type"){
          REQUIRE(is_one_of<Var>(var_ptr));
     }

     SECTION("Multiple types"){
          REQUIRE((is_one_of<Var,Call,Runtime_Fun>(var_ptr)));
     }

     SECTION("is callable"){
          REQUIRE(!is_callable(int_ptr));
     }

     SECTION("runtime callable"){
          REQUIRE(is_callable(rt_fun_ptr));
     }
}
