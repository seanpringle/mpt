
#include "common.h"
#include "glsl.h"

struct estate {
	double h;
	SDF2 sdf;
};

static double extrudeEvaluate(void *p, vec3 pos) {
	struct estate *s = p;
	double d = s->sdf.evaluate(s->sdf.context, (vec2){pos.x, pos.y});
	vec2 w = (vec2){d, abs(pos.z)-s->h};
	return min(max(w.x, w.y), 0.0) + len(max(w, Zero2));
}

static sphere_t extrudeBounds(void *p) {
	struct estate *s = p;
	circle_t bounds = s->sdf.bounds;
	return (sphere_t){
		(vec3){bounds.center.x, bounds.center.y, 0},
		sqrt(bounds.radius*bounds.radius + s->h*s->h),
	};
}

SDF3 extrude(double h, SDF2 sdf) {
	struct estate *s = allot(sizeof(struct estate));
	s->h = h/2;
	s->sdf = sdf;
	return (SDF3){extrudeEvaluate, extrudeBounds(s), s};
}

struct rstate {
	double o;
	SDF2 sdf;
};

static double revolveEvaluate(void *p, vec3 pos) {
	struct rstate *s = p;
	vec2 v = v2(pos.x, pos.y);
	vec2 q = v2(len(v) - s->o, pos.z);
	return s->sdf.evaluate(s->sdf.context, q);
}

static sphere_t revolveBounds(void *p) {
	struct rstate *s = p;
	circle_t bounds = s->sdf.bounds;
	return (sphere_t){
		(vec3){bounds.center.x, bounds.center.y, 0},
		bounds.radius + s->o,
	};
}

SDF3 revolve(double o, SDF2 sdf) {
	struct rstate *s = allot(sizeof(struct rstate));
	s->o = o;
	s->sdf = sdf;
	return (SDF3){revolveEvaluate, revolveBounds(s), s};
}