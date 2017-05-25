#pragma once
#include <pegtl.hh>
#include <pegtl/trace.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>

#include <string>

#include <parse_stack.h>
#include <L3.h>

namespace L3{
     struct null_ {};

     // Text: No actions on these!
     struct seps:
          pegtl::star<
          pegtl::space
          > {};


     struct name :
          pegtl::seq<
          pegtl::plus<
               pegtl::sor<
                    pegtl::alpha,
                    pegtl::one<'_'>
                    >,
               pegtl::star<
                    pegtl::sor<
                         pegtl::alpha,
                         pegtl::one< '_' >,
                         pegtl::digit
                         >
                    >
               >
          >
     {};

     struct left_arrow :
          pegtl_string_t("<-")
     {};

     struct number:
          pegtl::seq<
          pegtl::opt<
               pegtl::sor<
                    pegtl::one< '-' >,
                    pegtl::one< '+' >
                    >
               >,
          pegtl::plus<
               pegtl::digit
               >
          >{};



     // arithmetics
     struct plus_op :
          pegtl::one<'+'>
     {};
     struct minus_op :
          pegtl::one<'-'>
     {};
     struct and__op :
          pegtl::one<'&'>
     {};
     struct mult_op :
          pegtl::one<'*'>
     {};
     struct left_shift_op :
          pegtl_string_t("<<")
     {};
     struct right_shift_op :
          pegtl_string_t(">>")
     {};

     // comparators
     struct le_op :
          pegtl_string_t("<")
     {};
     struct leq_op :
          pegtl_string_t("<=")
     {};
     struct eq_op :
          pegtl_string_t("=")
     {};
     struct ge_op :
          pegtl_string_t(">")
     {};
     struct geq_op :
          pegtl_string_t(">=")
     {};



     // Data
     struct var :
          name
     {};

     struct label :
          pegtl::if_must<
          pegtl::one<':'>,
          name
          >
     {};

     struct int_literal :
          number
     {};

     struct t :
          pegtl::sor<
          var,
          int_literal
          >
     {};

     struct s :
          pegtl::sor<
          t,
          label
          >
     {};

     struct fn :
          pegtl_string_t("fn")
     {};
     struct allocate:
          pegtl_string_t("print")
     {};
     struct print:
          pegtl_string_t("allocate")
     {};
     struct array_error :
          pegtl_string_t("array_error")
     {};
     struct runtime_fun :
          pegtl::sor<
          fn,
          allocate,
          array_error
          >
     {};

     struct u :
          pegtl::sor<
          var,
          label
          >
     {};

     struct binop_op :
          pegtl::sor<
          plus_op,
          minus_op,
          and__op,
          mult_op,
          left_shift_op,
          right_shift_op,
          leq_op,
          le_op,
          geq_op,
          ge_op,
          eq_op
          >
     {};

     // operands
     struct binop_tail :
          pegtl::seq<
          seps,
          binop_op,
          seps,
          t
          >
     {};

     struct load :
          pegtl::if_must<
          pegtl_string_t("load"),
          seps,
          var
          >
     {};

     struct store :
          pegtl::if_must<
          pegtl_string_t("store"),
          seps,
          var
          >
     {};

     struct operand :
          pegtl::sor<
          load,
          store
          >
     {};


     struct callable :
          pegtl::sor<
          u,
          runtime_fun
          >
     {};

     template<typename T, typename Delim>
     struct separated_list_multiple :
          pegtl::seq<
          T,
          seps,
          pegtl::star<
               Delim,
               seps,
               T
               >
          >
     {};

     template<typename T, typename Delim>
     struct separated_list :
          pegtl::opt<
          pegtl::sor<
               separated_list_multiple<T, Delim>,
               T
               >
          >
     {};

     template <typename T, typename Surrounder_open, typename Surrounder_close>
     struct surrounded_list :
          pegtl::if_must<
          Surrounder_open,
          seps,
          separated_list<T, pegtl::one<','>>,
          seps,
          Surrounder_close
          >
     {};

     struct arg_list :
          surrounded_list<t, pegtl::one<'('>,pegtl::one<')'>>
     {};

     struct call_start_token :
          pegtl::seq<pegtl_string_t("call"),
                     pegtl::space>
     {};

