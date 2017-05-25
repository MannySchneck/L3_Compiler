#include <parser.h>

#ifdef UNIT_TEST
#include "catch.hpp"
#endif // UNIT_TEST


using namespace L3;


void L3::curfun_instr_push(Program& p, L3_ptr<Instruction> inst){
     p.functions.back()->instructions.push_back(inst);
}


Program L3::parse_file (std::string fileName){

        /*
         * Check the grammar for some possible issues.
         */
        pegtl::analyze< L3::grammar >();

        /*
         * Parse.
         */

        Program p;
        L3_Parse_Stack<L3_ptr<AST_Item>> the_stack;
        std::vector<Binop::Op> op_stack;
        std::vector<Runtime_Fun::Fun> fun_stack;


        pegtl::file_parser(fileName).parse< L3::grammar, L3::action>(p,
                                                                     the_stack,
                                                                     op_stack,
                                                                     fun_stack);

        return p;
}


#ifdef UNIT_TEST

std::string ptestdir{"/home/manny/322/hw/322_framework/L3/src/tests/"};

TEST_CASE("Can parse function shell"){
     std::string ptest1 = ptestdir + "ptest1.L3";
     Program p = parse_file(ptest1);
     std::string text = slurp_file(ptest1);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse return with number return"){
     std::string ptest2 = ptestdir + "ptest2.L3";
     Program p = parse_file(ptest2);
     std::string text = slurp_file(ptest2);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse return with var return"){
     std::string ptest3 = ptestdir + "ptest3.L3";
     Program p = parse_file(ptest3);
     std::string text = slurp_file(ptest3);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse multiple args"){
     std::string ptest4 = ptestdir + "ptest4.L3";
     Program p = parse_file(ptest4);
     std::string text = slurp_file(ptest4);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse Assigning numbers to variables"){
     std::string ptest5 = ptestdir + "ptest5.L3";
     Program p = parse_file(ptest5);
     std::string text = slurp_file(ptest5);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse Assigning addition to var"){
     std::string ptest6 = ptestdir + "ptest6.L3";
     Program p = parse_file(ptest6);
     std::string text = slurp_file(ptest6);
     Dump v;
     p.accept(v);

     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse jump and inst labels"){
     std::string ptest7 = ptestdir + "ptest7.L3";
     Program p = parse_file(ptest7);
     std::string text = slurp_file(ptest7);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse condjump and inst labels"){
     std::string ptest8 = ptestdir + "ptest8.L3";
     Program p = parse_file(ptest8);
     std::string text = slurp_file(ptest8);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}

TEST_CASE("Can parse all the calls"){
     std::string ptest9 = ptestdir + "ptest9.L3";
     Program p = parse_file(ptest9);
     std::string text = slurp_file(ptest9);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}

TEST_CASE("Loads, stores, call results, oh my..."){
     std::string ptest10 = ptestdir + "ptest10.L3";
     Program p = parse_file(ptest10);
     std::string text = slurp_file(ptest10);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}

TEST_CASE("test 11"){
     std::string ptest11 = ptestdir + "ptest11.L3";
     Program p = parse_file(ptest11);
     std::string text = slurp_file(ptest11);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}


TEST_CASE("test 12"){
     std::string ptest12 = ptestdir + "ptest12.L3";
     Program p = parse_file(ptest12);
     std::string text = slurp_file(ptest12);
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}

TEST_CASE("test 13"){
     std::string ptest13 = ptestdir + "ptest13.L3";
     Program p = parse_file(ptest13);
     std::string text = slurp_file(ptest13 + ".ex");
     Dump v;
     p.accept(v);
     REQUIRE(v.result.str() == text);
}


#endif // UNIT_TEST
