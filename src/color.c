
#include <math.h>
#include "color.h"

Color colorScale(Color ca, double s) {
	return (Color){ .r = ca.r * s, .g = ca.g * s, .b = ca.b * s };
}

Color colorMul(Color ca, Color cb) {
	return (Color){ .r = ca.r * cb.r, .g = ca.g * cb.g, .b = ca.b * cb.b };
}

Color colorDiv(Color ca, Color cb) {
	return (Color){ .r = ca.r / cb.r, .g = ca.g / cb.g, .b = ca.b / cb.b };
}

Color colorSub(Color ca, Color cb) {
	return (Color){ .r = ca.r - cb.r, .g = ca.g - cb.g, .b = ca.b - cb.b };
}

Color colorAdd(Color ca, Color cb) {
	return (Color){ .r = ca.r + cb.r, .g = ca.g + cb.g, .b = ca.b + cb.b };
}

Color colorMin(Color ca, Color cb) {
	return (Color){ .r = ca.r < cb.r ? ca.r: cb.r, .g = ca.g < cb.g ? ca.g: cb.g, .b = ca.b < cb.b ? ca.b: cb.b };
}

Color colorMax(Color ca, Color cb) {
	return (Color){ .r = ca.r > cb.r ? ca.r: cb.r, .g = ca.g > cb.g ? ca.g: cb.g, .b = ca.b > cb.b ? ca.b: cb.b };
}

double colorBrightness(Color c) {
	return 0.299*c.r + 0.587*c.g + 0.114*c.b;
}

Color hex(int x) {
	double r = (double)((x>>16)&0xff) / 255;
	double g = (double)((x>>8)&0xff) / 255;
	double b = (double)((x>>0)&0xff) / 255;
	return (Color){r, g, b};
}

uint32_t NRGBAtoARGB32(NRGBA rgba) {
	return (rgba.a << 24) | (rgba.r << 16) | (rgba.g << 8) | (rgba.b);
}