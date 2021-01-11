
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include "vec3.h"

double vec3Len(vec3 va) {
	return sqrt(va.x*va.x + va.y*va.y + va.z*va.z);
}

vec3 vec3Scale(vec3 va, double s) {
	return (vec3){ .x = va.x * s, .y = va.y * s, .z = va.z * s };
}

vec3 vec3Abs(vec3 va) {
	return (vec3){ .x = fabs(va.x), .y = fabs(va.y), .z = fabs(va.z) };
}

vec3 vec3Neg(vec3 va) {
	return (vec3){ .x = -va.x, .y = -va.y, .z = -va.z };
}

vec3 vec3Round(vec3 va) {
	return (vec3){ .x = round(va.x), .y = round(va.y), .z = round(va.z) };
}

vec3 vec3Unit(vec3 va) {
	return vec3Scale(va, 1.0 / vec3Len(va));
}

double vec3Dot(vec3 va, vec3 vb) {
	return va.x*vb.x + va.y*vb.y + va.z*vb.z;
}

vec3 vec3Mul(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x * vb.x, .y = va.y * vb.y, .z = va.z * vb.z };
}

vec3 vec3Div(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x / vb.x, .y = va.y / vb.y, .z = va.z / vb.z };
}

vec3 vec3Sub(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x - vb.x, .y = va.y - vb.y, .z = va.z - vb.z };
}

vec3 vec3Add(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x + vb.x, .y = va.y + vb.y, .z = va.z + vb.z };
}

vec3 vec3Min(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x < vb.x ? va.x: vb.x, .y = va.y < vb.y ? va.y: vb.y, .z = va.z < vb.z ? va.z: vb.z };
}

vec3 vec3Max(vec3 va, vec3 vb) {
	return (vec3){ .x = va.x > vb.x ? va.x: vb.x, .y = va.y > vb.y ? va.y: vb.y, .z = va.z > vb.z ? va.z: vb.z };
}

vec3 vec3Cross(vec3 va, vec3 vb) {
	return (vec3){ .x = va.y*vb.z - va.z*vb.y, .y = -(va.x*vb.z - va.z*vb.x), .z = va.x*vb.y - va.y*vb.x };
}

vec3 vec3Mod(vec3 va, vec3 vb) {
	return (vec3) { .x = va.x - vb.x*floor(va.x/vb.x), .y = va.y - vb.y*floor(va.y/vb.y), .z = va.z - vb.z*floor(va.z/vb.z) };
}

vec3 vec3Reflect(vec3 v, vec3 n) {
	return vec3Sub(v, vec3Scale(n, 2.0 * vec3Dot(v, n)));
}

bool vec3Refract(vec3 v, vec3 n, double niOverNt, vec3 *refracted) {
	vec3 uv = vec3Unit(v);
	vec3 un = vec3Unit(n);
	double dt = vec3Dot(uv, un);

	double discriminant = 1.0 - niOverNt*niOverNt*(1-dt*dt);

	if (discriminant > 0) {
		*refracted = vec3Sub(vec3Scale(vec3Sub(uv, vec3Scale(un, dt)), niOverNt), vec3Scale(un, sqrt(discriminant)));
		return true;
	}

	*refracted = (vec3){0,0,0};
	return false;
}

static double dclamp(double val, double low, double high) {
	val = val < low ? low: val;
	val = val > high ? high: val;
	return val;
}

vec3 vec3Clamp(vec3 v, vec3 l, vec3 h) {
	return (vec3){.x = dclamp(v.x, l.x, h.x), .y = dclamp(v.y, l.y, h.y), .z = dclamp(v.z, l.z, h.z)};
}

bool vec3Less(vec3 va, vec3 vb) {
	if (va.x < vb.x) return true;
	if (va.x > vb.x) return false;
	if (va.y < vb.y) return true;
	if (va.y > vb.y) return false;
	if (va.z < vb.z) return true;
	if (va.z > vb.z) return false;
	return false;
}
