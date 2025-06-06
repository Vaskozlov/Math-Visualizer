#ifndef MVL_AST_COS_HPP
#define MVL_AST_COS_HPP

#include <mvl/ast/function.hpp>

namespace mvl::ast
{
    class Cos final : public Function
    {
    public:
        ccl::parser::ast::SharedNode<> node;

        Cos(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : Function{id, initial_nodes[1], [](const double value) {
                         return std::cos(value);
                     }, [](const double value) { return -std::sin(value); }}
        {}
    };
} // namespace mvl::ast

#endif /* COS_HPP */
