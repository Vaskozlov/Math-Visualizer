#include <ast-lang-2/ast-lang.hpp>
#include <ccl/lexer/tokenizer.hpp>
#include <ccl/parser/ll/gll_parser.hpp>
#include <ccl/parser/rules_reader/rules_reader.hpp>
#include <mvl/mvl.hpp>

namespace mvl {
    static ccl::parser::reader::RulesReader
    reader(astlang2::getNodeTypesMap(), astlang2::getAstlangGrammar());

    static ccl::parser::reader::ParserBuilder&constructor = reader.getParserBuilder();
    static const ccl::lexer::LexicalAnalyzer&lexer = constructor.getLexicalAnalyzer();
    static const auto token_to_string = constructor.getIdToNameTranslationFunction();
    static const ccl::parser::GllParser parser = constructor.buildGLL();
    static const auto conversion_table = astlang2::ast::AstlangNode::buildConversionTable(constructor);

    auto parse(const isl::string_view source, const isl::string_view filename)
        -> isl::Task<astlang2::ast::SharedNode<astlang2::ast::AstlangNode>> {
        auto tokenizer = lexer.getTokenizer(source, filename);
        auto [nodes, algorithm] = parser.parse(tokenizer);

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
        -> astlang2::interpreter::Interpreter {
        return astlang2::interpreter::Interpreter{constructor, output_buffer};
    }
} // namespace mvl
