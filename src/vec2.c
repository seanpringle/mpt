
#include <math.h>
#include <stdint.h>
#include "vec2.h"

double vec2Len(vec2 va) {
	return sqrt(va.x*va.x + va.y*va.y);
}

vec2 vec2Scale(vec2 va, double s) {
	return (vec2){ .x = va.x * s, .y = va.y * s };
}

vec2 vec2Abs(vec2 va) {
	return (vec2){ .x = fabs(va.x), .y = fabs(va.y) };
}

vec2 vec2Neg(vec2 va) {
	return (vec2){ .x = -va.x, .y = -va.y };
}

vec2 vec2Unit(vec2 va) {
	return vec2Scale(va, 1.0 / vec2Len(va));
}

double vec2Dot(vec2 va, vec2 vb) {
	return va.x*vb.x + va.y*vb.y;
}

vec2 vec2Mul(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x * vb.x, .y = va.y * vb.y };
}

vec2 vec2Div(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x / vb.x, .y = va.y / vb.y };
}

vec2 vec2Sub(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x - vb.x, .y = va.y - vb.y };
}

vec2 vec2Add(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x + vb.x, .y = va.y + vb.y };
}

vec2 vec2Min(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x < vb.x ? va.x: vb.x, .y = va.y < vb.y ? va.y: vb.y };
}

vec2 vec2Max(vec2 va, vec2 vb) {
	return (vec2){ .x = va.x > vb.x ? va.x: vb.x, .y = va.y > vb.y ? va.y: vb.y };
}

vec2 vec2Mod(vec2 va, vec2 vb) {
	return (vec2) { .x = va.x - vb.x*floor(va.x/vb.x), .y = va.y - vb.y*floor(va.y/vb.y) };
}
