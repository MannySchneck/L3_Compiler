#include <L3.h>
#include <sstream>

using namespace L3;

void Dump::visit(Program* item){
        std::stringstream ss;

        for(auto f : item->functions){
                f->accept(*this);
        }
}

void Dump::visit(Function* item){
        result << "define";
        result << " ";
        item->name.accept(*this);
        result << "(";
        for(auto it =  item->vars.begin(); it != item->vars.end(); it++){
                if(it != item->vars.begin()){
                        result << ", ";
                }
                it->accept(*this);
        }
        result << ")";
        result << "{\n\t";
        for(auto it = item->instructions.begin(); it != item->instructions.end(); it++){
                (*it)->accept(*this);
                result << "\n";
                if(it != item->instructions.end()){
                        result << "\t";
                }
        }
        result << "}";
}

void Dump::visit(Assignment* item){}
void Dump::visit(Binop* item){}
void Dump::visit(Load* item){}
void Dump::visit(Store* item){}
void Dump::visit(Goto* item){}
void Dump::visit(Cjump* item){}
void Dump::visit(Call* item){}
void Dump::visit(Return* item){}

void Dump::visit(Var* item){}
void Dump::visit(Label* item){}
void Dump::visit(Int_Literal* item){}
void Dump::visit(Runtime_Fun* item){}


void Program     ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Function    ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Assignment  ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Binop       ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Load        ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Store       ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Goto        ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Cjump       ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Call        ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Return      ::accept(AST_Item_Visitor &v) { v.visit(this); }

void Var         ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Label       ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Int_Literal ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Runtime_Fun ::accept(AST_Item_Visitor &v) { v.visit(this); }

Instruction::~Instruction() = default;
