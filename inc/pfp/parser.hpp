#pragma once
#include "pfp/ast.hpp"
#include "pfp/lexer.hpp"
#include "pfp/token.hpp"
#include <memory>
#include <optional>
#include <string>

namespace pfp {
class parser_t {
	token_generator_t tokenizer;
	std::optional<token_t> currentToken;

	void advance();
	std::unique_ptr<ast_node_t> parseExpression();
	std::unique_ptr<ast_node_t> parseFunction();
	std::unique_ptr<ast_class_node_t> parseClass();
	std::unique_ptr<ast_preprocessor_node_t> parsePreprocessor();
	std::unique_ptr<ast_template_node_t> parseTemplate();

  public:
	explicit parser_t(token_generator_t tokenizer);
	std::vector<std::unique_ptr<ast_node_t>> parse();
};
}	 // namespace pfp
