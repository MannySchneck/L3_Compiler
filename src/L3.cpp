#include <L3.h>
#include <sstream>
#include <cassert>
#include <exception>

using namespace L3;


void Dump::visit(Program* item){
     for(auto it = item->functions.begin();
         it != item->functions.end();
         it++){
          (*it)->accept(*this);
          if(it != item->functions.end() - 1)
               result << "\n\n";
        }
     result << "\n";
}

void Dump::visit(Function* item){
        result << "define";
        result << " ";
        item->name.accept(*this);
        result << "(";
        for(auto it =  item->params.begin(); it != item->params.end(); it++){
                if(it != item->params.begin()){
                        result << ", ";
                }
                it->accept(*this);
        }
        result << ")";
        result << "{\n  ";
        for(auto it = item->instructions.begin(); it != item->instructions.end(); it++){
                (*it)->accept(*this);
                if(it != item->instructions.end() - 1){
                        result << "\n  ";
                }
        }
        result << "\n}";
}

void Dump::visit(Assignment* item){
     item->get_lhs()->accept(*this);
     result << " <- ";
     item->get_rhs()->accept(*this);
}
void Dump::visit(Binop* item){
     item->get_lhs()->accept(*this);
     result << " " << item->dump_op(item->op) << " ";
     item->get_rhs()->accept(*this);
}
void Dump::visit(Load* item){
     result << "load" << " ";
     item->get_loadee()->accept(*this);
}
void Dump::visit(Store* item){
     result << "store" << " ";
     item->get_storee()->accept(*this);
}
void Dump::visit(Goto* item){
     result << "br" << " ";
     item->get_target()->accept(*this);
}
void Dump::visit(Cjump* item){
     result << "br" << " ";
     item->get_cond()->accept(*this);
     result << " ";
     item->get_true_target()->accept(*this);
     result << " ";
     item->get_false_target()->accept(*this);
}
void Dump::visit(Call* item){

     result << "call";
     result << " ";

     item->get_callee()->accept(*this);

     auto args = item->get_args();
     result << "(";
     for(auto it =  args.begin(); it != args.end(); it++){
          if(it != args.begin()){
               result << ", ";
          }
          (*it)->accept(*this);
     }
     result << ")";
}

void Dump::visit(Void_Return* item){
     result << "return";
}

void Dump::visit(Val_Return* item){
     result << "return";
     result << " ";
     item->get_result()->accept(*this);
}


void Dump::visit(Var* item){
     result << item->name;
}
void Dump::visit(Label* item){
     result << item->name;
}
void Dump::visit(Int_Literal* item){
     result << item->val;
}
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
void Void_Return ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Val_Return  ::accept(AST_Item_Visitor &v) { v.visit(this); }

void Var         ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Label       ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Int_Literal ::accept(AST_Item_Visitor &v) { v.visit(this); }
void Runtime_Fun ::accept(AST_Item_Visitor &v) { v.visit(this); }
void STAHP::accept(AST_Item_Visitor &v){
     throw std::logic_error(std::string("look this class shouldn't exist, but you're a bad programmer, so.... idk what the moral here is. Don't be you? Yeah, that'd be great. I recommend doing that.\n"  " meanwhile: ") + static_cast<Dump&>(v).result.str());
}

// instructions

Instruction::Instruction(std::vector<L3_ptr<AST_Item>> operands) :
     operands(operands)
{}

Instruction::Instruction() = default;

Instruction::~Instruction() = default;


Val_Return::Val_Return(L3_ptr<AST_Item> result) :
     Instruction({result}) // ew
{}

L3_ptr<AST_Item>  Val_Return::get_result(){
     assert(operands.size() == 1);
     return operands[0];
}

Load::Load(ast_ptr loadee) :
     Instruction({loadee})
{}

ast_ptr Load::get_loadee(){
     assert(operands.size() == 1);
     return operands[0];
}

Store::Store(ast_ptr storee) :
     Instruction({storee})
{}

ast_ptr Store::get_storee(){
     assert(operands.size() == 1);
     return operands[0];
}

Assignment::Assignment(ast_ptr lhs, ast_ptr rhs) :
     Instruction({lhs, rhs})
{}

ast_ptr Assignment::get_rhs(){
     assert(operands.size() == 2);
     return operands[1];
}
ast_ptr Assignment::get_lhs(){
     assert(operands.size() == 2);
     return operands[0];
}

std::vector<ast_ptr> vector_concat(std::vector<ast_ptr> a, std::vector<ast_ptr> b){
     a.reserve(a.size() + b.size());
     a.insert(a.end(), b.begin(), b.end());
     return a;
}

Call::Call(std::vector<ast_ptr> everything) :
     Instruction(everything),
     numargs(everything.size() - 1)
{}

ast_ptr Call::get_callee(){
     assert(operands.size() == numargs + 1);
     return operands[0];
}

std::vector<ast_ptr> Call::get_args(){
     std::vector<ast_ptr> args;
     args.reserve(numargs);
     args.insert(args.end(), operands.begin() + 1, operands.end());
     assert(args.size() == numargs);
     return args;
}

Cjump::Cjump(L3_ptr<Var> cond,
             L3_ptr<Label> t_target,
             L3_ptr<Label> f_target) :
     Instruction({cond, t_target, f_target})
{}

ast_ptr Cjump::get_cond(){
     assert(operands.size() == 3);
     return operands[0];
}

ast_ptr Cjump::get_true_target(){
     assert(operands.size() == 3);
     return operands[1];
}

ast_ptr Cjump::get_false_target(){
     assert(operands.size() == 3);
     return operands[2];
}


Goto::Goto(L3_ptr<Label> target) :
     Instruction({target})
{}

ast_ptr Goto::get_target(){
     assert(operands.size() == 1);
     return operands[0];
}

Binop::Binop(Op op, ast_ptr lhs, ast_ptr rhs) :
     Instruction({lhs, rhs}),
     op(op)
{}

std::string Binop::dump_op(Op op){
     switch(op){
     case(plus):
          return "+";
          break;
     case(mult):
          return "*";
          break;
     case(minus):
          return "-";
          break;
     case(and_):
          return "&";
          break;
     case(left_shift):
          return "<<";
          break;
     case(right_shift):
          return ">>";
          break;
     case(le):
          return "<";
          break;
     case(leq):
          return "<=";
          break;
     case(eq):
          return "=";
          break;
     case(ge):
          return ">";
          break;
     case(geq):
          return ">=";
          break;
     default:
          throw std::logic_error("That's not an operator dude\n");
     }
}

ast_ptr Binop::get_rhs(){
     assert(operands.size() == 2);
     return operands[1];
}
ast_ptr Binop::get_lhs(){
     assert(operands.size() == 2);
     return operands[0];
}


Label::Label(std::string name) :
     name(name)
{}

Int_Literal::Int_Literal(int64_t val) :
     val(val)
{}

Runtime_Fun::Runtime_Fun(Runtime_Fun::Fun fun) :
     fun(fun)
{}

Var::Var(std::string name) :
     name(name)
{}

Function::Function(Label name) :
     name(name)
{}
