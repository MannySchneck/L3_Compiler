#include <parser.h>
#include <string>

#ifdef UNIT_TEST
#include "catch.hpp"
#include "prettyprint.hpp"
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


        pegtl::file_parser(fileName).parse< L3::grammar, L3::action
                                            >(p,
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

TEST_CASE("Name Grabber"){
        std::string ptest10 = ptestdir + "ptest10.L3";
        Program p = parse_file(ptest10);

        auto first_fun = p.functions[0];

        auto names = first_fun->grabber_of_the_vars();

        auto expected_names = std::unordered_set<std::string>{"v1",
                                                              "v2",
                                                              "v4",
                                                              "v4Encoded",
                                                              "myAr",
                                                              "el1Address",
                                                              "el1",
                                                              "hi",
                                                              "mlorp",
                                                              "mlerp",
                                                              "no"};
        REQUIRE(names == expected_names);
}

TEST_CASE("labelgrabber"){
        std::string ptest10 = ptestdir + "ptest10.L3";
        Program p = parse_file(ptest10);

        auto first_fun = p.functions[0];

        auto names = first_fun->grabber_of_the_labels();

        auto expected_names = std::unordered_set<std::string>{"main",
                                                              "myF",
                                                              "this_is_a_label"};
        REQUIRE(names == expected_names);
}

TEST_CASE("prefixFinder"){
        std::string ptest10 = ptestdir + "ptest10.L3";
        Program p = parse_file(ptest10);

        auto first_fun = p.functions[0];

        auto names = first_fun->grabber_of_the_labels();

        REQUIRE(first_fun->find_prefix(names) == "z");
        REQUIRE(first_fun->find_prefix({"z", "zap"}) == "z0");
        REQUIRE(first_fun->find_prefix({"z0aplopp", "zap"}) == "z1");
        REQUIRE(first_fun->find_prefix({"z099aplopp",
                                        "z11ap",
                                        "z2",
                                        "z3",
                                        "z4",
                                        "z5",
                                        "z6",
                                        "z7",
                                        "z8",
                                        "z9"}) == "z90");
}


TEST_CASE("please. please. please.", "[!mayfail]"){
        std::string ptest10 = ptestdir + "ptest10.L3";
        Program p = parse_file(ptest10);

        auto first_fun = p.functions[0];

        auto the_prefix = std::string("some_prefix");

        REQUIRE(first_fun->enstringify_l2ishly([the_prefix](){

                                auto my_prefix = std::string(":");
                                my_prefix.append(the_prefix);

                                static int label_counter = 0;
                                my_prefix.append(std::string(std::to_string(label_counter)));
                                my_prefix.append("ret");
                                label_counter += 1;
                                return my_prefix;}) == "");
}


#endif // UNIT_TEST
