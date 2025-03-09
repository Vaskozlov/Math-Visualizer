#ifndef MATH_NODE_HPP
#define MATH_NODE_HPP

#include <ccl/lexer/tokenizer.hpp>
#include <ccl/parser/dot/dot_repr.hpp>
#include <ccl/parser/rules_reader/rules_reader.hpp>
#include <isl/io.hpp>
#include <mvl/math_grammar.hpp>

namespace mvl
{
    extern ccl::parser::reader::RulesReader MathRulesReader;
    extern ccl::parser::reader::ParserBuilder &MathConstructor;
}// namespace mvl

namespace mvl::ast
{
    class MathNode : public ccl::parser::ast::Node
    {
    public:
        using Node::Node;

        using ChildrenNodesGenerator = isl::SmallFunction<ccl::parser::ast::SharedNode<>()>;

        [[nodiscard]] virtual auto compute(double x, double y) const -> double = 0;

        [[nodiscard]] static auto
            buildConversionTable(const ccl::parser::reader::ParserBuilder &constructor)
                -> ConversionTable;

        [[nodiscard]] virtual auto derivationX(double x, double y) const -> double = 0;

        [[nodiscard]] virtual auto derivationY(double x, double y) const -> double = 0;

        [[nodiscard]] static auto callCompute(const Node *node, const double x, const double y)
            -> double
        {
            return static_cast<const MathNode *>(node)->compute(x, y);
        }

        [[nodiscard]] static auto callDerivationX(const Node *node, const double x, const double y)
            -> double
        {
            return static_cast<const MathNode *>(node)->derivationX(x, y);
        }

        [[nodiscard]] static auto callDerivationY(const Node *node, const double x, const double y)
            -> double
        {
            return static_cast<const MathNode *>(node)->derivationY(x, y);
        }

        auto print(
            const std::string &prefix,
            bool is_left,
            const std::function<std::string(ccl::SmallId)> &id_converter) const -> void override;
    };
}// namespace mvl::ast

#endif /* MATH_NODE_HPP */
