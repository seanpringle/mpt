
#include "common.h"
#include "glsl.h"

struct rounded {
	double r;
	SDF3 sdf;
};

static double roundedEvaluate(void *p, vec3 pos) {
	struct rounded *s = p;
	return s->sdf.evaluate(s->sdf.context, pos) - s->r;
}

static Bounds3 roundedBounds(void *p) {
	struct rounded *s = p;
	Bounds3 bounds = s->sdf.bounds;
	bounds.radius += s->r;
	return bounds;
}

SDF3 Round(double r, SDF3 sdf) {
	struct rounded *s = allot(sizeof(struct rounded));
	s->r = r;
	s->sdf = sdf;
	return (SDF3){roundedEvaluate, roundedBounds(s), s};
}