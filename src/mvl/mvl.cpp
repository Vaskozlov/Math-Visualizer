#include <ast-lang-2/ast-lang.hpp>
#include <ccl/lexer/tokenizer.hpp>
#include <ccl/parser/ll/gll_parser.hpp>
#include <ccl/parser/rules_reader/rules_reader.hpp>
#include <mvl/mvl.hpp>

namespace mvl
{
    ccl::parser::reader::RulesReader MathRulesReader(getMathNodeTypesMap(), MathGrammar);
    ccl::parser::reader::ParserBuilder &MathConstructor = MathRulesReader.getParserBuilder();

    static ccl::parser::reader::RulesReader
        reader(astlang2::getNodeTypesMap(), astlang2::getAstlangGrammar());

    static ccl::parser::reader::ParserBuilder &constructor = reader.getParserBuilder();
    static const ccl::lexer::LexicalAnalyzer &lexer = constructor.getLexicalAnalyzer();
    static const auto token_to_string = constructor.getIdToNameTranslationFunction();
    static const ccl::parser::GllParser mvlParser = constructor.buildGLL();
    static const auto conversion_table =
        astlang2::ast::AstlangNode::buildConversionTable(constructor);

    auto parse(const isl::string_view source, const isl::string_view filename)
        -> isl::Task<astlang2::ast::SharedNode<astlang2::ast::AstlangNode>>
    {
        auto tokenizer = lexer.getTokenizer(source, filename);
        auto [nodes, algorithm] = mvlParser.parse(tokenizer);

        if (nodes.size() != 1) {
            throw std::runtime_error{"Failed to parse the input"};
        }

        auto row_root = nodes.front();
        row_root->cast(conversion_table);

        auto root = isl::staticPointerCast<astlang2::ast::AstlangNode>(std::move(row_root));
        root->optimize();

        co_return root;
    }

    auto newInterpreter(const std::back_insert_iterator<std::string> output_buffer)
        -> astlang2::interpreter::Interpreter
    {
        return astlang2::interpreter::Interpreter{constructor, output_buffer};
    }

    auto solve(const isl::string_view input, const double x, const double y) -> double
    {
        return solve(constructRoot(input), x, y);
    }

    auto derivativeX(const isl::string_view input, const double x, const double y) -> double
    {
        return derivativeX(constructRoot(input), x, y);
    }

    auto derivativeY(const isl::string_view input, const double x, const double y) -> double
    {
        return derivativeY(constructRoot(input), x, y);
    }

    auto solve(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y) -> double
    {
        return root->compute(x, y);
    }

    auto derivativeX(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y) -> double
    {
        return root->derivationX(x, y);
    }

    auto derivativeY(ccl::parser::ast::SharedNode<ast::MathNode> root, double x, double y) -> double
    {
        return root->derivationY(x, y);
    }

    auto constructRoot(const isl::string_view input) -> ccl::parser::ast::SharedNode<ast::MathNode>
    {
        static const auto &mvl_lexer = MathConstructor.getLexicalAnalyzer();
        static auto to_str = MathConstructor.getIdToNameTranslationFunction();
        static auto parser = MathConstructor.buildGLL();

        auto tokenizer = mvl_lexer.getTokenizer(input, "");
        auto [nodes, algorithm] = parser.parse(tokenizer);
        assert(nodes.size() < 2);

        nodes.front()->print("", false, to_str);

        const auto cvt_table = ast::MathNode::buildConversionTable(MathConstructor);
        auto *row_root = nodes.front().get();
        row_root->cast(cvt_table);

        nodes.front().updateDeleter<ccl::parser::ast::Node>();

        return isl::staticPointerCast<ast::MathNode>(nodes.front());
    }
}// namespace mvl
