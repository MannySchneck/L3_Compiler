#include <tiles.h>
#include <exception>
#ifdef UNIT_TEST
#include <catch.hpp>
#endif
#include <memory>
#include <array>
#include <cassert>

using namespace L3::Tile;

template<typename T, typename... Args>
tile_ptr make_tile(Args&&... args){
        return L3::make_thing<tile_ptr, T>(std::forward<Args>(args)...);
}


///////////////////////////////////////////////////////////////////////////////
//                                Atom Assign                                //
///////////////////////////////////////////////////////////////////////////////


Atom_Assignment::Atom_Assignment(L3_ptr<L3::AST_Item> lhs,
                                 L3_ptr<L3::AST_Item> rhs_atom) :
        lhs(lhs),
        rhs_atom(rhs_atom){

        if(!is_s(rhs_atom)){
                throw std::logic_error("can't make an atom rhs tile with a non-atom =(");
        }

        if(!L3::is_one_of<L3::Var>(lhs)){
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

#ifdef UNIT_TEST
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
                Atom_Assignment no{L3::ast_ptr {new L3::Var{"hi_mom"}}, L3::L3_ptr<L3::AST_Item>{new
                                                                                        L3::Label{":sad"}}};
                REQUIRE(no.to_L2() == "(hi_mom <- :sad)");
        }

        SECTION("bad use of a binop here"){
                REQUIRE_THROWS((Atom_Assignment{L3::ast_ptr{new L3::Var{"hi_mom"}}, L3::L3_ptr<L3::AST_Item>{
                                                new L3::Binop{L3::Binop::Op::plus,
                                                                L3::ast_ptr{new L3::Var{"hi"}},
                                                                L3::ast_ptr{new L3::Var{"mom"}}}}}));
        }
}
#endif

///////////////////////////////////////////////////////////////////////////////
//                                Load Assign                                //
///////////////////////////////////////////////////////////////////////////////



Load_Assignment::Load_Assignment(ast_ptr lhs, ast_ptr rhs) :
        lhs(lhs),
        rhs(rhs)
{
        if(!L3::is_one_of<L3::Var>(lhs)){
                throw std::logic_error("You can't load if lhs isn't a var");
        }

        if(!L3::is_one_of<L3::Load>(rhs)){
                throw std::logic_error("You can't load if lhs isn't a load");
        }
}

// Dicks

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


#ifdef UNIT_TEST
TEST_CASE("assigning from a load"){
        SECTION("Translate load"){
                Load_Assignment a_load(
                        L3::make_AST<L3::Var>("hi"),
                        L3::make_AST<L3::Load>(
                                L3::make_AST<L3::Var>("load_me")));
                REQUIRE(a_load.to_L2() == "(hi <- (mem load_me 0))");
        }

        SECTION("Constructor guards work"){
                REQUIRE_THROWS(Load_Assignment(L3::ast_ptr
                                               {new L3::Int_Literal{5}},
                                               L3::ast_ptr {new L3::Load{L3::ast_ptr{new L3::Var{"load_me"}}}}));
                REQUIRE_THROWS(Load_Assignment(L3::ast_ptr{new L3::Int_Literal{5}},
                                               L3::ast_ptr{new L3::Store{L3::ast_ptr{new
                                                                               L3::Var{"load_me"}}}}));
        }
}
#endif
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

#ifdef UNIT_TEST
TEST_CASE("assigning to a store"){
        SECTION("Translate store"){
                Store_Assignment a_store(
                        L3::make_AST<L3::Store>(
                                L3::make_AST<L3::Var>("store_at_me")),
                        L3::make_AST<L3::Var>("hi"));
                REQUIRE(a_store.to_L2() == "((mem store_at_me 0) <- hi)");
        }
}
#endif
///////////////////////////////////////////////////////////////////////////////
//                                Call Assign                                //
///////////////////////////////////////////////////////////////////////////////
Call_Assignment::Call_Assignment(L3::ast_ptr lhs,
                                 L3::ast_ptr rhs,
                                 std::function<std::string()> name_gen) :
        lhs(lhs),
        rhs(rhs)
{

        auto call_ptr = std::dynamic_pointer_cast<L3::Call>(rhs);

        children.push_back(
                make_tile<Call>(call_ptr->get_callee(), call_ptr->get_args(), L3::Label{name_gen()})
                );

        if(!L3::is_one_of<L3::Var>(lhs)){
                throw std::logic_error("can't make a binop assignment with non-var lhs");
        }
        if(!L3::is_one_of<L3::Call>(rhs)){
                throw std::logic_error("can't make a call assignment with a non-call rhs");
        }
}

