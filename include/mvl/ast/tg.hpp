#ifndef TG_HPP
#define TG_HPP

#include <mvl/ast/function.hpp>

namespace mvl::ast
{
    class Tg final : public Function
    {
    public:
        ccl::parser::ast::SharedNode<> node;

        Tg(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : Function{
                id, initial_nodes[1], isl::SmallFunction<double(double)>{[](const double value) {
                    return std::tan(value);
                }},
                isl::SmallFunction<double(double)>{[](const double value) {
                    const auto c = std::cos(value);
                    return 1.0 / (c * c);
                }}}
        {}
    };
}// namespace mvl::ast


#endif /* TG_HPP */
