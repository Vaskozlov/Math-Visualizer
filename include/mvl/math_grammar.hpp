#ifndef MATH_GRAMMAR_HPP
#define MATH_GRAMMAR_HPP

#include <ccl/ccl.hpp>
#include <isl/string_view.hpp>

namespace mvl
{
    enum struct NodeTypes : ccl::SmallId
    {
        FACTOR = 128,
        NUMBER,
        FLOAT,
        STATEMENT,
        SIN,
        COS,
        EXPRESSION,
        TERM,
        POWER,
        VALUE,
        NEGATE,
        MUL,
        DIV,
        ADD,
        SUB,
        X
    };

    extern const isl::string_view MathGrammar;

    auto getMathNodeTypesMap() -> const std::unordered_map<std::string, ccl::SmallId> &;
}// namespace mvl

#endif /* MATH_GRAMMAR_HPP */