// Delegate to call tile
std::string Call_Assignment::to_L2(){
        Dump v;

        assert(children.size() == 1);
        v.result << children[0]->to_L2();

        v.result << "(";

        lhs->accept(v);

        v.result << " <- rax)\n";

        return v.result.str();
}

#ifdef UNIT_TEST
TEST_CASE("A call assignment"){
        auto target = L3::make_AST<L3::Var>("hi");
        auto funfun = L3::make_AST<L3::Call>(std::vector<L3::ast_ptr>{L3::make_AST<L3::Label>(":funfun"),
                                L3::make_AST<L3::Var>("an_arg"),
                                L3::make_AST<L3::Var>("another_arg"),
                                L3::make_AST<L3::Int_Literal>(5)});

        auto call_assign_tile = make_tile<Call_Assignment>(target, funfun, [](){return ":rett";});


        REQUIRE(call_assign_tile->to_L2() ==
                "(rdi <- an_arg)\n"
                "(rsi <- another_arg)\n"
                "(rdx <- 5)\n"
                "((mem rsp -8) <- :rett)\n"
                "(call :funfun 3)\n"
                ":rett\n"
                "(hi <- rax)\n");
}
#endif
///////////////////////////////////////////////////////////////////////////////
//                                Binop Assign                                //
///////////////////////////////////////////////////////////////////////////////

