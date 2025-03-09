#ifndef TERM_HPP
#define TERM_HPP

#include <mvl/ast/math_node.hpp>

namespace mvl::ast
{
    class Term : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> lhs;
        ccl::parser::ast::SharedNode<> rhs;
        std::function<double(double, double)> function;
        ccl::SmallId type{std::to_underlying(mvl::NodeTypes::TERM)};

        Term(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : MathNode{id}
        {
            lhs = initial_nodes[0];

            if (initial_nodes.size() != 1) {
                rhs = initial_nodes[2];
                type = initial_nodes[1]->getType();

                if (type == MathConstructor.getRuleId("\'*\'")) {
                    function = [](double x, double y) {
                        return x * y;
                    };
                } else {
                    function = [](double x, double y) {
                        return x / y;
                    };
                }
            }
        }

        auto derivationX(const double x, const double y) const -> double override
        {
            auto left_der = callDerivationX(lhs.get(), x, y);
            auto left_val = callCompute(lhs.get(), x, y);

            if (rhs != nullptr) {
                auto right_der = callDerivationX(rhs.get(), x, y);
                auto right_val = callCompute(rhs.get(), x, y);

                if (type == MathConstructor.getRuleId("\'*\'")) {
                    return left_der * right_val + right_der * left_val;
                }

                return (left_der * right_val - left_val * right_der) / (right_val * right_val);
            }

            return left_der;
        }

        auto derivationY(const double x, const double y) const -> double override
        {
            auto left_der = callDerivationY(lhs.get(), x, y);
            auto left_val = callCompute(lhs.get(), x, y);

            if (rhs != nullptr) {
                auto right_der = callDerivationY(rhs.get(), x, y);
                auto right_val = callCompute(rhs.get(), x, y);

                if (type == MathConstructor.getRuleId("\'*\'")) {
                    return left_der * right_val + right_der * left_val;
                }

                return (left_der * right_val - left_val * right_der) / (right_val * right_val);
            }

            return left_der;
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
            return ChildrenNodesGenerator{
                [index = 0, this]() mutable -> ccl::parser::ast::SharedNode<> {
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
                }};
        }
    };
}// namespace mvl::ast


#endif /* TERM_HPP */
