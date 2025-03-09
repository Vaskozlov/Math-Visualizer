#ifndef COS_HPP
#define COS_HPP

#include <mvl/ast/function.hpp>

namespace mvl::ast
{
    class Cos final : public Function
    {
    public:
        ccl::parser::ast::SharedNode<> node;

        Cos(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : Function{
                id, initial_nodes[1], isl::SmallFunction<double(double)>{[](const double value) {
                    return std::cos(value);
                }},
                isl::SmallFunction<double(double)>{[](const double value) {
                    return -std::sin(value);
                }}}
        {}
    };
}// namespace mvl::ast

#endif /* COS_HPP */
