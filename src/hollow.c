
#include "common.h"
#include "glsl.h"

struct state {
	double thickness;
	SDF3 sdf;
};

static double hollowEvaluate(void *ptr, vec3 p) {
	struct state *s = ptr;
	return abs(s->sdf.evaluate(s->sdf.context, p)) - s->thickness;
}

static sphere_t hollowBounds(void *p) {
	struct state *s = p;
	return s->sdf.bounds;
}

SDF3 Hollow(double thickness, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->thickness = thickness;
	s->sdf = sdf;
	return (SDF3){hollowEvaluate, hollowBounds(s), s};
}
