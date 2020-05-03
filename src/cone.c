
#include "common.h"
#include "glsl.h"

struct state {
	double x, y, h, r;
};

static double coneEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	vec2 q = v2(len(v2(pos.x, pos.y)), pos.z);
	double d1 = -pos.z - s->h;
	double d2 = max(dot(q, v2(s->x, s->y)), pos.z);
	return len(max(v2(d1, d2), Zero2)) + min(max(d1, d2), 0.0);
}

static sphere_t coneBounds(void *p) {
	struct state *s = p;
	return (sphere_t){Zero3, sqrt(s->h*s->h + s->r*s->r)};
}

SDF3 cone(double h, double d) {
	double r = d/2;
	double rad = atan(h/r);
	struct state *s = allot(sizeof(struct state));
	s->x = sin(rad);
	s->y = cos(rad);
	s->h = h;
	s->r = r;
	SDF3 sdf = (SDF3){coneEvaluate, coneBounds(s), s};
	return translateZ(h/2, sdf);
}
