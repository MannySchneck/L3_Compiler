#include <memory>
#include <deque>
#include <string>
#include <sstream>

namespace L3{
     template <typename T>
     using compiler_ptr = std::shared_ptr<T>;

     template <typename T>
     class L3_Parse_Stack{
     public:
          void push(T item);

          template<typename expected_T>
          compiler_ptr<expected_T> downcast_pop();

          T pop();

          bool is_empty() const;

          const T& peek() const;

          void NUKE();

     private:
          std::deque<T> instr_elements;
     };

}

#include <parse_stack.tpp>
