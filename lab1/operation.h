#pragma once
#include <stack>
#include <functional>
#include <string>

using realisation = std::function<void(std::stack<double>&)>;

class Operation {
private:
	std::string type;
	int priority; 
	bool binary;
public:
	Operation(std::string type, int prior, bool binary, realisation func) : type(type), priority(prior), binary(binary), func(func) {}
	Operation(Operation const& other) : type(other.type), priority(other.priority), binary(other.binary), func(other.func) {}
	~Operation() = default;

	std::string get_type() { return type; }
	int get_priority() { return priority; }
	realisation func;
};
