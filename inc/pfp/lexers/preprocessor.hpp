#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>

namespace pfp {
class preprocessor_state_t : public tokenizer_state_t {
  public:
	DEBUG_STATE_NAME(preprocessor_state_t)
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		if(input[start] != '#')
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};

		std::size_t i = start;
		while(i < input.size() && input[i] != '\n') i++;

		return {token_t {input.substr(start, i - start), token_t::type_t::preprocessor},
				i - start,
				DEBUG_INJECT_STATE_NAME()};
	}
};
}	 // namespace pfp
