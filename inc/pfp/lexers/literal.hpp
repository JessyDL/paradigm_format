#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>

namespace pfp {
class literal_state_t : public tokenizer_state_t {
  private:
  public:
	DEBUG_STATE_NAME(literal_state_t);
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i				 = start;
		constexpr char8_t literals[] = {'"', '`', '\''};

		if(std::find(std::begin(literals), std::end(literals), input[i]) == std::end(literals)) {
			return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
		}

		char8_t literal = input[i];

		// check if the literal appears twice more so we know it's a long-form literal
		if(i + 2 < input.size() && input[i + 1] == literal && input[i + 2] == literal) {
			i += 3;
			while(i < input.size() && !(input[i] == literal && input[i - 1] == literal && input[i - 2] == literal)) {
				i++;
			}
			if(i < input.size()) {
				i += 3;
				return {token_t {input.substr(start, i - start), token_t::type_t::string},
						i - start,
						DEBUG_INJECT_STATE_NAME()};
			}
		} else {
			i++;
			while(i < input.size() && input[i] != literal) {
				i++;
			}
			if(i < input.size()) {
				i++;
				return {token_t {input.substr(start, i - start), token_t::type_t::string},
						i - start,
						DEBUG_INJECT_STATE_NAME()};
			}
		}

		return {std::nullopt, 0, DEBUG_INJECT_STATE_NAME()};
	}
};
}	 // namespace pfp
