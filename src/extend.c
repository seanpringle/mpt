
#include "common.h"
#include "glsl.h"

struct extrude {
	double h;
	SDF2 sdf;
};

static double extrudeEvaluate(void *p, vec3 pos) {
	struct extrude *s = p;
	double d = s->sdf.evaluate(s->sdf.context, (vec2){pos.x, pos.y});
	vec2 w = (vec2){d, abs(pos.z)-s->h};
	return min(max(w.x, w.y), 0.0) + len(max(w, Zero2));
}

static Bounds3 extrudeBounds(void *p) {
	struct extrude *s = p;
	Bounds2 bounds = s->sdf.bounds;
	return (Bounds3){
		(vec3){bounds.center.x, bounds.center.y, 0},
		sqrt(bounds.radius*bounds.radius + s->h*s->h),
	};
}

SDF3 Extrude(double h, SDF2 sdf) {
	struct extrude *s = allot(sizeof(struct extrude));
	s->h = h/2;
	s->sdf = sdf;
	return (SDF3){extrudeEvaluate, extrudeBounds(s), s};
}

struct revolve {
	double o;
	SDF2 sdf;
};

static double revolveEvaluate(void *p, vec3 pos) {
	struct revolve *s = p;
	vec2 v = v2(pos.x, pos.y);
	vec2 q = v2(len(v) - s->o, pos.z);
	return s->sdf.evaluate(s->sdf.context, q);
}

static Bounds3 revolveBounds(void *p) {
	struct revolve *s = p;
	Bounds2 bounds = s->sdf.bounds;
	return (Bounds3){
		(vec3){bounds.center.x, bounds.center.y, 0},
		bounds.radius + s->o,
	};
}

SDF3 Revolve(double o, SDF2 sdf) {
	struct revolve *s = allot(sizeof(struct revolve));
	s->o = o;
	s->sdf = sdf;
	return (SDF3){revolveEvaluate, revolveBounds(s), s};
}