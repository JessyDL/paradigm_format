#include "pfp/lexer.hpp"

#include <algorithm>
#include <numeric>

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
lexer_t::lexer_t() {
	states.push_back(std::make_pair(std::make_unique<whitespace_state_t>(), "whitespace"));
	states.push_back(std::make_pair(std::make_unique<float_number_state_t>(), "floating point"));
	states.push_back(std::make_pair(std::make_unique<keyword_state_t>(), "identifier"));
	states.push_back(std::make_pair(std::make_unique<operator_state_t>(), "operator"));
	states.push_back(std::make_pair(std::make_unique<preprocessor_state_t>(), "preprocessor"));
	states.push_back(std::make_pair(std::make_unique<template_state_t>(), "variadic"));
	states.push_back(std::make_pair(std::make_unique<number_state_t>(), "number"));
	states.push_back(std::make_pair(std::make_unique<comment_state_t>(), "comment"));
	states.push_back(std::make_pair(std::make_unique<literal_state_t>(), "literal"));
	errorBuffer.reserve(1024);
}

token_generator_t lexer_t::tokenize(std::string_view input) {
	std::size_t i = 0;

	while(i < input.size()) {
		token_candidate_t bestCandidate {};
		std::vector<std::pair<token_candidate_t, std::size_t>> competingCandidates {};

		std::size_t state_i = 0;
		for(const auto& [state, ignore] : states) {
			auto candidate = state->handle(input, i);
			if(candidate.isValid()) {
				competingCandidates.push_back(std::make_pair(candidate, state_i));
				if(bestCandidate.lengthConsumed == 0 || candidate.lengthConsumed > bestCandidate.lengthConsumed) {
					bestCandidate = candidate;
				}
			}
			state_i++;
		}

		if(competingCandidates.size() > 1) {
			// **Check for ambiguity**
			auto numEqualWinners =
			  std::count_if(competingCandidates.begin(), competingCandidates.end(), [&](const auto& pair) {
				  return pair.first.lengthConsumed == bestCandidate.lengthConsumed;
			  });

			if(numEqualWinners > 1) {
				errorBuffer = "Ambiguous tokenization: ";
				auto errorString =
				  std::string("lexers : [") + std::accumulate(competingCandidates.begin(),
															  competingCandidates.end(),
															  std::string(""),
															  [this](const std::string& acc, const auto& pair) {
																  return acc + (acc.empty() ? "" : ", ") +
																		 states[pair.second].second.data();
															  });
				errorString += "] at position " + std::to_string(i) + " in input value '" +
							   std::string(input.substr(i, competingCandidates[0].first.lengthConsumed)) + "'";
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
			co_yield bestCandidate.token.value_or(
			  token_t {"Candidate token was set but had no value", token_t::type_t::error});
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
