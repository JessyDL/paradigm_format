#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>
#include <unordered_set>

namespace pfp {
class keyword_state_t : public tokenizer_state_t {
  private:
	static inline const std::unordered_set<std::string_view> keywords {"if",
																	   "else",
																	   "while",
																	   "for",
																	   "return",
																	   "int",
																	   "float",
																	   "char",
																	   "void",
																	   "struct",
																	   "class",
																	   "template",
																	   "typename"};

  public:
	DEBUG_STATE_NAME(keyword_state_t)
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i = start;
		if(!std::isalpha(input[i])) {
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
		}
		i++;
		while(i < input.size() && std::isalnum(input[i])) {
			i++;
		}

		std::string_view tokenStr = input.substr(start, i - start);
		if(keywords.find(tokenStr) != keywords.end()) {
			return {token_t {tokenStr, token_t::type_t::identifier},
					i - start,
					DEBUG_INJECT_STATE_NAME()};	   // Mark as keyword
		}

		if(i == start) {
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};	// Not a keyword
		}

		return {token_t {tokenStr, token_t::type_t::identifier},
				i - start,
				DEBUG_INJECT_STATE_NAME()};	   // Normal identifier
	}
};
}	 // namespace pfp
