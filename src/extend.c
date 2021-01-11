
#include "common.h"
#include "glsl.h"

struct state {
	vec3 h;
	SDF3 sdf;
};

static double extendEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	return SDF3Evaluate(s->sdf, sub(pos, clamp(pos, neg(s->h), s->h)));
}

static sphere_t extendBounds(void *p) {
	struct state *s = p;
	sphere_t bounds = s->sdf.bounds;
	bounds.radius += len(s->h);
	return bounds;
}

SDF3 extend(double x, double y, double z, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->h = v3(x/2, y/2, z/2);
	s->sdf = sdf;
	return (SDF3){extendEvaluate, extendBounds(s), s};
}
