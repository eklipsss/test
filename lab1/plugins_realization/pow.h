#pragma once
#include <math.h>
#include <string>
#include <iostream>

extern "C" __declspec(dllexport) std::string type();
extern "C" __declspec(dllexport) bool binary();
extern "C" __declspec(dllexport) int priority();
extern "C" __declspec(dllexport) double operation(double x, double y);