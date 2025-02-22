#include <iostream>
#include <string>

#include "pfp/lexer.hpp"
#include "pfp/parser.hpp"
#include "pfp/ast.hpp"


// **Usage Example**
int main() {
	std::string input = R"(type Foo := {
var1 : int = { 100 };
var2 : bool;
var3:string;
};
    )";


	pfp::tokenizer_t tokenizer;

	/* while(auto token = gen.next()) {
		std::cout << token->type_string() << ": " << token->value << "\n";
	}*/

    auto parser	   = pfp::parser_t(tokenizer.tokenize(input));
	auto ast_nodes	= parser.parse();

    if(!ast_nodes.empty()) {
		for(const auto& node : ast_nodes) {
			node->print();
		}
	} else {
		std::cerr << "Failed to parse\n";
		return -1;
	}
	return 0;
}