     struct call :
          pegtl::if_must<
          call_start_token,
          seps,
          callable,
          seps,
          arg_list
          >
     {};

     struct instr_call :
          pegtl::seq<
          call, // BAD
          seps
          >
     {};

     struct assignment_lhs :
          pegtl::sor<
          store,
          var
          >{};

     struct assignment_rhs :
          pegtl::sor<
          // This kills the stack =(
          call,
          load,
          pegtl::seq<
               s,
               seps,
               pegtl::opt<binop_tail>
               >
          >
     {};

     //instructions
     struct assignment :
          pegtl::seq<
          assignment_lhs,
          seps,
          left_arrow,
          seps,
          assignment_rhs
          >
     {};

     struct goto_ :
          pegtl::if_must<
          pegtl_string_t("br"),
          seps,
          label
          >
     {};

     struct cjump :
          pegtl::seq<
          pegtl_string_t("br"),
          seps,
          var,
          seps,
          label,
          seps,
          label
          >
     {};


     struct val_return_ :
          pegtl::seq<
          pegtl_string_t("return"),
          seps,
          t
          >
     {};

     struct void_return_ :
          pegtl_string_t("return")
     {};

     struct return_ :
          pegtl::sor<
          val_return_,
          void_return_
          >
     {};

     struct inst_label :
          label
     {};

     struct instruction :
          pegtl::sor<
          instr_call,
          int_literal,
          assignment,
          store,
          cjump,
          goto_,
          return_,
          inst_label
          >
     {};

     struct instructions :
          pegtl::if_must<
          pegtl::one<'{'>,
          pegtl::plus<
               pegtl::seq<seps,
                          instruction>
               >,
          seps,
          pegtl::one<'}'>
          >
     {};


     struct function_head :
          pegtl::if_must<
          pegtl_string_t("define"),
          seps,
          label
          >
     {};

     struct param_list :
          surrounded_list<var, pegtl::one<'('>,pegtl::one<')'>>
     {};

     struct function :
          pegtl::if_must<
          function_head,
          seps,
          param_list,
          seps,
          instructions
          >
     {};

     struct program :
          pegtl::seq<
          seps,
          pegtl::plus<pegtl::seq<
                           function,
                           seps>
                      >
          >
     {};

     struct grammar :
          pegtl::must<program>
     {};

     template <typename Rule>
     struct action :
          pegtl::nothing<Rule>{};


///////////////////////////////////////////////////////////////////////////////
//                                  Actions                                  //
///////////////////////////////////////////////////////////////////////////////
     //// ops
#define BINOP_ACTION(op_)                                               \
     template<> struct action<op_ ## _op>{                              \
          static void apply(const pegtl::input &in,                     \
                            Program &p,                                 \
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack, \
                            std::vector<Binop::Op> &op_stack,           \
                            std::vector<Runtime_Fun::Fun> &fun_stack){  \
               op_stack.push_back(Binop:: op_);                         \
                                                                        \
          }                                                             \
     };
     BINOP_ACTION(plus);
     BINOP_ACTION(minus);
     BINOP_ACTION(mult);
     BINOP_ACTION(and_);
     BINOP_ACTION(left_shift);
     BINOP_ACTION(right_shift);
     BINOP_ACTION(le);
     BINOP_ACTION(leq);
     BINOP_ACTION(eq);
     BINOP_ACTION(ge);
     BINOP_ACTION(geq);

     //// grammar atoms

     // rt funs

#define RT_FUN_ACTION(fun)                                              \
     template<> struct action<fun>{                                     \
          static void apply(const pegtl::input &in,                     \
                            Program &p,                                 \
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack, \
                            std::vector<Binop::Op> &op_stack,           \
                            std::vector<Runtime_Fun::Fun> &fun_stack){  \
               fun_stack.push_back(Runtime_Fun:: fun);                  \
          }                                                             \
     };
     RT_FUN_ACTION(print);
     RT_FUN_ACTION(allocate);
     RT_FUN_ACTION(array_error);


     template<> struct action<int_literal>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               the_stack.push(L3_ptr<AST_Item>{new Int_Literal{stoll(in.string())}});
          }
     };

     template<> struct action<var>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               the_stack.push(L3_ptr<AST_Item>{new Var{in.string()}});
          }
     };


     template<> struct action<label>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               the_stack.push(L3_ptr<AST_Item>{new Label{in.string()}});
          }
     };


