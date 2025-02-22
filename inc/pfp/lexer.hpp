#pragma once
#include <algorithm>
#include <coroutine>
#include <cstdint>
#include <memory>
#include <numeric>
#include <string_view>
#include <vector>

#include "pfp/token.hpp"


namespace pfp {

class tokenizer_t final {
  private:
	std::vector<std::unique_ptr<tokenizer_state_t>> states;
	std::string errorBuffer {};

  public:
	tokenizer_t();
	token_generator_t tokenize(std::string_view input);
};
}	 // namespace pfp
