#include <tiles.h>
#include <cassert>
#include <tile_o_tron_4000.h>
#include <catch.hpp>
#include <exception>
#include <memory>
#include <array>

using namespace L3::Tile;

///////////////////////////////////////////////////////////////////////////////
//                                Atom Assign                                //
///////////////////////////////////////////////////////////////////////////////

Atom_Assignment::Atom_Assignment(L3_ptr<L3::AST_Item> lhs, L3_ptr<L3::AST_Item> rhs_atom) :
        lhs(lhs),
        rhs_atom(rhs_atom) {

        if(!is_s(rhs_atom)){
                throw std::logic_error("can't make an atom rhs tile with a non-atom =(");
        }

        if(!is_one_of<L3::Var>(lhs)){
                throw std::logic_error("can't make a thing because this isn't a var =(");
        }

        }

std::string Atom_Assignment::to_L2() {
        std::stringstream ss;

        ss << "(";

        L3::Dump v;
        lhs->accept(v);
        ss << v.result.str();

        ss << " <- ";

        L3::Dump v2;
        rhs_atom->accept(v2);

        ss << v2.result.str();

        ss << ")";
        return ss.str();
}

TEST_CASE("L2ization of atomic assignment"){
        SECTION("var to var"){
                Atom_Assignment no{L3::ast_ptr {new L3::Var{"hi_mom"} },
                                L3::L3_ptr<L3::AST_Item>{new L3::Var{"mork"}}};
                REQUIRE(no.to_L2() == "(hi_mom <- mork)");
        }

        SECTION("int to var"){
                Atom_Assignment no{L3::ast_ptr{new L3::Var{"hi_mom"} },
                                L3::L3_ptr<L3::AST_Item>{new L3::Int_Literal{3}}};
                REQUIRE(no.to_L2() == "(hi_mom <- 3)");
        }
        SECTION("label to var"){
                Atom_Assignment no{L3::ast_ptr {new L3::Var{"hi_mom"}}, L3::L3_ptr<L3::AST_Item>{new L3::Label{":sad"}}};
                REQUIRE(no.to_L2() == "(hi_mom <- :sad)");
        }

        SECTION("bad use of a binop here"){
                REQUIRE_THROWS((Atom_Assignment{L3::ast_ptr{new L3::Var{"hi_mom"}}, L3::L3_ptr<L3::AST_Item>{
                                                new L3::Binop{L3::Binop::Op::plus,
                                                                L3::ast_ptr{new L3::Var{"hi"}},
                                                                L3::ast_ptr{new L3::Var{"mom"}}}}}));
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                Load Assign                                //
///////////////////////////////////////////////////////////////////////////////

Load_Assignment::Load_Assignment(ast_ptr lhs, ast_ptr rhs) :
        lhs(lhs),
        rhs(rhs)
{
        if(!is_one_of<L3::Var>(lhs)){
                throw std::logic_error("You can't load if lhs isn't a var");
        }

        if(!is_one_of<L3::Load>(rhs)){
                throw std::logic_error("You can't load if lhs isn't a load");
        }
}


std::string Load_Assignment::to_L2(){
        std::stringstream ss;

        Dump v;
        lhs->accept(v);

        ss << "(" << v.result.str();

        ss << " <- ";

        auto load_ptr = dynamic_cast<Load*>(rhs.get());

        Dump v2;
        load_ptr->get_loadee()->accept(v2);

        ss << "(mem " << v2.result.str() << " " << "0" << ")";
        ss << ")";

        return ss.str();
}


TEST_CASE("assigning from a load"){
        SECTION("Translate load"){
                Load_Assignment a_load(
                        L3::make_AST<L3::Var>("hi"),
                        L3::make_AST<L3::Load>(
                                L3::make_AST<L3::Var>("load_me")));
                REQUIRE(a_load.to_L2() == "(hi <- (mem load_me 0))");
        }

        SECTION("Constructor guards work"){
                REQUIRE_THROWS(Load_Assignment(L3::ast_ptr{new L3::Int_Literal{5}},
                                               L3::ast_ptr{new L3::Load{L3::ast_ptr{new
                                                                               L3::Var{"load_me"}}}}));
                REQUIRE_THROWS(Load_Assignment(L3::ast_ptr{new L3::Int_Literal{5}},
                                               L3::ast_ptr{new L3::Store{L3::ast_ptr{new
                                                                               L3::Var{"load_me"}}}}));
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                Store Assign                               //
///////////////////////////////////////////////////////////////////////////////

Store_Assignment::Store_Assignment(L3::ast_ptr lhs,
                                   L3::ast_ptr rhs) :
        lhs(lhs),
        rhs(rhs)
{}


std::string Store_Assignment::to_L2(){
        std::stringstream ss;

        Dump v;
        auto store_ptr = dynamic_cast<Store*>(lhs.get());
        store_ptr->get_storee()->accept(v);

        ss << "(" << "(mem "  << v.result.str() << " " << "0" << ")";
        ss << " " << "<-" << " ";

        Dump v2;
        rhs->accept(v2);
        ss << v2.result.str() << ")";

        return ss.str();
}

TEST_CASE("assigning to a store"){
        SECTION("Translate store"){
                Store_Assignment a_store(
                        L3::make_AST<L3::Store>(
                                L3::make_AST<L3::Var>("store_at_me")),
                        L3::make_AST<L3::Var>("hi"));
                REQUIRE(a_store.to_L2() == "((mem store_at_me 0) <- hi)");
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                Binop Assign                                //
///////////////////////////////////////////////////////////////////////////////

Binop_Assignment::Binop_Assignment(L3::ast_ptr lhs,
                                   L3::ast_ptr rhs) :
        lhs(lhs),
        rhs(rhs){
        if(!is_one_of<L3::Var>(lhs)){
                throw std::logic_error("can't make a binop assignment with non-var lhs");
        }
        if(!is_one_of<L3::Binop>(rhs)){
                throw std::logic_error("can't make a binop assignment with a non-binop rhs");
        }
        }

// This is bad m'kay?
std::string Binop_Assignment::to_L2(){
        std::stringstream ss;

        auto binop_ptr = dynamic_cast<L3::Binop*>(rhs.get());

        Dump v;

        lhs->accept(v);
        auto bas_lhs_var = v.result.str();

        Dump v2;
        binop_ptr->get_lhs()->accept(v2);
        Dump v3;
        binop_ptr->get_rhs()->accept(v3);

        auto binop_lhs = v2.result.str();
        auto binop_rhs = v3.result.str();

        switch(binop_ptr->op){
        case(L3::Binop::plus):
        case(L3::Binop::mult):
        case(L3::Binop::minus):
        case(L3::Binop::and_):
        case(L3::Binop::left_shift):
        case(L3::Binop::right_shift):
                ss << "(";
        ss << bas_lhs_var;
        ss << " <- ";
        ss << binop_lhs;
        ss << ")\n";


        ss << "(";
        ss << bas_lhs_var;
        ss << " ";
        ss << binop_ptr->dump_op(binop_ptr->op);
        ss << "=";
        ss << " ";
        ss << binop_rhs;
        ss << ")\n";
        break;
        case(L3::Binop::le):
        case(L3::Binop::leq):
        case(L3::Binop::eq):
                ss << "(";
        ss << bas_lhs_var;
        ss << " <- ";
        ss << binop_lhs;
        ss << " ";
        ss << binop_ptr->dump_op(binop_ptr->op);
        ss << " ";
        ss << binop_rhs;
        ss << ")\n";
        break;
        case(L3::Binop::ge):
        case(L3::Binop::geq):
                throw std::logic_error("How did a greater comparison get past the parser???");
        break;
        }

        return ss.str();
}

TEST_CASE("a binop tile"){
        SECTION("plus me up baby"){
                Binop_Assignment ba(L3::make_AST<L3::Var>("sad_face"),
                                    L3::make_AST<L3::Binop>(L3::Binop::plus,
                                                            L3::make_AST<L3::Int_Literal>(6),
                                                            L3::make_AST<L3::Var>("no")));

                REQUIRE(ba.to_L2() == "(sad_face <- 6)\n(sad_face += no)\n");
        }

        SECTION("shall I compare thee to an integer value?"){
                Binop_Assignment ba(L3::make_AST<L3::Var>("sad_face"),
                                    L3::make_AST<L3::Binop>(L3::Binop::le,
                                                            L3::make_AST<L3::Int_Literal>(6),
                                                            L3::make_AST<L3::Var>("no")));

                REQUIRE(ba.to_L2() == "(sad_face <- 6 < no)\n");
        }

        SECTION("U can't do this SAD"){
                Binop_Assignment ba(L3::make_AST<L3::Var>("sad_face"),
                                    L3::make_AST<L3::Binop>(L3::Binop::ge,
                                                            L3::make_AST<L3::Int_Literal>(6),
                                                            L3::make_AST<L3::Var>("no")));

                REQUIRE_THROWS(ba.to_L2());
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                    Goto                                   //
///////////////////////////////////////////////////////////////////////////////


Goto::Goto(ast_ptr target) :
        target(target)
{
        if(!is_one_of<L3::Label>(target)){
                throw std::logic_error("Can't go somewhere if that where is not a place. So. Deep.");
        }
}


std::string Goto::to_L2(){
        std::stringstream ss;

        Dump v;

        v.result << "(";
        v.result << "goto";
        v.result << " ";
        target->accept(v);
        v.result << ")";
        return v.result.str();
}

TEST_CASE("Going to the going to place"){
        SECTION("Going there"){
                Goto gt(L3::make_AST<L3::Label>(":there"));
                REQUIRE(gt.to_L2() == "(goto :there)");
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                   Cjump                                   //
///////////////////////////////////////////////////////////////////////////////

Cjump::Cjump(ast_ptr cmp_result, ast_ptr t_target, ast_ptr f_target) :
        cmp_result(cmp_result),
        t_target(t_target),
        f_target(f_target)
{}


std::string Cjump::to_L2(){
        Dump v;

        v.result << "(";
        v.result << "cjump";
        v.result << " ";
        cmp_result->accept(v); v.result << " = "; v.result << "1";
        v.result << " ";
        t_target->accept(v);
        v.result << " ";
        f_target->accept(v);
        v.result << ")\n";

        return v.result.str();
}

TEST_CASE("Jump little man jump. It won't save you. But you're going to do it anyway. SAD!"){
        SECTION("do we can a cjump?"){
                Cjump cj(L3::make_AST<L3::Var>("plz"), L3::make_AST<L3::Label>(":t"), L3::make_AST<L3::Label>(":f"));
                REQUIRE(cj.to_L2() == "(cjump plz = 1 :t :f)\n");
        }
}

///////////////////////////////////////////////////////////////////////////////
//                                 Val_Return                                //
///////////////////////////////////////////////////////////////////////////////

Val_Return::Val_Return(ast_ptr result) :
        result(result)
{}

std::string Val_Return::to_L2(){
        Dump v;

        v.result << "(";
        v.result << "rax";
        v.result << " <- ";
        result->accept(v); // This isn't confusing at all. Great job Brotato.
        v.result << ")\n";
        v.result << "(";
        v.result << "return";
        v.result << ")\n";

        return v.result.str();
}

TEST_CASE("returning a value"){
        Val_Return r(L3::make_AST<L3::Int_Literal>(6));
        REQUIRE(r.to_L2() == "(rax <- 6)\n(return)\n");
        Val_Return r2(L3::make_AST<L3::Var>("blorpistry"));
        REQUIRE(r2.to_L2() == "(rax <- blorpistry)\n(return)\n");
}

///////////////////////////////////////////////////////////////////////////////
//                                Void_Return                                //
///////////////////////////////////////////////////////////////////////////////
Void_Return::Void_Return()
{}

std::string Void_Return::to_L2(){
        Dump v;

        v.result << "(";
        v.result << "return";
        v.result << ")\n";

        return v.result.str();
}

TEST_CASE("returning Nothing. Because Life means nothing"){
        Void_Return r{};
        REQUIRE(r.to_L2() == "(return)\n");
}

///////////////////////////////////////////////////////////////////////////////
//                                    Call                                   //
///////////////////////////////////////////////////////////////////////////////
// Fuck

std::string gen_unique_label(std::set<std::string> used_names){
        return ":rett";
}


Call::Call(ast_ptr target, std::vector<ast_ptr> args) :
        target(target),
        args(args)
{}

std::string Call::to_L2(){
        static std::array<char*, 6> arg_reg_str = {"rdi",
                                                   "rsi",
                                                   "rdx",
                                                   "rcx",
                                                   "r8",
                                                   "r9"};


        Dump v;

        for(int i = 0; i < args.size(); i++){
                if(i < 6){
                        v.result << "(";
                        v.result << arg_reg_str[i];
                        v.result << " <- ";
                        args[i]->accept(v);
                        v.result << ")\n";
                } else{
                        v.result << "(";
                        v.result << "(";
                        v.result << "mem rsp ";
                        v.result << -16 - (8 *(i - 6));
                        v.result << ")";

                        v.result << " <- ";
                        args[i]->accept(v);
                        v.result << ")\n";
                }
        }

        v.result << "((mem rsp -8) <- " << gen_unique_label({}) << ")\n";
        v.result << "(call";
        v.result << " ";
        target->accept(v);
        v.result << " ";
        v.result << args.size();
        v.result << ")\n";
        v.result << gen_unique_label({}) << "\n";

        return v.result.str();
}

TEST_CASE("Call me baby"){
        SECTION("Calling you baby, like you asked me to"){
                Call a_call(L3::make_AST<L3::Label>(":call_me_please_im_so_alone"),
                            {L3::make_AST<L3::Var>("Hi"),
                                            L3::make_AST<L3::Var>("ho")});
                REQUIRE(a_call.to_L2() ==
                        "(rdi <- Hi)\n"\
                        "(rsi <- ho)\n"\
                        "((mem rsp -8) <- :rett)\n"\
                        "(call :call_me_please_im_so_alone 2)\n"
                        ":rett\n");
        }
        SECTION("Calling you too many times. Now you're blocked (spilled)"){
                Call a_call(L3::make_AST<L3::Label>(":call_me_please_im_so_alone"),
                            {L3::make_AST<L3::Var>("Hi"),
                                            L3::make_AST<L3::Var>("ho"),
                                            L3::make_AST<L3::Var>("hope"),
                                            L3::make_AST<L3::Var>("is"),
                                            L3::make_AST<L3::Var>("useless"),
                                            L3::make_AST<L3::Int_Literal>(5),
                                            L3::make_AST<L3::Int_Literal>(12),
                                            L3::make_AST<L3::Int_Literal>(12)
                                            });

                REQUIRE(a_call.to_L2() ==
                        "(rdi <- Hi)\n" \
                        "(rsi <- ho)\n" \
                        "(rdx <- hope)\n" \
                        "(rcx <- is)\n" \
                        "(r8 <- useless)\n" \
                        "(r9 <- 5)\n" \
                        "((mem rsp -16) <- 12)\n" \
                        "((mem rsp -24) <- 12)\n" \
                        "((mem rsp -8) <- :rett)\n" \
                        "(call :call_me_please_im_so_alone 8)\n"\
                        ":rett\n"
                        );
        }

}
