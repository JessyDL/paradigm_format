#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>

namespace pfp {
class float_number_state_t : public tokenizer_state_t {
  public:
	DEBUG_STATE_NAME(float_number_state_t)

	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i	= start;
		bool hasDecimal = false, hasExponent = false;

		while(i < input.size()) {
			char next = input[i];

			if(std::isdigit(next)) {
				i++;
			} else if(next == '.' && !hasDecimal) {
				hasDecimal = true;
				i++;
			} else if((next == 'e' || next == 'E') && !hasExponent) {
				hasExponent = true;
				i++;
				if(i < input.size() && (input[i] == '+' || input[i] == '-')) {
					i++;	// Handle exponent sign (e.g., `2e-10`)
				}
			} else {
				break;
			}
		}

		// last needs to be a 'f'
		if(i < input.size() && input[i] == 'f') {
			i++;
		} else {
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
		}

		if(i > start)
			return {
			  token_t {input.substr(start, i - start), token_t::type_t::number}, i - start, DEBUG_INJECT_STATE_NAME()};
		return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
	}
};
}	 // namespace pfp
