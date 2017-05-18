#include <parser.h>

#ifdef UNIT_TEST
#include "../tests/catch/catch.hpp"
#endif // UNIT_TEST


using namespace L3

Program parse_file (std::string fileName){

        /*
         * Check the grammar for some possible issues.
         */
        pegtl::analyze< L3::grammar >();

        /*
         * Parse.
         */

        Program p;
        pegtl::file_parser(fileName).parse< L3::grammar, L3::action>(p);

        return p;
}

Function parse_function_file (std::string fileName){

        /*
         * Check the grammar for some possible issues.
         */
        pegtl::analyze< L3::function>();

        /*
         * Parse.
         */

        Program p;
        pegtl::file_parser(fileName).parse< L3::L3_function_rule, L3::action>(p);

        return (std::move(*p.functions.back()));
}


#ifdef UNIT_TEST

TEST_CASE("Can parse function shell"){
        std::string ptest1{"tests/ptest1.L3"};

        Function f = parse_function_file(ptest1);
        std::string text = slurp_file(ptest1);

        Dump v;
        f.accept(v);

        require(v.result == text);
}

#endif // UNIT_TEST
