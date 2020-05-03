
#include "common.h"
#include "glsl.h"

struct state {
	double r;
	SDF3 sdf;
};

static double roundedEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	return SDF3Evaluate(s->sdf, pos) - s->r;
}

static sphere_t roundedBounds(void *p) {
	struct state *s = p;
	sphere_t bounds = s->sdf.bounds;
	bounds.radius += s->r;
	return bounds;
}

SDF3 rounded(double r, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->r = r;
	s->sdf = sdf;
	return (SDF3){roundedEvaluate, roundedBounds(s), s};
}