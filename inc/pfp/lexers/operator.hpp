#pragma once
#include "pfp/token.hpp"
#include <cstdint>
#include <string_view>
#include <unordered_set>

namespace pfp {
class operator_state_t : public tokenizer_state_t {
  private:
	static inline const std::unordered_set<std::string_view>
	  multiCharOps {"+=", "-=", "*=", "/=", "==", "!=", "&&", "||", "<<", ">>", "->", "=>"};

	static inline const std::unordered_set<std::string_view>
	  oneCharOps {"+", "-", "*", "/", "=", "!", "&", "|", "<", ">", "(", ")", "{", "}", "[", "]", ";", ":", "#"};

  public:
	token_candidate_t handle(std::string_view input, std::size_t start) override {
		std::size_t i = start;

		if(i + 1 < input.size()) {
			std::string_view twoCharOp = input.substr(i, 2);
			if(multiCharOps.find(twoCharOp) != multiCharOps.end()) {
				return {token_t {twoCharOp, token_t::type_t::symbol}, 2};
			}
		}

		if(i < input.size()) {
			std::string_view oneCharOp = input.substr(i, 1);
			if(oneCharOps.find(oneCharOp) != oneCharOps.end()) {
				return {token_t {oneCharOp, token_t::type_t::symbol}, 1};
			}
		}

		return {};
	}
};
}	 // namespace pfp
