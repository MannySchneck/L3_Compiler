#include <utils.h>
#include <fstream>
#include <sstream>

using namespace L3;

std::string L3::slurp_file(std::string filename){
        std::stringstream ss;

        std::ifstream f{filename};

        ss << f.rdbuf();

        return ss.str();
}
