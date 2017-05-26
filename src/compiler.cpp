#include <parser.h>
#include <fstream>

#ifndef UNIT_TEST
int main(int argc, char** argv){

        if(argc != 1){
                cerr << "USAGE: " << argv[0] << " <source file>";
        }

        Program p = parse_file(std::string(argv[1]));

        std::ofstream shiny_new_prog("prog.L2");

        shiny_new_prog << "(" << ":main";

        for(auto fun : p.functions){
                shiny_new_prog << fun.enstringify_L2ishly();
        }

        shiny_new_prog << ")";

        return 0;
}
#endif
;;
