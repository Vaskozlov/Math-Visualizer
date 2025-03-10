#ifndef MVL_AST_FUNCTION_HPP
#define MVL_AST_FUNCTION_HPP

#include <mvl/ast/math_node.hpp>

namespace mvl::ast
{
    class Function : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> node;
        isl::SmallFunction<double(double)> function;
        isl::SmallFunction<double(double)> derivation;

        Function(
            const ccl::SmallId id, const ccl::parser::ast::SharedNode<> &initial_node,
            isl::SmallFunction<double(double)> function,
            isl::SmallFunction<double(double)> derivation)
          : MathNode{id}
          , node{initial_node}
          , function{std::move(function)}
          , derivation{std::move(derivation)}
        {}

        auto derivationX(const double x, const double y) const -> double override
        {
            const auto inner_value = callCompute(node.get(), x, y);
            const auto outer_value = callDerivationX(node.get(), x, y);

            return derivation(inner_value) * outer_value;
        }

        auto derivationY(const double x, const double y) const -> double override
        {
            const auto inner_value = callCompute(node.get(), x, y);
            const auto outer_value = callDerivationY(node.get(), x, y);

            return derivation(inner_value) * outer_value;
        }

        auto compute(const double x, const double y) const -> double override
        {
            return function(callCompute(node.get(), x, y));
        }

        auto getChildrenNodes() const
            -> isl::SmallFunction<ccl::parser::ast::SharedNode<>()> override
        {
            return ChildrenNodesGenerator{
                [index = 0, this]() mutable -> ccl::parser::ast::SharedNode<> {
                    if (index > 0) {
                        return nullptr;
                    }

                    ++index;
                    return node;
                }};
        }
    };
}// namespace mvl::ast

#endif /* FUNCTION_HPP */
