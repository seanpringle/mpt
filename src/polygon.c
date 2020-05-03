
#include "common.h"
#include "glsl.h"

struct polygon {
	int n;
	double r;
};

static double polygonEvaluate(void *p, vec2 pos) {
	struct polygon *s = p;
	double n = (double)s->n / 2.0;
	double o = M_PI / 2.0 / n;
	double a = atan(pos.y / pos.x);
	a = pos.x < 0 ? a+M_PI: a;
	double t = round(a/M_PI*n) / n * M_PI;
	double d = round((a+o)/M_PI*n)/n*M_PI - o;
	vec2 f = v2(cos(t), sin(t));

	if (abs(dot(v2(pos.x, -pos.y), v2(f.y, f.x))) < sin(o)*s->r) {
		return dot(pos, f) - cos(o)*s->r;
	}

	return len(sub(pos, scale(v2(cos(d), sin(d)), s->r)));
}

static Bounds2 polygonBounds(void *p) {
	struct polygon *s = p;
	return (Bounds2){Zero2, s->r*2}; // *2?
}

SDF2 Polygon(int n, double r) {
	struct polygon *s = allot(sizeof(struct polygon));
	s->n = n;
	s->r = r;
	return (SDF2){polygonEvaluate, polygonBounds(s), s};
}
