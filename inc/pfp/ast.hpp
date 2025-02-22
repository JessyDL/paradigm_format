#pragma once

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace pfp {
// **Base AST Node**
class ast_node_t {
  public:
	virtual void print() const = 0;
	virtual ~ast_node_t()	   = default;
};

// **preprocessor Directive Node**
class ast_preprocessor_node_t : public ast_node_t {
  public:
	std::string directive;

	explicit ast_preprocessor_node_t(std::string directive) : directive(std::move(directive)) {}

	void print() const override { std::cout << "preprocessor: " << directive << "\n"; }
};

// **Variable Declaration Node**
class ast_variable_node_t : public ast_node_t {
  public:
	std::string type;
	std::string name;
	std::unique_ptr<ast_node_t> value;

	ast_variable_node_t(std::string type, std::string name, std::unique_ptr<ast_node_t> value)
		: type(std::move(type)), name(std::move(name)), value(std::move(value)) {}

	void print() const override {
		std::cout << "Variable: " << type << " " << name << " = ";
		value->print();
	}
};

// **identifier Node (Variable, Function Name, etc.)**
class ast_identifier_node_t : public ast_node_t {
  public:
	std::string name;

	explicit ast_identifier_node_t(std::string name) : name(std::move(name)) {}

	void print() const override { std::cout << "identifier: " << name << "\n"; }
};

// **number Literal (3, 3.14, 2e10, etc.)**
class ast_number_literal_node_t : public ast_node_t {
  public:
	std::string value;

	explicit ast_number_literal_node_t(std::string value) : value(std::move(value)) {}

	void print() const override { std::cout << "NumberLiteral: " << value << "\n"; }
};


// **Binary Expression Node**
class ast_binary_expression_node_t : public ast_node_t {
  public:
	std::unique_ptr<ast_node_t> left;
	std::string op;
	std::unique_ptr<ast_node_t> right;

	ast_binary_expression_node_t(std::unique_ptr<ast_node_t> left, std::string op, std::unique_ptr<ast_node_t> right)
		: left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

	void print() const override {
		std::cout << "(";
		left->print();
		std::cout << " " << op << " ";
		right->print();
		std::cout << ")";
	}
};

class ast_template_node_t : public ast_node_t {
  public:
	std::vector<std::string> templateParams;

	ast_template_node_t(std::vector<std::string> templateParams) : templateParams(std::move(templateParams)) {}

	void print() const override {
		std::cout << "variadic <";
		for(const auto& param : templateParams) {
			std::cout << param << " ";
		}
		std::cout << ">\n";
	}
};

// **Function Definition Node**
class ast_function_node_t : public ast_node_t {
  public:
	std::string name;
	std::vector<std::pair<std::string, std::string>> params;
	std::vector<std::unique_ptr<ast_node_t>> body;
	std::optional<std::unique_ptr<ast_template_node_t>> templateNode;

	ast_function_node_t(std::string name,
						std::optional<std::unique_ptr<ast_template_node_t>> templateNode = std::nullopt)
		: name(std::move(name)), templateNode(std::move(templateNode)) {}

	void print() const override {
		if(templateNode) {
			(*templateNode)->print();
		}
		std::cout << "Function: " << name << " (";
		for(const auto& [type, param] : params) {
			std::cout << type << " " << param << ", ";
		}
		std::cout << ")\n{\n";
		for(const auto& stmt : body) {
			stmt->print();
		}
		std::cout << "}\n";
	}
};

// **Class Definition Node**
class ast_class_node_t : public ast_node_t {
  public:
	std::string name;
	std::vector<std::unique_ptr<ast_node_t>> members;
	std::optional<std::unique_ptr<ast_template_node_t>> templateNode;

	explicit ast_class_node_t(std::string name,
							  std::optional<std::unique_ptr<ast_template_node_t>> templateNode = std::nullopt)
		: name(std::move(name)), templateNode(std::move(templateNode)) {}

	void print() const override {
		if(templateNode) {
			(*templateNode)->print();
		}
		std::cout << "Class: " << name << "\n{\n";
		for(const auto& member : members) {
			member->print();
		}
		std::cout << "}\n";
	}
};

class ast_tree_t {
  public:
	std::vector<std::unique_ptr<ast_node_t>> nodes;
	void addNode(std::unique_ptr<ast_node_t> node) { nodes.push_back(std::move(node)); }
	void print() const {
		for(const auto& node : nodes) {
			node->print();
		}
	}
};
}	 // namespace pfp
