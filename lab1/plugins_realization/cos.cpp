#include "cos.h"

extern "C" __declspec(dllexport) std::string type() {
	std::string s = "cos";
	return s;
}

extern "C" __declspec(dllexport) bool binary() {
	bool b = 0;
	return b;
}

extern "C" __declspec(dllexport) int priority() {
	int p = 3;
	return p;
}

extern "C" __declspec(dllexport) double operation(double x) {
	double r = cos(x);
	return r;
}
