#ifndef MVL_AST_VALUE_HPP
#define MVL_AST_VALUE_HPP

#include <mvl/ast/math_node.hpp>

namespace mvl::ast
{
    class Value : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> node;
        std::function<double(double)> function;

        Value(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : MathNode{id}
        {
            if (initial_nodes.size() == 1) {
                function = [](const double x) {
                    return x;
                };
                node = initial_nodes[0];
                return;
            }

            if (initial_nodes[0]->getType() == std::to_underlying(mvl::NodeTypes::SUB)) {
                function = std::negate<double>{};
            } else {
                function = [](const double value) {
                    return value;
                };
            }

            node = initial_nodes[1];
        }

        auto derivationX(const double x, const double y) const -> double override
        {
            return function(callDerivationX(node.get(), x, y));
        }

        auto derivationY(const double x, const double y) const -> double override
        {
            return function(callDerivationY(node.get(), x, y));
        }

        auto compute(const double x, const double y) const -> double override
        {
            return function(callCompute(node.get(), x, y));
        }

        auto getChildrenNodes() const
            -> isl::SmallFunction<ccl::parser::ast::SharedNode<>()> override
        {
            return [index = 0, node = this->node]() mutable -> ccl::parser::ast::SharedNode<> {
                if (index > 0) {
                    return nullptr;
                }

                ++index;

                return node;
            };
        }
    };

}// namespace mvl::ast

#endif /* VALUE_HPP */
