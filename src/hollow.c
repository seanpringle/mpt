
#include "common.h"
#include "glsl.h"

struct hollow {
	double thickness;
	SDF3 sdf;
};

static double hollowEvaluate(void *ptr, vec3 p) {
	struct hollow *s = ptr;
	return abs(s->sdf.evaluate(s->sdf.context, p)) - s->thickness;
}

static Bounds3 hollowBounds(void *p) {
	struct hollow *s = p;
	return s->sdf.bounds;
}

SDF3 Hollow(double thickness, SDF3 sdf) {
	struct hollow *s = allot(sizeof(struct hollow));
	s->thickness = thickness;
	s->sdf = sdf;
	return (SDF3){hollowEvaluate, hollowBounds(s), s};
}
