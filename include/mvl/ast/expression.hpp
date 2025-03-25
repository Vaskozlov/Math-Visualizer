#ifndef MVL_AST_EXPRESSION_HPP
#define MVL_AST_EXPRESSION_HPP

#include <mvl/ast/math_node.hpp>

namespace mvl::ast
{
    class Expression : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> lhs;
        ccl::parser::ast::SharedNode<> rhs;
        std::function<double(double, double)> function;

        Expression(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : MathNode{id}
        {
            lhs = initial_nodes[0];

            if (initial_nodes.size() != 1) {
                rhs = initial_nodes[2];

                if (initial_nodes[1]->getType() == MathConstructor.getRuleId("\'+\'")) {
                    function = std::plus<double>{};
                } else {
                    function = std::minus<double>{};
                }
            }
        }

        auto derivationX(double x, double y) const -> double override
        {
            auto left = static_cast<const MathNode *>(lhs.get())->derivationX(x, y);

            if (rhs != nullptr) {
                auto right = static_cast<MathNode *>(this->rhs.get())->derivationX(x, y);

                return function(left, right);
            }

            return left;
        }

        auto derivationY(const double x, const double y) const -> double override
        {
            const auto left = callDerivationY(lhs.get(), x, y);

            if (rhs != nullptr) {
                const auto right = callDerivationY(rhs.get(), x, y);

                return function(left, right);
            }

            return left;
        }

        auto compute(const double x, const double y) const -> double override
        {
            const auto left = callCompute(lhs.get(), x, y);

            if (rhs != nullptr) {
                const auto right = callCompute(rhs.get(), x, y);
                return function(left, right);
            }

            return left;
        }

        auto getChildrenNodes() const
            -> isl::SmallFunction<ccl::parser::ast::SharedNode<>()> override
        {
            return [index = 0, this]() mutable -> ccl::parser::ast::SharedNode<> {
                if (index > 1) {
                    return nullptr;
                }

                if (index == 0) {
                    ++index;
                    return lhs;
                }

                if (rhs != nullptr) {
                    ++index;
                    return rhs;
                }

                ++index;
                return nullptr;
            };
        }
    };
}// namespace mvl::ast

#endif /* EXPRESSION_HPP */
