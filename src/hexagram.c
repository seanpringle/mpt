
#include "common.h"
#include "glsl.h"

struct state {
	double r;
};

static double hexagramEvaluate(void *ptr, vec2 p) {
	struct state *s = ptr;
	const double kx = -0.5;
	const double ky = 0.8660254038;
	const double kz = 0.5773502692;
	const double kw = 1.7320508076;
	p = abs(p);
	p = sub(p, scale(scale(v2(kx, ky), min(dot(v2(kx, ky), p), 0.0)), 2.0));
	p = sub(p, scale(scale(v2(ky, kx), min(dot(v2(ky, kx), p), 0.0)), 2.0));
	p = sub(p, v2(clamp(p.x, s->r*kz, s->r*kw), s->r));
	return len(p) * sign(p.y);
}

static circle_t hexagramBounds(void *p) {
	struct state *s = p;
	return (circle_t){Zero2, s->r*2}; //*2?
}

SDF2 hexagram(double diameter) {
	struct state *s = allot(sizeof(struct state));
	s->r = diameter/2;
	return (SDF2){hexagramEvaluate, hexagramBounds(s), s};
}

