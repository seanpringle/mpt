
#include "common.h"
#include "glsl.h"

struct cone {
	double x, y, h, r;
};

static double coneEvaluate(void *p, vec3 pos) {
	struct cone *s = p;
	vec2 q = v2(len(v2(pos.x, pos.y)), pos.z);
	double d1 = -pos.z - s->h;
	double d2 = max(dot(q, v2(s->x, s->y)), pos.z);
	return len(max(v2(d1, d2), Zero2)) + min(max(d1, d2), 0.0);
}

static Bounds3 coneBounds(void *p) {
	struct cone *s = p;
	return (Bounds3){Zero3, sqrt(s->h*s->h + s->r*s->r)};
}

SDF3 Cone(double h, double d) {
	double r = d/2;
	double rad = atan(h/r);
	struct cone *s = allot(sizeof(struct cone));
	s->x = sin(rad);
	s->y = cos(rad);
	s->h = h;
	s->r = r;
	SDF3 sdf = (SDF3){coneEvaluate, coneBounds(s), s};
	return TranslateZ(h/2, sdf);
}
