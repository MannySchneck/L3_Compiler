#pragma once

#include <parse_stack.h>
#include <pegtl.hh>
#include <pegtl/trace.hh>
#include <pegtl/analyze.hh>
#include <pegtl/contrib/raw_string.hh>

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
        struct and_op :
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

        struct runtime_fun :
                pegtl::sor<
                pegtl_string_t("print"),
                pegtl_string_t("allocate"),
                pegtl_string_t("array_error")
                >
        {};

        struct u :
                pegtl::sor<
                var,
                label
                >
        {};

        // operands
        struct binop :
                pegtl::raise<binop>
        {};

        struct load :
                pegtl::raise<load>
        {};

        struct store :
                pegtl::raise<store>
        {};

        struct operand :
                pegtl::sor<
                load,
                store,
                binop
                >
        {};

        //instructions
        struct assignment :
                pegtl::raise<assignment>
        {};

        struct goto_ :
                pegtl::raise<goto_>
        {};

        struct cjump :
                pegtl::raise<cjump>
        {};

        struct callable :
                pegtl::sor<
                u,
                runtime_fun
                >
        {};

        struct call :
                pegtl::if_must<
                pegtl_string_t("call"),
                callable
                >
        {};

        struct return_ :
                pegtl::if_must<
                pegtl_string_t("return"),
                pegtl::opt<t>
                >
        {};

        struct instruction :
                pegtl::sor<
                label,
                int_literal,
                assignment,
                store,
                goto_,
                cjump,
                call,
                return_
                >
        {};

        struct instructions :
                pegtl::plus<
                pegtl::seq<seps,
                           instruction>
                >
        {};

        template<typename T, typename Delim>
        struct separated_list_multiple:
                seq<
                T,
                pegtl::star<
                        Delim,
                        seps,
                        T
                        >
                >
        {};

        Template<typename T, typename Delim>
        struct arg_list :
                opt<
                pegtl::sor<
                        T,
                        separated_list<T, Delim>
                        >
                >
        {};

        template <typename T, typename Surrounder_open, typename Surrounder_close>
        struct surrounded_list :
                pegtl::if_must<
                Surrounder_open,
                seps,
                ,
                seps,
                Surrounder_close
                >
        {};


        struct function :
                pegtl::if_must<
                pegtl_string_t("define"),
                        seps,

        {};

        struct program :
                pegtl::plus<function>
        {};

        struct grammar :
                pegtl::must<program>
        {};

}
