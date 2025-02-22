#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <optional>
#include <string_view>

namespace pfp {
class template_state_t : public tokenizer_state_t {
  public:
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		if(input[start] != '<') {
			return {};
		}

		std::size_t i = start + 1;
		int depth	  = 1;

		while(i < input.size() && depth > 0) {
			if(input[i] == '<') {
				depth++;
			} else if(input[i] == '>') {
				depth--;
			}
			i++;
		}

		if(depth == 0) {
			return {token_t {input.substr(start, i - start), token_t::type_t::variadic}, i - start};
		} else {
			return {token_t {"ERROR: Unmatched template brackets", token_t::type_t::error}, i - start};
		}
	}
};
}	 // namespace pfp
