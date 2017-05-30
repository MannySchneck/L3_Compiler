#include <parser.h>
#include <fstream>
#include <set>
#include <unordered_set>
#include <string>

using namespace L3;

#ifndef UNIT_TEST
int main(int argc, char** argv){

        if(argc != 2){
                std::cerr << "USAGE: " << argv[0] << " <source file>";
        }

        Program p = parse_file(std::string(argv[1]));

        std::ofstream shiny_new_prog("prog.L2");

        shiny_new_prog << "(" << ":main" << "\n\n";

        // scopify labels
        std::unordered_set<std::string> final_label_names;
        for(auto fun : p.functions){
                auto fun_label_names = fun->grabber_of_the_labels();
                final_label_names.insert(fun_label_names.begin(), fun_label_names.end());
        }
        auto scoping_prefix = Function::find_prefix(final_label_names);

        std::set<std::string> globally_scoped_names;
        for(auto fun : p.functions){
                globally_scoped_names.insert(fun->name.name);
        }

        for(int i = 0; i < p.functions.size(); i++){
                auto fun_prefix = scoping_prefix.append(std::to_string(i));
                p.functions[i]->scopify_labels(fun_prefix, globally_scoped_names);
        }

        // Make return labels, tile, and output L2
        std::unordered_set<std::string> label_names;
        for(auto fun : p.functions){
                auto fun_label_names = fun->grabber_of_the_labels();
                label_names.insert(fun_label_names.begin(), fun_label_names.end());
        }
        auto the_prefix = Function::find_prefix(label_names);

        auto retlabel_maker = [the_prefix](){
                auto my_prefix = std::string(":");
                my_prefix.append(the_prefix);
                static int label_counter = 0;
                my_prefix.append(std::string(std::to_string(label_counter)));
                my_prefix.append("ret");
                label_counter += 1;
                return my_prefix;};


        for(auto fun : p.functions){
                shiny_new_prog << fun->enstringify_l2ishly(retlabel_maker);
                shiny_new_prog << "\n";
        }


        shiny_new_prog << "\n)\n";

        return 0;
}
#endif
