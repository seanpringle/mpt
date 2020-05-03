
#include "common.h"
#include "glsl.h"

struct state {
	double h;
	double r1;
	double r2;
};

static double stadiumEvaluate(void *p, vec2 pos) {
	struct state *s = p;
	pos.x = abs(pos.x);
	double b = (s->r1 - s->r2) / s->h;
	double a = sqrt(1.0 - b*b);
	double k = dot(pos, v2(-b, a));
	if (k < 0.0) {
		return len(pos) - s->r1;
	}
	if (k > a*s->h) {
		return len(sub(pos, v2(0.0, s->h))) - s->r2;
	}
	return dot(pos, v2(a, b)) - s->r1;
}

static circle_t stadiumBounds(void *p) {
	struct state *s = p;
	return (circle_t){Zero2, s->h + s->r1 + s->r2};
}

SDF2 stadium(double h, double d1, double d2) {
	struct state *s = allot(sizeof(struct state));
	s->h = h;
	s->r1 = d1/2;
	s->r2 = d2/2;
	return (SDF2){stadiumEvaluate, stadiumBounds(s), s};
}
