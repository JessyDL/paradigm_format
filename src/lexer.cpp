#include "pfp/lexer.hpp"
#include <string>

#include "pfp/lexers/comment.hpp"
#include "pfp/lexers/float_number.hpp"
#include "pfp/lexers/keyword.hpp"
#include "pfp/lexers/literal.hpp"
#include "pfp/lexers/number.hpp"
#include "pfp/lexers/operator.hpp"
#include "pfp/lexers/preprocessor.hpp"
#include "pfp/lexers/template.hpp"
#include "pfp/lexers/whitespace.hpp"

namespace pfp {
tokenizer_t::tokenizer_t() {
	states.push_back(std::make_unique<whitespace_state_t>());
	states.push_back(std::make_unique<float_number_state_t>());
	states.push_back(std::make_unique<keyword_state_t>());
	states.push_back(std::make_unique<operator_state_t>());
	states.push_back(std::make_unique<preprocessor_state_t>());
	states.push_back(std::make_unique<template_state_t>());
	states.push_back(std::make_unique<number_state_t>());
	states.push_back(std::make_unique<comment_state_t>());
	states.push_back(std::make_unique<literal_state_t>());
	errorBuffer.reserve(1024);
}

token_generator_t tokenizer_t::tokenize(std::string_view input) {
	std::size_t i = 0;

	while(i < input.size()) {
		token_candidate_t bestCandidate {};
		std::vector<token_candidate_t> competingCandidates {};

		for(const auto& state : states) {
			auto candidate = state->handle(input, i);
			if(candidate.isValid()) {
				competingCandidates.push_back(candidate);
				if(bestCandidate.lengthConsumed == 0 || candidate.lengthConsumed > bestCandidate.lengthConsumed) {
					bestCandidate = candidate;
				}
			}
		}

		if(competingCandidates.size() > 1) {
			// **Check for ambiguity**
			auto numEqualWinners =
			  std::count_if(competingCandidates.begin(), competingCandidates.end(), [&](const token_candidate_t& c) {
				  return c.lengthConsumed == bestCandidate.lengthConsumed;
			  });

			if(numEqualWinners > 1) {
				errorBuffer		 = "Ambiguous tokenization: ";
				auto errorString = std::accumulate(competingCandidates.begin(),
												   competingCandidates.end(),
												   std::string(""),
												   [](const std::string& acc, const token_candidate_t& c) {
													   return acc + (acc.empty() ? "" : ", ") + c._state_name.data();
												   });
				errorString += " at position " + std::to_string(i) + " in input value '" +
							   std::string(input.substr(i, competingCandidates[0].lengthConsumed)) + "'";
				if(errorBuffer.size() + errorString.size() < errorBuffer.capacity()) {
					errorBuffer += errorString;
				} else {
					errorString.resize(errorBuffer.capacity() - errorBuffer.size() - 3);
					errorString += "...";
					errorBuffer += errorString;
				}
				co_yield token_t {errorBuffer, token_t::type_t::error};
				i++;
				continue;
			}
		}

		if(bestCandidate.isValid()) {
			co_yield bestCandidate.token.value();
			i += bestCandidate.lengthConsumed;
		} else {
			errorBuffer = "Unexpected character at position " + std::to_string(i) + " in input value '" +
						  std::string(input.substr(i, 1)) + "'";

			co_yield token_t {errorBuffer, token_t::type_t::error};
			i++;
		}
	}
}

}	 // namespace pfp
