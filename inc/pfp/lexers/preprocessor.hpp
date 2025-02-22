#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>

namespace pfp {
class preprocessor_state_t : public tokenizer_state_t {
  public:
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		if(input[start] != '#')
			return {};

		std::size_t i = start;
		while(i < input.size() && input[i] != '\n') i++;

		return {token_t {input.substr(start, i - start), token_t::type_t::preprocessor}, i - start};
	}
};
}	 // namespace pfp
