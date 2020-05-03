
#include "common.h"
#include "glsl.h"

vec2 Zero2;

struct circle {
	double radius;
};

static double circleEvaluate(void *p, vec2 pos) {
	struct circle *s = p;
	return len(pos) - s->radius;
}

static circle_t circleBounds(void *p) {
	struct circle *s = p;
	return (circle_t){Zero2, s->radius};
}

SDF2 Circle(double diameter) {
	struct circle *s = allot(sizeof(struct circle));
	s->radius = diameter/2;
	return (SDF2){circleEvaluate, circleBounds(s), s};
}

struct rectangle {
	double x, y;
};

static double rectangleEvaluate(void *p, vec2 pos) {
	struct rectangle *s = p;
	vec2 d = sub(abs(pos), v2(s->x,s->y));
	return len(max(d, Zero2)) + min(max(d.x, d.y), 0.0);
}

static circle_t rectangleBounds(void *p) {
	struct rectangle *s = p;
	return (circle_t){Zero2, sqrt(s->x*s->x + s->y*s->y)};
}

SDF2 Rectangle(double x, double y) {
	struct rectangle *s = allot(sizeof(struct rectangle));
	s->x = x/2;
	s->y = y/2;
	return (SDF2){rectangleEvaluate, rectangleBounds(s), s};
}
