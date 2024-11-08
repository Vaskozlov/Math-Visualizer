#include <ccl/lexer/tokenizer.hpp>
#include <ccl/parser/ll/gll_parser.hpp>
#include <ccl/parser/rules_reader/rules_reader.hpp>
#include <mvl/mvl.hpp>

static ccl::parser::reader::RulesReader reader(astlang::AstLangGrammar);
static ccl::parser::reader::ParserBuilder &constructor = reader.getParserBuilder();
static const ccl::lexer::LexicalAnalyzer &lexer = constructor.getLexicalAnalyzer();
static const auto token_to_string = constructor.getIdToNameTranslationFunction();
static const ccl::parser::GllParser parser = constructor.buildGLL();
const auto conversion_table = astlang::ast::Node::buildConversionTable(constructor);

namespace mvl
{
    auto parse(const isl::string_view source, const isl::string_view filename)
        -> astlang::ast::SharedNode<astlang::ast::Node>
    {
        auto tokenizer = lexer.getTokenizer(source, filename);
        auto [nodes, algorithm] = parser.parse(tokenizer);

        if (nodes.size() != 1) {
            throw std::runtime_error{"Failed to parse the input"};
        }

        auto row_root = nodes.front();
        row_root->cast(conversion_table);

        auto root = isl::dynamicPointerCast<astlang::ast::Node>(std::move(row_root));
        root->runRecursiveOptimization();

        return root;
    }

    auto newInterpreter(const std::back_insert_iterator<std::string> output_buffer)
        -> astlang::interpreter::Interpreter
    {
        return astlang::interpreter::Interpreter{constructor, output_buffer};
    }
}// namespace mvl
