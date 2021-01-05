
#include "common.h"

int iabs(int i) {
	return i < 0 ? -i: i;
}

int ineg(int i) {
	return -i;
}

double dneg(double d) {
	return -d;
}

int imax(int a, int b) {
	return a > b ? a: b;
}

int imin(int a, int b) {
	return a < b ? a: b;
}

double dmax(double a, double b) {
	return a > b ? a: b;
}

double dmin(double a, double b) {
	return a < b ? a: b;
}

double dclamp(double val, double low, double high) {
	return dmin(high, dmax(low, val));
}

double dround(double val) {
	return round(val);
}

double sign(double val) {
	return val < 0.0 ? -1.0: (val > 0.0 ? 1.0: 0.0);
}

double dmix(double x, double y, double a) {
	return x + (y - x) * a;
}