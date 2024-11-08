#ifndef MVL_HPP
#define MVL_HPP

#include <ast-lang/interpreter/interpreter.hpp>
#include <ccl/ccl.hpp>
#include <ccl/parser/types.hpp>
#include <isl/string_view.hpp>

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
        -> astlang::ast::SharedNode<astlang::ast::Node>;

    auto newInterpreter(std::back_insert_iterator<std::string> output_buffer) -> astlang::interpreter::Interpreter;
}// namespace mvl

#endif /* MVL_HPP */
