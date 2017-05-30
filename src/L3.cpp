#include <L3.h>
#include <sstream>
#include <cassert>
#include <exception>
#ifdef UNIT_TEST
#include <catch.hpp>
#endif
#include <unordered_set>
#include <algorithm>
#include <set>

#include <tile_o_tron_4000.h>
#include <tiles.h> // bad.. should be singpulare

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
        for(auto it =  item->params.begin();
            it != item->params.end();
            it++){
                if(it != item->params.begin()){
                        result << ", ";
                }
                it->accept(*this);
        }
        result << ")";
        result << "{\n  ";
        for(auto it = item->instructions.begin();
            it != item->instructions.end();
            it++){
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


void Program     ::accept(AST_Item_Visitor &v) {v.visit(this); }
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

std::vector<ast_ptr> vector_concat(std::vector<ast_ptr> a,
                                   std::vector<ast_ptr> b){
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

std::unordered_set<std::string> Function::grabber_of_the_vars(){
        std::unordered_set<std::string> names;
        for(auto i_ptr : instructions){
                auto i_names = walk_for_names<Var, Label>(i_ptr);
                names.insert(i_names.begin(), i_names.end());
        }

        return names;
}

std::unordered_set<std::string> Function::grabber_of_the_labels(){
        std::unordered_set<std::string> names;

        for(auto i_ptr : instructions){
                auto i_names = walk_for_names<Label, Var>(i_ptr);
                names.insert(i_names.begin(), i_names.end());
        }

        names.insert(name.name);

        auto names_it = names.begin();
        auto stripped_names = std::unordered_set<std::string>{};

        while(names_it != names.end()){
                stripped_names.insert(names_it->substr(1));
                names_it++;
        }

        return stripped_names;
}

std::string Function::find_prefix(std::unordered_set<std::string> scrambled_symbols){

        std::set<std::string> symbols(scrambled_symbols.begin(), scrambled_symbols.end());

        auto z_symbol =
                std::find_if(symbols.begin(),
                             symbols.end(),
                             [](const std::string& s){
                                     return s[0] == 'z';
                             });

        if(z_symbol == symbols.end()){
                return std::string{'z'};
        }
        else{
                char next_char = '0';
                std::string the_prefix{"z0"};
                for(; z_symbol != symbols.end(); z_symbol++){
                        auto symbol = *z_symbol;

                        bool found_prefix{false};
                        // the_prefix can't collide with anything previous
                        // since if collision was possible, it would have
                        // sorted after the string we're checking
                        while(!found_prefix){

                                if(next_char == ':'){
                                        next_char = '0';
                                        the_prefix.push_back(next_char);
                                }

                                if(the_prefix == symbol.substr(0, the_prefix.length())){
                                        char prefix_add =
                                                symbol[the_prefix.length() - 1] + 1;
                                        the_prefix[the_prefix.length() - 1] = prefix_add;
                                        next_char = prefix_add + 1;
                                }
                                else{
                                        found_prefix = true;
                                }
                        }
                }
                return the_prefix;
        }
}



std::string Function::enstringify_l2ishly(std::function<std::string()> name_gen){
        Dump v;

        static std::array<char*, 6> arg_reg_str = {"rdi",
                                                   "rsi",
                                                   "rdx",
                                                   "rcx",
                                                   "r8",
                                                   "r9"};



        v.result << "(";
        name.accept(v);
        v.result << "\n";
        v.result << params.size() << " ";
        v.result << (params.size() > 6) ? params.size() - 6 : 0; // I love magic numbers. So. Much.
        v.result << "\n";

        for(int i = 0; i < params.size(); i++){
                if(i < 6){
                v.result << "(";
                params[i].accept(v);
                v.result << " <- ";
                v.result << arg_reg_str[i];
                v.result << ")" << "\n";
                } else {
                        v.result << "(";
                        params[i].accept(v);
                        v.result << " <- ";
                        v.result << "(stack-arg ";
                        v.result << ((params.size() - i) * 8);
                        v.result << "))\n";
                }
        }

        std::vector<Tile::tile_ptr> my_brand_new_tiles;

        my_brand_new_tiles.reserve(instructions.size());

        auto names = grabber_of_the_labels();

        auto prefix = find_prefix(names);

        // Make me some tiles. Yummy.
        for(auto i_ptr : instructions){
                my_brand_new_tiles.push_back(L3::Tile::match_me_bro(i_ptr, name_gen));
        }

        for(auto t_ptr : my_brand_new_tiles){
                v.result << t_ptr->to_L2();
                v.result << "\n";
        }

        v.result << ")";

        return v.result.str();
}

namespace L3{
        void prefixify_labels(ast_ptr item,
                              std::string fun_prefix,
                              std::set<std::string> globally_scoped_names){

                if(is_one_of<L3::Label>(item)){
                        auto lab_ptr = dynamic_cast<L3::Label*>(item.get());

                        if(globally_scoped_names.count(lab_ptr->name)){
                                return;
                        }

                        auto super_fun_prefix = std::string(":");
                        super_fun_prefix.append(fun_prefix);


                        auto orig_label_name = lab_ptr->name.substr(1);

                        lab_ptr->name = super_fun_prefix.append(orig_label_name);
                }

                else if(is_one_of<L3::Instruction>(item)){
                        auto i_ptr = dynamic_cast<L3::Instruction*>(item.get());
                        for(auto child : i_ptr->operands){
                                prefixify_labels(child, fun_prefix, globally_scoped_names);
                        }
                }
        }
}
void Function::scopify_labels(std::string fun_prefix, std::set<std::string> globally_scoped_names){
        for(auto inst : instructions){
                if(is_one_of<Call>(inst)){
                        continue;
                }
                prefixify_labels(inst, fun_prefix, globally_scoped_names);
        }
}

#ifdef UNIT_TEST

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

#endif
