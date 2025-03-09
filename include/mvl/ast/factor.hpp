#ifndef FACTOR_HPP
#define FACTOR_HPP

#include <ccl/parser/ast/terminal.hpp>
#include <mvl/ast/math_node.hpp>

namespace ccl::parser::ast
{
    class Terminal;
}
namespace mvl::ast
{
    class Factor final : public MathNode
    {
    public:
        ccl::parser::ast::SharedNode<> node;

        Factor(const ccl::SmallId id, const ccl::parser::ast::SmallVectorOfNodes &initial_nodes)
          : MathNode{id}
        {
            if (initial_nodes.size() == 1) {
                node = initial_nodes[0];
            } else {
                node = initial_nodes[1];
            }
        }

        auto derivationX(const double x, const double y) const -> double override
        {
            if (node->getType() == MathConstructor.getRuleId("NUMBER")) {
                return 0.0;
            }

            if (node->getType() == MathConstructor.getRuleId("X")) {
                return 1.0;
            }

            if (node->getType() == MathConstructor.getRuleId("FLOAT")) {
                return 0.0;
            }

            if (node->getType() == MathConstructor.getRuleId("Y")) {
                return 0.0;
            }

            return callDerivationX(node.get(), x, y);
        }

        auto derivationY(const double x, const double y) const -> double override
        {
            if (node->getType() == MathConstructor.getRuleId("NUMBER")) {
                return 0.0;
            }

            if (node->getType() == MathConstructor.getRuleId("X")) {
                return 0.0;
            }

            if (node->getType() == MathConstructor.getRuleId("FLOAT")) {
                return 0.0;
            }

            if (node->getType() == MathConstructor.getRuleId("Y")) {
                return 1.0;
            }

            return callDerivationY(node.get(), x, y);
        }

        auto compute(const double x, const double y) const -> double override
        {
            if (node->getType() == MathConstructor.getRuleId("NUMBER")) {
                return std::stod(
                    std::string{dynamic_cast<ccl::parser::ast::Terminal *>(node.get())
                                    ->getToken()
                                    .getRepr()});
            }

            if (node->getType() == MathConstructor.getRuleId("FLOAT")) {
                return std::stod(
                    std::string{dynamic_cast<ccl::parser::ast::Terminal *>(node.get())
                                    ->getToken()
                                    .getRepr()});
            }

            if (node->getType() == MathConstructor.getRuleId("X")) {
                return x;
            }

            if (node->getType() == MathConstructor.getRuleId("Y")) {
                return y;
            }

            return callCompute(node.get(), x, y);
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

#endif /* FACTOR_HPP */
