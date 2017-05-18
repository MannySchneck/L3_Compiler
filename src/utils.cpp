#include <utils.h>
#include <fstream>
#include <sstream>

using namespace L3;

std::string slurp_file(std::string filename){
        std::stringstream ss;

        std::ifstream f{filename};

        ss << f.rdbuf();

        return ss.str();
}
