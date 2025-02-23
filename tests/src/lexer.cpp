#include <format>
#include <utility>

#include "catch2/catch_test_macros.hpp"
#include "catch2/generators/catch_generators.hpp"

#include "common.hpp"
#include "pfp/lexer.hpp"
#include "pfp/token.hpp"

static constexpr std::string_view TOKEN_TYPE_STR[std::to_underlying(pfp::token_t::type_t::_end)] =
  {"IDENTIFIER", "NUMBER", "SYMBOL", "WHITESPACE", "STRING", "COMMENT", "PREPROCESSOR", "TEMPLATE"};

std::vector<std::pair<std::string, std::string>> parse_csv(std::string_view csv, std::string_view filename) {
	std::vector<std::pair<std::string, std::string>> tokens;
	std::istringstream ss(csv.data());
	std::string token;
	std::vector<std::string> tempTokens;

	while(std::getline(ss, token, ',')) {
		token.erase(token.begin(),
					std::find_if(token.begin(), token.end(), [](unsigned char ch) { return !std::isspace(ch); }));
		if(token.empty()) {
			continue;
		}
		tempTokens.push_back(token);
	}

	for(size_t i = 0; i + 1 < tempTokens.size(); i += 2) {
		tokens.emplace_back(tempTokens[i], tempTokens[i + 1]);
	}

	if(tempTokens.size() % 2 != 0) {
		throw std::runtime_error(std::format("Odd number of tokens in CSV {}", filename));
	}

	return tokens;
}

TEST_CASE("lexer_test", "[lexer]") {
	auto data_filename = GENERATE("basic", "comments", "regression_whitespace_eof");
	auto data_file	   = std::format("data/lexer/{}.ptf", data_filename);
	auto csv_file	   = std::format("{}.csv", data_file);
	auto data		   = utils::load_data(data_file);
	auto csv		   = utils::load_data(csv_file);

	auto tokens = parse_csv(csv, csv_file);


	pfp::lexer_t tokenizer;
	auto gen = tokenizer.tokenize(data);

	auto csv_iter = tokens.begin();

	while(auto token_opt = gen.next()) {
		auto token = token_opt.value();

		if(token.type == pfp::token_t::type_t::whitespace || token.type == pfp::token_t::type_t::comment) {
			continue;
		}

		CAPTURE(TOKEN_TYPE_STR[std::to_underlying(token.type)], std::string(token.value));

		REQUIRE(csv_iter != tokens.end());
		auto const [csv_type, csv_value] = *csv_iter;
		CAPTURE(csv_type, csv_value);

		REQUIRE(csv_type == TOKEN_TYPE_STR[std::to_underlying(token.type)]);
		REQUIRE(csv_value == token.value);

		++csv_iter;
	}
}