Binop_Assignment::Binop_Assignment(L3::ast_ptr lhs,
                                   L3::ast_ptr rhs) :
        lhs(lhs),
        rhs(rhs){
        if(!L3::is_one_of<L3::Var>(lhs)){
                throw std::logic_error("can't make a binop assignment with non-var lhs");
        }
        if(!L3::is_one_of<L3::Binop>(rhs)){
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



#ifdef UNIT_TEST
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
#endif
///////////////////////////////////////////////////////////////////////////////
//                                    Goto                                   //
///////////////////////////////////////////////////////////////////////////////


Goto::Goto(ast_ptr target) :
        target(target)
{
        if(!L3::is_one_of<L3::Label>(target)){
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

#ifdef UNIT_TEST
TEST_CASE("Going to the going to place"){
        SECTION("Going there"){
                Goto gt(L3::make_AST<L3::Label>(":there"));
                REQUIRE(gt.to_L2() == "(goto :there)");
        }
}
#endif
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

#ifdef UNIT_TEST
TEST_CASE("Jump little man jump. It won't save you. But you're going to do it anyway. SAD!"){
        SECTION("do we can a cjump?"){
                Cjump cj(L3::make_AST<L3::Var>("plz"), L3::make_AST<L3::Label>(":t"), L3::make_AST<L3::Label>(":f"));
                REQUIRE(cj.to_L2() == "(cjump plz = 1 :t :f)\n");
        }
}
#endif
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

#ifdef UNIT_TEST
TEST_CASE("returning a value"){
        Val_Return r(L3::make_AST<L3::Int_Literal>(6));
        REQUIRE(r.to_L2() == "(rax <- 6)\n(return)\n");
        Val_Return r2(L3::make_AST<L3::Var>("blorpistry"));
        REQUIRE(r2.to_L2() == "(rax <- blorpistry)\n(return)\n");
}
#endif

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

#ifdef UNIT_TEST
TEST_CASE("returning Nothing. Because Life means nothing"){
        Void_Return r{};
        REQUIRE(r.to_L2() == "(return)\n");
}
#endif
///////////////////////////////////////////////////////////////////////////////
//                                    Call                                   //
///////////////////////////////////////////////////////////////////////////////
// Fuck


Call::Call(ast_ptr target, std::vector<ast_ptr> args, L3::Label retlab) :
        target(target),
        args(args),
        retlab(retlab)
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


        v.result << "((mem rsp -8) <- ";
        retlab.accept(v);
        v.result << ")\n";
        v.result << "(call";
        v.result << " ";
        target->accept(v);
        v.result << " ";
        v.result << args.size();
        v.result << ")\n";
        retlab.accept(v);
        v.result << "\n";

        return v.result.str();
}

#ifdef UNIT_TEST
TEST_CASE("Call me baby"){
        SECTION("Calling you baby, like you asked me to"){
                Call a_call(L3::make_AST<L3::Label>(":call_me_please_im_so_alone"),
                            {L3::make_AST<L3::Var>("Hi"),
                                            L3::make_AST<L3::Var>("ho")},
                            L3::Label(":rett"));
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
                                            },
                            L3::Label(":rett"));

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
#endif

///////////////////////////////////////////////////////////////////////////////
//                                  Free Label                               //
///////////////////////////////////////////////////////////////////////////////

Label::Label(ast_ptr lab) : 
        lab(lab)
{
        assert(is_one_of<L3::Label>(lab));
}

std::string Label::to_L2(){
        Dump v;

        lab->accept(v);

        return v.result.str();
}

/////////////////////////////////////////////////////////////////////////////////////////
// Matchmaker matchmaker make me a match. Because I created you. I AM YOUR GOD. BOW TO ME.
/////////////////////////////////////////////////////////////////////////////////////////



/*
  ONE FUNCTION TO RULE THEM ALL ONE FUNCTION TO FIND THEM. ONE FUNCTION TO
  TILE THEM ALL AND IN L2-TRANSLATABLE FORM BIND THEM.
*/


tile_ptr L3::Tile::match_me_bro(L3::ast_ptr item, std::function<std::string()> name_gen){
        L3::Assignment* assignement_ptr = nullptr;


        // Whelp this is ugly.... SAD Eventually this will have recursive calls and
        // I can strip out the visited flag


        // FORTRAN CODE STARTS HERE
        if(L3::is_one_of<L3::Assignment>(item)){
                // of course we need a goto, if this is FORTRAN

                goto SAD;
        SAD:

                auto assgn_ptr = std::dynamic_pointer_cast<L3::Assignment>(item);

                if(L3::is_one_of<L3::Var>(assgn_ptr->get_lhs())){
                        assgn_ptr->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                        assgn_ptr->
                                get_lhs()->
                                has_already_been_tiled_why_are_you_still_here_question_mark = true;
                        assgn_ptr->
                                get_rhs()->
                                has_already_been_tiled_why_are_you_still_here_question_mark = true;

                        if(is_s(assgn_ptr->get_rhs()) && L3::is_one_of<L3::Var>(assgn_ptr->get_lhs())){
                                return make_tile<Atom_Assignment>(assgn_ptr->get_lhs(), assgn_ptr->get_rhs());
                        }

                        if(L3::is_one_of<L3::Load>(assgn_ptr->get_rhs())){
                                auto load_ptr = std::dynamic_pointer_cast<L3::Load>(assgn_ptr->get_rhs());

                                load_ptr->get_loadee()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                                return make_tile<Load_Assignment>(assgn_ptr->get_lhs(), assgn_ptr->get_rhs());
                        }

                        if(L3::is_one_of<L3::Binop>(assgn_ptr->get_rhs())
                           && L3::is_one_of<L3::Var>(assgn_ptr->get_lhs())){
                                auto binop_ptr = std::dynamic_pointer_cast<L3::Binop>(assgn_ptr->get_rhs());

                                binop_ptr->get_lhs()->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                                binop_ptr->get_rhs()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                                return make_tile<Binop_Assignment>(assgn_ptr->get_lhs(), assgn_ptr->get_rhs());
                        }

                        if(L3::is_one_of<L3::Call>(assgn_ptr->get_rhs())){

                                assgn_ptr->get_lhs()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                                auto call_ptr = std::dynamic_pointer_cast<L3::Call>(assgn_ptr->get_rhs());
                                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                                call_ptr->get_callee()->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                                for(auto thing : call_ptr->get_args()){
                                        thing->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                                }

                                return make_tile<Call_Assignment>(assgn_ptr->get_lhs(), assgn_ptr->get_rhs(), name_gen);
                        }
                }

                if(L3::is_one_of<L3::Store>(assgn_ptr->get_lhs())
                   && L3::is_s(assgn_ptr->get_rhs())){

                        auto store_ptr = std::dynamic_pointer_cast<L3::Store>(assgn_ptr->get_lhs());

                        store_ptr->get_storee()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                        return make_tile<Store_Assignment>(assgn_ptr->get_lhs(), assgn_ptr->get_rhs());
                }
        }

        if (L3::is_one_of<L3::Goto>(item)){
                auto goto_ptr = std::dynamic_pointer_cast<L3::Goto>(item);
                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                goto_ptr->get_target()->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                return make_tile<Goto>(goto_ptr->get_target());
        }

        if (L3::is_one_of<L3::Cjump>(item)){
                auto cjump_ptr = std::dynamic_pointer_cast<L3::Cjump>(item);
                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                cjump_ptr->get_cond()->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                cjump_ptr->get_true_target()->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                cjump_ptr->get_false_target()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                return make_tile<Cjump>(cjump_ptr->get_cond(),
                                        cjump_ptr->get_true_target(),
                                        cjump_ptr->get_false_target());
        }

        if (L3::is_one_of<L3::Call>(item)){
                auto call_ptr = std::dynamic_pointer_cast<L3::Call>(item);
                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                call_ptr->get_callee()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                for(auto thing : call_ptr->get_args()){
                        thing->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                }

                return make_tile<Call>(call_ptr->get_callee(), call_ptr->get_args(), L3::Label{name_gen()});
        }

        if(L3::is_one_of<L3::Val_Return>(item)){
                auto r_ptr = std::dynamic_pointer_cast<L3::Val_Return>(item);

                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;
                r_ptr->get_result()->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                return make_tile<Val_Return>(r_ptr->get_result());
        }

        if(L3::is_one_of<L3::Void_Return>(item)){
                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                return make_tile<Void_Return>();
        }

        if(L3::is_one_of<L3::Label>(item)){
                item->has_already_been_tiled_why_are_you_still_here_question_mark = true;

                return make_tile<Label>(item);
        }

        Dump v;

        item->accept(v);

        std::cerr << "You couldn't tile this: " << v.result.str();

        throw std::logic_error("This impossible. Go home manny, you're drunk");
}


#ifdef UNIT_TEST
TEST_CASE("The tile matcher can match things"){
        SECTION("Assignment"){
                SECTION("atom assignment"){
                        auto my_downfall = L3::make_AST<L3::Assignment>(L3::make_AST<L3::Var>("hi_mom"),
                                                                        L3::make_AST<L3::Var>("mork"));

                        auto some_tile = match_me_bro(my_downfall, [](){ return ":rett";});

                        Atom_Assignment no{L3::ast_ptr {new L3::Var{"hi_mom"} },
                                        L3::L3_ptr<L3::AST_Item>{new L3::Var{"mork"}}};

                        REQUIRE(some_tile->to_L2() == no.to_L2());
                }
                SECTION("load assignment"){
                        auto my_downfall = L3::make_AST<L3::Assignment>(L3::make_AST<L3::Var>("hi_mom"),
                                                                        L3::make_AST<L3::Load>(
                                                                                L3::make_AST<L3::Var>("mork")));

                        auto some_tile = match_me_bro(my_downfall, [](){ return ":rett";});

                        Load_Assignment no{L3::ast_ptr {new L3::Var{"hi_mom"} },
                                        L3::make_AST<L3::Load>(L3::L3_ptr<L3::AST_Item>{new L3::Var{"mork"}})};

                        REQUIRE(some_tile->to_L2() == no.to_L2());
                }

                SECTION("store assignment"){
                        auto my_downfall = L3::make_AST<L3::Assignment>(
                                L3::make_AST<L3::Store>(
                                        L3::make_AST<L3::Var>("mork")),
                                L3::make_AST<L3::Var>("hi_mom")
                                );

                        auto some_tile = match_me_bro(my_downfall, [](){ return ":rett";});

                        Store_Assignment no{
                                L3::make_AST<L3::Store>(L3::L3_ptr<L3::AST_Item>{new L3::Var{"mork"}}),
                                        L3::ast_ptr {new L3::Var{"hi_mom"} }};

                        REQUIRE(some_tile->to_L2() == no.to_L2());
                }

                SECTION("Binop Assignment"){
                        auto my_downfall = L3::make_AST<L3::Assignment>(
                                L3::make_AST<L3::Var>("hi_mom"),
                                L3::make_AST<L3::Binop>(L3::Binop::plus,
                                                        L3::make_AST<L3::Var>("lhs"),
                                                        L3::make_AST<L3::Var>("rhs"))
                                );

                        auto some_tile = match_me_bro(my_downfall, [](){ return ":rett";});

                        Binop_Assignment no{
                                L3::ast_ptr {new L3::Var{"hi_mom"} },
                                        L3::make_AST<L3::Binop>(L3::Binop::plus,
                                                                L3::make_AST<L3::Var>("lhs"),
                                                                L3::make_AST<L3::Var>("rhs"))
                                                };

                        REQUIRE(some_tile->to_L2() == no.to_L2());
                }

                SECTION("Goto"){
                        auto why_bother = L3::make_AST<L3::Goto>(std::make_shared<L3::Label>(":nope"));

                        auto stop_in_the_name_of_love = match_me_bro(why_bother, [](){ return ":rett";});

                        Goto go(L3::make_AST<L3::Label>(":nope"));

                        REQUIRE(stop_in_the_name_of_love->to_L2() == go.to_L2());
                }

                SECTION("cjump"){
                        auto why_bother = L3::make_AST<L3::Cjump>(std::make_shared<L3::Var>("stahp"),
                                                                  std::make_shared<L3::Label>(":yup"),
                                                                  std::make_shared<L3::Label>(":nope"));

                        auto stop_in_the_name_of_love = match_me_bro(why_bother, [](){ return ":rett";});

                        Cjump j(L3::make_AST<L3::Var>("stahp"),
                                L3::make_AST<L3::Label>(":yup"),
                                L3::make_AST<L3::Label>(":nope"));

                        REQUIRE(stop_in_the_name_of_love->to_L2() == j.to_L2());

                }

                SECTION("Call"){
                        SECTION("Calling you baby, like you asked me to"){
                                auto no = L3::make_AST<L3::Call>(
                                        std::vector<L3::ast_ptr>{L3::make_AST<L3::Label>(":call_me_please_im_so_alone"),
                                                        L3::make_AST<L3::Var>("Hi"),
                                                        L3::make_AST<L3::Var>("ho")}
                                        );
                                Call a_call(L3::make_AST<L3::Label>(":call_me_please_im_so_alone"),
                                            {L3::make_AST<L3::Var>("Hi"),
                                                            L3::make_AST<L3::Var>("ho")},
                                            L3::Label(":rett"));

                                auto blerp = match_me_bro(no, [](){ return ":rett";});
                                REQUIRE(blerp->to_L2() == a_call.to_L2());
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
                                                            },
                                            L3::Label(":rett"));

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

                        SECTION("val ret"){

                                auto wat = L3::make_AST<L3::Val_Return>(L3::make_AST<L3::Var>("blorpistry"));

                                auto blerp = match_me_bro(wat, [](){return ":rett";});

                                Val_Return r2(L3::make_AST<L3::Var>("blorpistry"));
                                REQUIRE(blerp->to_L2() == r2.to_L2());
                        }

                        SECTION("void ret"){
                                auto nope = L3::make_AST<L3::Void_Return>();

                                auto blerp = match_me_bro(nope, [](){return ":rett";});

                                Void_Return hi;
                                REQUIRE(blerp->to_L2() == hi.to_L2());
                        }
                }
        }
}
#endif
