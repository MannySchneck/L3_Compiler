#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <utils.h>
#include <sstream>
#include <boost/optional/optional.hpp>

namespace L3{


///////////////////////////////////////////////////////////////////////////////
//                            Forward Declarations                           //
///////////////////////////////////////////////////////////////////////////////
     struct Program;
     struct Function;
     struct Assignment;
     struct Binop;
     struct Load;
     struct Store;
     struct Goto;
     struct Cjump;
     struct Call;
     struct Val_Return;
     struct Void_Return;

     struct Var;
     struct Label;
     struct Int_Literal;
     struct Runtime_Fun;


///////////////////////////////////////////////////////////////////////////////
//                       Visitors and base-type things                       //
///////////////////////////////////////////////////////////////////////////////
     class AST_Item_Visitor{
     public:
          virtual void visit(Program* item)     = 0;
          virtual void visit(Function* item)    = 0;
          virtual void visit(Assignment* item)  = 0;
          virtual void visit(Binop* item)       = 0;
          virtual void visit(Load* item)        = 0;
          virtual void visit(Store* item)       = 0;
          virtual void visit(Goto* item)        = 0;
          virtual void visit(Cjump* item)       = 0;
          virtual void visit(Call* item)        = 0;
          virtual void visit(Void_Return* item)        = 0;
          virtual void visit(Val_Return* item)        = 0;

          virtual void visit(Var* item)         = 0;
          virtual void visit(Label* item)       = 0;
          virtual void visit(Int_Literal* item) = 0;
          virtual void visit(Runtime_Fun* item) = 0;
     };

     class Dump :
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
          void visit(Val_Return* item)        override;
          void visit(Void_Return* item)        override;

          void visit(Var* item)         override;
          void visit(Label* item)       override;
          void visit(Int_Literal* item) override;
          void visit(Runtime_Fun* item) override;

          std::stringstream result;
     };


     struct AST_Item{
          virtual void accept(AST_Item_Visitor &v) = 0;
     };


     using ast_ptr = L3_ptr<AST_Item>;

///////////////////////////////////////////////////////////////////////////////
//                                Instructions                               //
///////////////////////////////////////////////////////////////////////////////

/*
  An instruction is any internal node in the program tree.
*/
     struct Instruction :
          public virtual AST_Item{

          Instruction();
          Instruction(std::vector<L3_ptr<AST_Item>> operands);

          const std::vector<L3_ptr<AST_Item>> operands;

          virtual ~Instruction();
     };



     struct Assignment :
          public Instruction{

          Assignment(ast_ptr lhs, ast_ptr rhs);

          ast_ptr get_rhs();
          ast_ptr get_lhs();

          void accept(AST_Item_Visitor &v) override;
     };


     struct Goto :
          public Instruction{

          Goto(L3_ptr<Label> target);

          ast_ptr get_target();

          void accept(AST_Item_Visitor &v) override;
     };

     struct Cjump :
          public Instruction{

          Cjump(L3_ptr<Var> cond,
                L3_ptr<Label> t_target,
                L3_ptr<Label> f_target);

          ast_ptr get_cond();
          ast_ptr get_true_target();
          ast_ptr get_false_target();

          void accept(AST_Item_Visitor &v) override;
     };

     struct Call :
          public Instruction{

          Call(std::vector<ast_ptr> everything);

          ast_ptr get_callee();

          std::vector<ast_ptr> get_args();

          int64_t numargs;

          void accept(AST_Item_Visitor &v) override;
     };

     struct Val_Return :
          public Instruction{

          Val_Return(L3_ptr<AST_Item> result);

          L3_ptr<AST_Item> get_result();

          void accept(AST_Item_Visitor &v) override;
     };

     struct Void_Return :
          public Instruction{

          void accept(AST_Item_Visitor &v) override;
     };




     // Internal instructions (internal nodes)
     struct Binop :
          public Instruction{

          enum Op{
               plus,
               mult,
               minus,
               and_,
               left_shift,
               right_shift,
               //
               le,
               leq,
               eq,
               ge,
               geq
          };

          Binop(Op op, ast_ptr lhs, ast_ptr rhs);

          ast_ptr get_rhs();
          ast_ptr get_lhs();

          Op op;

          std::string dump_op(Op op);

          void accept(AST_Item_Visitor &v) override;
     };

     struct Load :
          public Instruction{
          Load(ast_ptr loadee);

          ast_ptr get_loadee();

          void accept(AST_Item_Visitor &v) override;
     };

     struct Store :
          public Instruction{
          Store(ast_ptr storee);

          ast_ptr get_storee();

          void accept(AST_Item_Visitor &v) override;
     };


///////////////////////////////////////////////////////////////////////////////
//                                   Atoms                                    //
///////////////////////////////////////////////////////////////////////////////
     struct Atom :
          public virtual AST_Item{
     };

     struct Var :
          public Atom{

          explicit Var(std::string name);

          std::string name;
          void accept(AST_Item_Visitor &v) override;
     };

     struct Label :
          public Atom,
          public Instruction{

          explicit Label(std::string name);

          std::string name;
          void accept(AST_Item_Visitor &v) override;
     };

     struct Int_Literal :
          public Atom{

          explicit Int_Literal(int64_t val);

          int64_t val;
          void accept(AST_Item_Visitor &v) override;
     };

     struct Runtime_Fun:
          public Atom{

          enum Fun{
               print,
               allocate,
               array_error
          };

          explicit Runtime_Fun(Fun fun);

          Fun fun;
          void accept(AST_Item_Visitor &v) override;
     };

///////////////////////////////////////////////////////////////////////////////
//                            Structural elements                            //
///////////////////////////////////////////////////////////////////////////////
     struct Function :
          public AST_Item{

          explicit Function(Label name);

          Label name;
          std::vector<Var> params;
          std::vector<L3_ptr<Instruction>> instructions;

          void accept(AST_Item_Visitor &v) override;
     };

     struct Program :
          public AST_Item{

          using Functions_t = std::vector<L3_ptr<Function>>;

          Program() = default;
          explicit Program(Functions_t functions);

          std::vector<L3_ptr<Function>> functions;


          void accept(AST_Item_Visitor &v) override;
     };

     // Phony instructions
     struct STAHP :
          public AST_Item{
          void accept(AST_Item_Visitor &v) override;
     };
}
