#include <mvl/ast/cos.hpp>
#include <mvl/ast/expression.hpp>
#include <mvl/ast/factor.hpp>
#include <mvl/ast/math_node.hpp>
#include <mvl/ast/power.hpp>
#include <mvl/ast/sin.hpp>
#include <mvl/ast/term.hpp>
#include <mvl/ast/tg.hpp>
#include <mvl/ast/value.hpp>

namespace mvl::ast
{
    auto MathNode::print(
        const std::string &prefix,
        bool is_left,
        const std::function<std::string(ccl::SmallId)> &id_converter) const -> void
    {
        const auto expanded_prefix = expandPrefix(prefix, is_left);
        std::cout << fmt::format(
                         "{}NodeSequence-{}",
                         getPrintingPrefix(prefix, is_left),
                         id_converter(getType()))
                  << std::endl;

        auto generator = getChildrenNodes();
        ccl::parser::ast::SharedNode<> next = generator();

        while (next != nullptr) {
            ccl::parser::ast::SharedNode<> node = std::move(next);
            next = generator();

            node->print(expanded_prefix, next != nullptr, id_converter);
        }
    }

    auto MathNode::buildConversionTable(const ccl::parser::reader::ParserBuilder &constructor)
        -> ConversionTable
    {
        return ConversionTable{
            {
                constructor.getRuleId("FACTOR"),
                ccl::parser::ast::NonTerminal::reconstructNode<Factor>,
            },
            {
                constructor.getRuleId("EXPRESSION"),
                ccl::parser::ast::NonTerminal::reconstructNode<Expression>,
            },
            {
                constructor.getRuleId("TERM"),
                ccl::parser::ast::NonTerminal::reconstructNode<Term>,
            },
            {
                constructor.getRuleId("POWER"),
                ccl::parser::ast::NonTerminal::reconstructNode<Power>,
            },
            {
                constructor.getRuleId("VALUE"),
                ccl::parser::ast::NonTerminal::reconstructNode<Value>,
            },
            {
                constructor.getRuleId("COS"),
                ccl::parser::ast::NonTerminal::reconstructNode<Cos>,
            },
            {
                constructor.getRuleId("SIN"),
                ccl::parser::ast::NonTerminal::reconstructNode<Sin>,
            },
            {
                constructor.getRuleId("TG"),
                ccl::parser::ast::NonTerminal::reconstructNode<Tg>,
            },
        };
    }

}// namespace mvl::ast