#ifndef MVL_HPP
#define MVL_HPP

#include <ast-lang-2/ast/node.hpp>
#include <ast-lang-2/interpreter/interpreter.hpp>
#include <ccl/ccl.hpp>
#include <ccl/parser/types.hpp>
#include <isl/coroutine/task.hpp>
#include <isl/string_view.hpp>
#include <mvl/ast/math_node.hpp>
#include <numbers>

#define MVL_DECL CCL_DECL
#define MVL_INLINE CCL_INLINE
#define MVL_LIFETIMEBOUND CCL_LIFETIMEBOUND
#define MVL_TRIVIAL_ABI CCL_TRIVIAL_ABI
#define MVL_NOEXCEPT_IF(EXPR) CCL_NOEXCEPT_IF(EXPR)

namespace mvl
{
    using i8 = ccl::i8;
    using i16 = ccl::i16;
    using i32 = ccl::i32;
    using i64 = ccl::i64;

    using u8 = ccl::u8;
    using u16 = ccl::u16;
    using u32 = ccl::u32;
    using u64 = ccl::u64;

    using SmallId = ccl::SmallId;
    using State = ccl::parser::State;
    using Symbol = ccl::parser::Symbol;

    using f32 = ccl::f32;
    using f64 = ccl::f64;

    auto parse(isl::string_view source, isl::string_view filename)
        -> isl::Task<astlang2::ast::SharedNode<astlang2::ast::AstlangNode>>;

    auto newInterpreter(std::back_insert_iterator<std::string> output_buffer)
        -> astlang2::interpreter::Interpreter;

    auto solve(isl::string_view input, double x, double y) -> double;

    auto derivativeX(isl::string_view input, double x, double y) -> double;

    auto derivativeY(isl::string_view input, double x, double y) -> double;

    auto solve(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y) -> double;

    auto derivativeX(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y)
        -> double;

    auto derivativeY(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y)
        -> double;

    auto constructRoot(isl::string_view input) -> ccl::parser::ast::SharedNode<ast::MathNode>;
} // namespace mvl

#endif /* MVL_HPP */
