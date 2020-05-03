
#include "common.h"
#include "glsl.h"

struct hexagram {
	double r;
};

static double hexagramEvaluate(void *ptr, vec2 p) {
	struct hexagram *s = ptr;
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

static Bounds2 hexagramBounds(void *p) {
	struct hexagram *s = p;
	return (Bounds2){Zero2, s->r*2}; //*2?
}

SDF2 Hexagram(double diameter) {
	struct hexagram *s = allot(sizeof(struct hexagram));
	s->r = diameter/2;
	return (SDF2){hexagramEvaluate, hexagramBounds(s), s};
}

