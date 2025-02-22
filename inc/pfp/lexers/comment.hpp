#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>

namespace pfp {
class comment_state_t : public tokenizer_state_t {
  private:
  public:
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i = start;

		if(i + 1 < input.size()) {
			std::string_view next = input.substr(i, 2);
			if(next == "//") {
				i += 2;
				while(i < input.size() && input[i] != '\n') {
					i++;
				}
				return {token_t {input.substr(start, i - start), token_t::type_t::comment}, i - start};
			} else if(next == "/*") {
				i += 2;
				while(i + 1 < input.size() && input[i] != '*' && input[i + 1] != '/') {
					i++;
				}
				if(i + 1 < input.size()) {
					i += 2;
					return {token_t {input.substr(start, i - start), token_t::type_t::comment}, i - start};
				}
			}
		}

		return {};
	}
};
}	 // namespace pfp
