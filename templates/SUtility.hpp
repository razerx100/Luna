#ifndef __S_UTILITY_HPP__
#define __S_UTILITY_HPP__

struct __declspec(dllimport) SRect {
	long left;
	long top;
	long right;
	long bottom;
};

struct __declspec(dllimport) Color {
	float r;
	float g;
	float b;
	float a;
};
#endif
