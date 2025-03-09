#ifndef SIN_HPP
#define SIN_HPP

#include <mvl/ast/function.hpp>

namespace mvl::ast
{
    class Sin final : public Function
    {
    public:
        ccl::parser::ast::SharedNode<> node;

        Sin(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : Function{
                id, initial_nodes[1], isl::SmallFunction<double(double)>{[](const double value) {
                    return std::sin(value);
                }},
                isl::SmallFunction<double(double)>{[](const double value) {
                    return std::cos(value);
                }}}
        {}
    };
}// namespace mvl::ast


#endif /* SIN_HPP */
