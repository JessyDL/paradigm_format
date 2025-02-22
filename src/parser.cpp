#include "pfp/parser.hpp"


namespace pfp {

parser_t::parser_t(token_generator_t tokenizer) : tokenizer(std::move(tokenizer)) {
	advance();	  // Load first token
}

std::unique_ptr<ast_node_t> parser_t::parseExpression() {
	if(!currentToken)
		return nullptr;

	if(currentToken->type == token_t::type_t::identifier) {
		std::string name = std::string(currentToken->value);
		advance();
		return std::make_unique<ast_identifier_node_t>(name);
	}

	if(currentToken->type == token_t::type_t::number) {
		std::string value = std::string(currentToken->value);
		advance();
		return std::make_unique<ast_number_literal_node_t>(value);
	}

	return nullptr;
}

std::unique_ptr<ast_node_t> parser_t::parseFunction() {
	auto templateNode = std::optional {parseTemplate()};
	if(!currentToken || currentToken->value != "function")
		return nullptr;
	advance();	  // Skip "function"

	if(!currentToken || currentToken->type != token_t::type_t::identifier)
		return nullptr;
	std::string functionName = std::string(currentToken->value);
	advance();

	if(!currentToken || currentToken->value != "(")
		return nullptr;
	advance();

	auto functionNode = std::make_unique<ast_function_node_t>(functionName, std::move(templateNode));

	while(currentToken && currentToken->type == token_t::type_t::identifier) {
		std::string paramType = std::string(currentToken->value);
		advance();
		if(!currentToken || currentToken->type != token_t::type_t::identifier)
			return nullptr;
		std::string paramName = std::string(currentToken->value);
		advance();
		functionNode->params.emplace_back(paramType, paramName);
		if(currentToken->value == ",")
			advance();
	}

	if(!currentToken || currentToken->value != ")") {
		return nullptr;
	}
	advance();	  // Skip ')'

	if(!currentToken || currentToken->value != "{") {
		return nullptr;
	}
	advance();	  // Skip '{'

	while(currentToken && currentToken->value != "}") {
		functionNode->body.push_back(parseExpression());
	}

	advance();	  // Skip '}'
	return functionNode;
}

std::unique_ptr<ast_class_node_t> parser_t::parseClass() {
	auto templateNode = std::optional {parseTemplate()};
	if(!currentToken || currentToken->value != "class") {
		return nullptr;
	}
	advance();

	if(!currentToken || currentToken->type != token_t::type_t::identifier) {
		return nullptr;
	}
	std::string className = std::string(currentToken->value);
	advance();

	auto classNode = std::make_unique<ast_class_node_t>(className, std::move(templateNode));

	if(!currentToken || currentToken->value != "{") {
		return nullptr;
	}
	advance();

	while(currentToken && currentToken->value != "}") {
		classNode->members.push_back(parseExpression());
	}

	advance();
	return classNode;
}

std::unique_ptr<ast_preprocessor_node_t> parser_t::parsePreprocessor() {
	if(!currentToken || currentToken->type != token_t::type_t::preprocessor) {
		return nullptr;
	}
	auto directive = std::make_unique<ast_preprocessor_node_t>(std::string(currentToken->value));
	advance();
	return directive;
}

std::unique_ptr<ast_template_node_t> parser_t::parseTemplate() {
	if(!currentToken || currentToken->value != "template") {
		return nullptr;
	}
	advance();	  // Move past `template`

	if(!currentToken || currentToken->type != token_t::type_t::variadic) {
		return nullptr;
	}
	auto value = currentToken->value;
	// remove the '<' and '>'
	value = value.substr(1, value.size() - 2);

	std::vector<std::string> templateParams;
	while(currentToken && currentToken->type == token_t::type_t::identifier) {
		templateParams.push_back(std::string(currentToken->value));
		advance();
		if(currentToken->value == ",")
			advance();
	}

	if(!currentToken || currentToken->value != ">")
		return nullptr;
	advance();	  // Move past `>`

	return std::make_unique<ast_template_node_t>(templateParams);
}

std::vector<std::unique_ptr<ast_node_t>> parser_t::parse() {
	std::optional<token_t> startToken = std::nullopt;
	std::vector<std::unique_ptr<ast_node_t>> nodes {};
	do {
		startToken = currentToken;
		if(auto res = parsePreprocessor(); res) {
			nodes.push_back(std::move(res));
		}
		if(auto res = parseFunction(); res) {
			nodes.push_back(std::move(res));
		}
		if(auto res = parseClass(); res) {
			nodes.push_back(std::move(res));
		}
	} while(currentToken && currentToken != startToken);
	return nodes;
}

void parser_t::advance() {
	currentToken = tokenizer.next();
	while(currentToken &&
		  (currentToken->type == token_t::type_t::whitespace || currentToken->type == token_t::type_t::comment)) {
		currentToken = tokenizer.next();
	}
}

}	 // namespace pfp
