#include "pow.h"

extern "C" __declspec(dllexport) std::string type() {
	std::string s = "^";
	return s;
}

extern "C" __declspec(dllexport) bool binary() {
	bool b = 1;
	return b;
}

extern "C" __declspec(dllexport) int priority() {
	int p = 3;
	return p;
}

extern "C" __declspec(dllexport) double operation(double x, double y) {
	if (x==0 && y<0) throw  std::exception("Error: Division by 0\n");
	if (x<0 && y<1 && y>0) throw  std::exception("Error: Root of a negative number\n");
	double r = pow(x,y);
	return r;
}
