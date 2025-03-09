#include <ast-lang-2/ast/node.hpp>
#include <ast-lang-2/interpreter/interpreter.hpp>
#include <cassert>
#include <ccl/lexer/tokenizer.hpp>
#include <ccl/parser/dot/dot_repr.hpp>
#include <ccl/parser/ll/gll_parser.hpp>
#include <ccl/parser/lr/glr_parser.hpp>
#include <ccl/parser/lr/lr_parser.hpp>
#include <ccl/parser/rules_reader/rules_reader.hpp>
#include <isl/io.hpp>
#include <mvl/math_grammar.hpp>

namespace mvl
{
    constexpr isl::string_view MathGrammar = R"(
%LEXER%
FLOAT:      [0-9]+[.][0-9]+
NUMBER:     [0-9]+
X:          ! [x]
Y:          ! [y]

%PARSER%

GOAL:
    EXPRESSION

EXPRESSION:
    EXPRESSION '+' TERM
    | EXPRESSION '-' TERM
    | TERM

TERM:
    TERM '*' POWER
    | TERM '/' POWER
    | POWER

POWER:
    POWER '^' VALUE
    | VALUE

VALUE:
    '-' VALUE
    | '+' VALUE
    | FACTOR

FACTOR:
    NUMBER
    | FLOAT
    | '(' EXPRESSION ')'
    | SIN
    | COS
    | TG
    | X
    | Y

SIN:
    'sin' FACTOR

COS:
    'cos' FACTOR

TG:
    'tg' FACTOR

)";

    auto getMathNodeTypesMap() -> const std::unordered_map<std::string, ccl::SmallId> &
    {
        const static std::unordered_map<std::string, ccl::SmallId> node_types_map{
            // NOSONAR
            {
                "\"NUMBER\"",
                std::to_underlying(NodeTypes::NUMBER),
            },
            {
                "\"FACTOR\"",
                std::to_underlying(NodeTypes::FACTOR),
            },
            {
                "\"SIN\"",
                std::to_underlying(NodeTypes::SIN),
            },
            {
                "\"COS\"",
                std::to_underlying(NodeTypes::COS),
            },
            {
                "\"EXPRESSION\"",
                std::to_underlying(NodeTypes::EXPRESSION),
            },
            {
                "\"TERM\"",
                std::to_underlying(NodeTypes::TERM),
            },
            {
                "\"POWER\"",
                std::to_underlying(NodeTypes::COS),
            },
            {
                "\"FLOAT\"",
                std::to_underlying(NodeTypes::FLOAT),
            },
            {
                "\"STATEMENT\"",
                std::to_underlying(NodeTypes::STATEMENT),
            },
            {
                "\'*\'",
                std::to_underlying(NodeTypes::MUL),
            },
            {
                "\'/\'",
                std::to_underlying(NodeTypes::DIV),
            },
            {
                "\'+\'",
                std::to_underlying(NodeTypes::ADD),
            },
            {
                "\'-\'",
                std::to_underlying(NodeTypes::SUB),
            },
            {
                "X",
                std::to_underlying(NodeTypes::X),
            },
        };

        return node_types_map;
    }
}// namespace mvl