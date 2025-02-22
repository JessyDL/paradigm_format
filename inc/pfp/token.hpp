#pragma once

#include <coroutine>
#include <cstdint>
#include <exception>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace pfp {
// Define token_t Structure
struct token_t {
	std::string_view value;
	enum class type_t {
		identifier,
		number,
		symbol,
		whitespace,
		string,
		comment,
		preprocessor,
		variadic,
		_end,
		error
	} type;

	std::string_view type_string() const { return TypeString(type); }
	static constexpr std::string_view TypeString(type_t type) {
		switch(type) {
		case type_t::identifier:
			return "identifier";
		case type_t::number:
			return "number";
		case type_t::symbol:
			return "symbol";
		case type_t::whitespace:
			return "whitespace";
		case type_t::string:
			return "string";
		case type_t::comment:
			return "comment";
		case type_t::preprocessor:
			return "preprocessor";
		case type_t::variadic:
			return "variadic";
		case type_t::_end:	  // special value for end of token stream
			return "_end";
		case type_t::error:	   // special value for error cases, this isn't part of the regular token stream and should
							   // not be checked for typically
			return "error";
		}
		throw std::runtime_error("Invalid token type");
	};

	bool operator==(const token_t& other) const { return value == other.value && type == other.type; }
	bool operator!=(const token_t& other) const { return !(*this == other); }
};

//
struct token_candidate_t {
	std::optional<token_t> token;
	std::size_t lengthConsumed;	   // How many characters were used?

	bool isValid() const { return token.has_value(); }
};


// Base tokenizer_state_t
class tokenizer_state_t {
  public:
	virtual ~tokenizer_state_t()												= default;
	virtual token_candidate_t handle(std::string_view input, std::size_t start) = 0;
};

// **token_t Generator (Coroutine-Based)**
struct token_generator_t {
	struct promise_type {
		token_t current_value;
		bool has_yielded = false;

		token_generator_t get_return_object() {
			return token_generator_t {std::coroutine_handle<promise_type>::from_promise(*this)};
		}

		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }

		std::suspend_always yield_value(token_t value) {
			current_value = std::move(value);
			has_yielded	  = true;
			return {};
		}

		void return_void() { has_yielded = false; }
		void unhandled_exception() { std::terminate(); }
	};

	std::coroutine_handle<promise_type> handle;

	explicit token_generator_t(std::coroutine_handle<promise_type> h) : handle(h) {}

	token_generator_t(const token_generator_t&)			   = delete;	// Prevent copying
	token_generator_t& operator=(const token_generator_t&) = delete;

	token_generator_t(token_generator_t&& other) noexcept : handle(other.handle) { other.handle = nullptr; }

	token_generator_t& operator=(token_generator_t&& other) noexcept {
		if(this != &other) {
			if(handle) {
				handle.destroy();
			}
			handle		 = other.handle;
			other.handle = nullptr;
		}
		return *this;
	}

	~token_generator_t() {
		if(handle) {
			handle.destroy();
		}
	}

	std::optional<token_t> next() {
		if(!handle || handle.done()) {
			return std::nullopt;
		}
		handle.resume();

		if(!handle.promise().has_yielded) {
			return std::nullopt;
		}

		return handle.promise().current_value;
	}
};
}	 // namespace pfp
