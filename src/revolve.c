
#include "common.h"
#include "glsl.h"

struct state {
	double o;
	SDF2 sdf;
};

static double revolveEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	double x = len(v2(pos.x, pos.y)) - s->o;
	return SDF2Evaluate(s->sdf, v2(x, pos.z));
}

static sphere_t revolveBounds(void *p) {
	struct state *s = p;
	circle_t bounds = s->sdf.bounds;
	return (sphere_t){
		(vec3){bounds.center.x, bounds.center.y, 0},
		bounds.radius + s->o,
	};
}

SDF3 revolve(double o, SDF2 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->o = o;
	s->sdf = sdf;
	return (SDF3){revolveEvaluate, revolveBounds(s), s};
}