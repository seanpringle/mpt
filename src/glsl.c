
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
	return MAX(a, b);
}

int imin(int a, int b) {
	return MIN(a, b);
}

double dmax(double a, double b) {
	return MAX(a, b);
}

double dmin(double a, double b) {
	return MIN(a, b);
}

double dclamp(double val, double low, double high) {
	return MIN(high, MAX(low, val));
}

double dround(double val) {
	return round(val);
}

double sign(double val) {
	return val < 0.0 ? -1.0: (val > 0.0 ? 1.0: 0.0);
}
