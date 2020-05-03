
#include "common.h"
#include "glsl.h"

struct state {
	vec3 h;
	SDF3 sdf;
};

static double elongateEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	return s->sdf.evaluate(s->sdf.context, sub(pos, clamp(pos, neg(s->h), s->h)));
}

static sphere_t elongateBounds(void *p) {
	struct state *s = p;
	sphere_t bounds = s->sdf.bounds;
	bounds.radius += vec3Len(s->h);
	return bounds;
}

SDF3 elongate(double x, double y, double z, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->h = (vec3){x/2,y/2,z/2};
	s->sdf = sdf;
	return (SDF3){elongateEvaluate, elongateBounds(s), s};
}
