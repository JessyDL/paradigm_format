#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "pfp/token.hpp"


namespace pfp {

class tokenizer_t final {
  private:
	std::vector<std::pair<std::unique_ptr<tokenizer_state_t>, std::string_view>> states;
	std::string errorBuffer {};

  public:
	tokenizer_t();
	token_generator_t tokenize(std::string_view input);
};
}	 // namespace pfp
