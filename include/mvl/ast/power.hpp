#ifndef MVL_AST_POWER_HPP
#define MVL_AST_POWER_HPP

#include <mvl/ast/math_node.hpp>

namespace mvl::ast
{
    class Power : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> node;
        ccl::parser::ast::SharedNode<> power;

        Power(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : MathNode{id}
        {
            node = initial_nodes[0];

            if (initial_nodes.size() != 1) {
                power = initial_nodes[2];
            }
        }

        auto derivationX(const double x, const double y) const -> double override
        {
            // if (power == nullptr) {
            //     return callDerivationX(node.get(), x, y);
            // }
            //
            // auto f = callCompute(node.get(), x, y);
            // auto f_derivation = callDerivationX(node.get(), x, y);
            //
            // auto g = callCompute(power.get(), x, y);
            // auto g_derivation = callDerivationX(power.get(), x, y);
            //
            // return std::pow(f, g) * (g * f_derivation / f + std::log(f) * g_derivation);

            const auto inner_derivation = callDerivationX(node.get(), x, y);
            const auto inner_value = callCompute(node.get(), x, y);

            if (power != nullptr) {
                auto power = callCompute(this->power.get(), x, y);

                return inner_derivation * power * std::pow(inner_value, power - 1);
            }

            return inner_derivation;
        }

        auto derivationY(double x, double y) const -> double override
        {
            const auto inner_derivation = callDerivationY(node.get(), x, y);
            const auto inner_value = callCompute(node.get(), x, y);

            if (power != nullptr) {
                auto power = callCompute(this->power.get(), x, y);

                return inner_derivation * power * std::pow(inner_value, power - 1);
            }

            return inner_derivation;
        }

        auto compute(const double x, const double y) const -> double override
        {
            const auto value = callCompute(node.get(), x, y);

            if (power != nullptr) {
                const auto p = callCompute(power.get(), x, y);
                return std::pow(value, p);
            }

            return value;
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
                    return node;
                }

                if (power != nullptr) {
                    ++index;
                    return power;
                }

                ++index;
                return nullptr;
            };
        }
    };
}// namespace mvl::ast


#endif /* POWER_HPP */
