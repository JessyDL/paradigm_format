#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <optional>
#include <string_view>

namespace pfp {
class whitespace_state_t : public tokenizer_state_t {
  public:
	DEBUG_STATE_NAME(whitespace_state_t);
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i = start;
		while(i < input.size() && std::isspace(input[i])) {
			i++;
		}
		if(i == start) {
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
		}
		return {token_t {input.substr(start, i - start), token_t::type_t::whitespace},
				i - start,
				DEBUG_INJECT_STATE_NAME()};	   // Skip whitespace silently
	}
};
}	 // namespace pfp