///////////////////////////////////////////////////////////////////////////////
//                            Instruction Actions                           //
///////////////////////////////////////////////////////////////////////////////

     void curfun_instr_push(Program& p, L3_ptr<Instruction> inst);

     template<> struct action<store>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               the_stack.push(ast_ptr{new Store(the_stack.pop())});
          }
     };

     template<> struct action<load>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               the_stack.push(ast_ptr{new Load(the_stack.pop())});
          }
     };


          template<> struct action<call_start_token>{
               static void apply(const pegtl::input &in,
                                 Program &p,
                                 L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                                 std::vector<Binop::Op> &op_stack,
                                 std::vector<Runtime_Fun::Fun> &fun_stack){
                    the_stack.push(ast_ptr{new STAHP()});
               }
          };


     template<> struct action<call>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               std::vector<ast_ptr> everything;
               while(! dynamic_cast<STAHP*>(the_stack.peek().get())){
                    everything.insert(everything.begin(), the_stack.downcast_pop<AST_Item>());
               }

               the_stack.pop(); // clear STAHP

               the_stack.push(L3_ptr<AST_Item>{new Call(everything)});

          }
     };

     template<> struct action<instr_call>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               curfun_instr_push(p, the_stack.downcast_pop<Instruction>());
          }
     };



     template<> struct action<cjump>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               auto false_target = the_stack.downcast_pop<Label>();
               auto true_target = the_stack.downcast_pop<Label>();
               auto cond = the_stack.downcast_pop<Var>();

               curfun_instr_push(p, L3_ptr<Instruction>{
                         new Cjump{cond, true_target, false_target}});
          }
     };


     template<> struct action<inst_label>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               curfun_instr_push(p, L3_ptr<Instruction>{
                         new Label{in.string()}});
          }
     };


     template<> struct action<goto_>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               curfun_instr_push(p, L3_ptr<Instruction>{new Goto{
                              the_stack.downcast_pop<Label>()
                                   }});

               the_stack.NUKE();
          }
     };


     template<> struct action<binop_tail>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               auto rhs = the_stack.downcast_pop<AST_Item>();
               auto lhs = the_stack.downcast_pop<AST_Item>();
               Binop::Op op = op_stack.back();

               if(op == Binop::Op::ge){
                    op = Binop::Op::le;
                    std::swap(lhs, rhs);
               }
               if(op == Binop::Op::geq){
                    op = Binop::Op::leq;
                    std::swap(lhs, rhs);
               }
               the_stack.push(L3_ptr<AST_Item>{new Binop{op,
                                   lhs,
                                   rhs}});

               op_stack.pop_back();
          }
     };


     template<> struct action<assignment>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               auto rhs = the_stack.downcast_pop<AST_Item>();
               auto lhs = the_stack.downcast_pop<AST_Item>();
               curfun_instr_push(p, L3_ptr<Instruction>{new
                              Assignment{lhs, rhs}});
          }
     };


     template<> struct action<val_return_>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               curfun_instr_push(p, L3_ptr<Instruction>{new
                              Val_Return{the_stack.downcast_pop<AST_Item>()}});

               the_stack.NUKE();
          }
     };

     template<> struct action<void_return_>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               curfun_instr_push(p, L3_ptr<Instruction>{new Void_Return{}});
          }
     };

     template<> struct action<function_head>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){

               auto name = the_stack.downcast_pop<Label>();

               p.functions.push_back(std::make_shared<Function>(*name));

               the_stack.NUKE();
          }
     };


     template<> struct action<param_list>{
          static void apply(const pegtl::input &in,
                            Program &p,
                            L3_Parse_Stack<L3_ptr<AST_Item>> &the_stack,
                            std::vector<Binop::Op> &op_stack,
                            std::vector<Runtime_Fun::Fun> &fun_stack){
               auto curf = p.functions.back();

               while(!the_stack.is_empty()){
                    curf->params.insert(curf->params.begin(),
                                        *the_stack.downcast_pop<Var>());
               }
          }
     };

     Program parse_file (std::string fileName);
     Function parse_function_file (std::string fileName);
}
