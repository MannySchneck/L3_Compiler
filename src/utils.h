#pragma once

#include <memory>
#include <string>

namespace L3{
        template <typename T>
        using L3_ptr = std::shared_ptr<T>;

        std::string slurp_file(std::string filename);
}
