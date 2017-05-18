#include <memory>
#include <deque>
#include <string>
#include <sstream>

namespace L2{
        template <typename T>
        using compiler_ptr = std::shared_ptr<T>;

        template <typename T>
        class L2_Parse_Stack{
        public:
                void push(T item);

                template<typename expected_T>
                compiler_ptr<expected_T> downcast_pop();

                T pop();

                const T& peek() const;

                void NUKE();

        private:
                std::deque<T> instr_elements;
        };

}

#include <parse_stack.tpp>
