
#include "common.h"
#include "glsl.h"

struct elongate {
	vec3 h;
	SDF3 sdf;
};

static double elongateEvaluate(void *p, vec3 pos) {
	struct elongate *s = p;
	return s->sdf.evaluate(s->sdf.context, sub(pos, clamp(pos, neg(s->h), s->h)));
}

static Bounds3 elongateBounds(void *p) {
	struct elongate *s = p;
	Bounds3 bounds = s->sdf.bounds;
	bounds.radius += vec3Len(s->h);
	return bounds;
}

SDF3 Elongate(double x, double y, double z, SDF3 sdf) {
	struct elongate *s = allot(sizeof(struct elongate));
	s->h = (vec3){x/2,y/2,z/2};
	s->sdf = sdf;
	return (SDF3){elongateEvaluate, elongateBounds(s), s};
}
