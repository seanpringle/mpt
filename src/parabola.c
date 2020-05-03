
#include "common.h"
#include "glsl.h"

struct parabola {
	double m, h;
};

static double parabolaEvaluate(void *ptr, vec2 pos) {
	struct parabola *s = ptr;
	pos.x = abs(pos.x);
	double m = s->m;

	// capped at height
	if (pos.y > s->h) {
		double l = sqrt(s->h / m);
		vec2 a = {-l, s->h};
		vec2 b = {l, s->h};
		vec2 pa = sub(pos, a);
		vec2 ba = sub(b, a);
		double h = clamp(dot(pa, ba)/dot(ba, ba), 0.0, 1.0);
		return len(sub(pa, scale(ba, h)));
	}

	double p = (2.0*m*pos.y - 1.0) / (6.0 * m * m);
	double q = abs(pos.x) / (4.0 * m * m);
	double h = q*q - p*p*p;
	double r = sqrt(abs(h));
	double x;
	if (h > 0) {
		x = pow(q+r, 1.0/3.0) - pow(abs(q-r), 1.0/3.0)*sign(r-q);
	} else {
		x = 2.0 * cos(atan2(r, q)/3.0) * sqrt(p);
	}
	double y = m * x * x;
	return len(sub(pos, v2(x, y))) * sign(pos.x-x);
}

static Bounds2 parabolaBounds(void *p) {
	struct parabola *s = p;
	double x = sqrt(s->h / s->m);
	double r = sqrt(x*x + s->h*s->h);
	return (Bounds2){Zero2, r};
}

SDF2 Parabola(double w, double h) {
	w = w/2;
	struct parabola *s = allot(sizeof(struct parabola));
	s->m = h/(w*w);
	s->h = h;
	return (SDF2){parabolaEvaluate, parabolaBounds(s), s};
}
