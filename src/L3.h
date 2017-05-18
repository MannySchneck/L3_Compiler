#pragma once

#include <stdint.h>
#include <string>
#include <vector>
#include <utils.h>
#include <sstream>

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
        struct Return;

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
                virtual void visit(Return* item)        = 0;

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
                void visit(Return* item)        override;

                void visit(Var* item)         override;
                void visit(Label* item)       override;
                void visit(Int_Literal* item) override;
                void visit(Runtime_Fun* item) override;

                std::stringstream result;
        };


        struct AST_Item{
                virtual void accept(AST_Item_Visitor &v) = 0;
        };


/*
  An instruction is any internal node in the program tree.
 */
///////////////////////////////////////////////////////////////////////////////
//                            Structural elements                            //
///////////////////////////////////////////////////////////////////////////////
        struct Instruction :
                public AST_Item{

                virtual ~Instruction();
        };


        struct Function :
                public AST_Item{

                Label name;
                std::vector<Var> vars;
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


///////////////////////////////////////////////////////////////////////////////
//                                Instructions                               //
///////////////////////////////////////////////////////////////////////////////

        struct Assignment :
                public Instruction{
                void accept(AST_Item_Visitor &v) override;

                std::vector<AST_Item> operands;

        };


        struct Goto :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };

        struct Cjump :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };

        struct Call :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };

        struct Return :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };


        // Internal instructions (internal nodes)
        struct Binop :
                public Instruction{


                void accept(AST_Item_Visitor &v) override;
        };

        struct Load :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };

        struct Store :
                public Instruction{

                void accept(AST_Item_Visitor &v) override;
        };


///////////////////////////////////////////////////////////////////////////////
//                                   Atoms                                    //
///////////////////////////////////////////////////////////////////////////////
        struct Var :
                public AST_Item{

                explicit Var(std::string name);

                std::string name;
                void accept(AST_Item_Visitor &v) override;
        };

        struct Label :
                public AST_Item{

                explicit Label(std::string name);

                std::string name;
                void accept(AST_Item_Visitor &v) override;
        };

        struct Int_Literal :
                public AST_Item{

                explicit Int_Literal(int64_t val);

                int64_t val;
                void accept(AST_Item_Visitor &v) override;
        };

        struct Runtime_Fun:
                public AST_Item{

                enum Funs{
                        Call,
                        Allocate,
                        Array_Error
                };

                explicit Runtime_Fun(Funs fun);

                Funs fun;
                void accept(AST_Item_Visitor &v) override;
        };
}
