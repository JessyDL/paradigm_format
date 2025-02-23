#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "pfp/token.hpp"


namespace pfp {

class lexer_t final {
  private:
	std::vector<std::pair<std::unique_ptr<tokenizer_state_t>, std::string_view>> states;
	std::string errorBuffer {};

  public:
	lexer_t();
	// @brief Tokenize the input string
	// @param input The input string to tokenize
	// @return A token_generator_t that can be used to iterate over the tokens
	// @details This function will tokenize the input string and return a token_generator_t that can be used to iterate
	// over the tokens. Internally all the tokenization states are tried in order and the one that consumes the most
	// characters is chosen. If there are multiple states that consume the same number of characters, an error is
	// generated.
	token_generator_t tokenize(std::string_view input);
};
}	 // namespace pfp
