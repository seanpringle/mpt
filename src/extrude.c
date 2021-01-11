
#include "common.h"
#include "glsl.h"

struct state {
	double h;
	SDF2 sdf;
};

static double extrudeEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	double x = SDF2Evaluate(s->sdf, v2(pos.x, pos.y));
	double y = abs(pos.z) - s->h;
	return min(max(x, y), 0.0) + len(max(v2(x,y), Zero2));
}

static sphere_t extrudeBounds(void *p) {
	struct state *s = p;
	circle_t bounds = s->sdf.bounds;
	return (sphere_t){
		v3(bounds.center.x, bounds.center.y, 0),
		len(v2(bounds.radius, s->h)),
//		sqrt(bounds.radius * bounds.radius + s->h * s->h),
	};
}

SDF3 extrude(double h, SDF2 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->h = h/2;
	s->sdf = sdf;
	return (SDF3){extrudeEvaluate, extrudeBounds(s), s};
}
