#include "pfp/parser.hpp"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>

// make a fixture
class DataLoader {
	std::string input;
	pfp::tokenizer_t tokenizer;

  public:
	pfp::parser_t parser;

	explicit DataLoader(std::filesystem::path path)
		: input(load_data(path)), tokenizer(), parser(tokenizer.tokenize(input)) {}

  private:
	std::string load_data(std::filesystem::path path) {
		std::ifstream file(path);
		if(file.fail()) {
			throw std::runtime_error("Could not open file");
		}
		std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		return data;
	}
};


TEST_CASE("simple_types", "[parser]") {
	return;
	DataLoader loader("./data/types/simple_types.ptf");

	auto res = loader.parser.parse();
}
