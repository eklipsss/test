#include "break_sin.h"

extern "C" __declspec(dllexport) bool binary() {
	bool b = 0;
	return b;
}

extern "C" __declspec(dllexport) int priority() {
	int p = 3;
	return p;
}

extern "C" __declspec(dllexport) double operation(double x) {
	double r = sin(x);
	return r;
}
