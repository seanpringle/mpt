
#include "common.h"
#include "glsl.h"

struct repeat {
	vec3 count;
	vec3 offset;
	SDF3 sdf;
};

static double repeatEvaluate(void *p, vec3 pos) {
	struct repeat *s = p;
	vec3 count = s->count;
	vec3 offset = s->offset;
	return s->sdf.evaluate(s->sdf.context,
		sub(pos, mul(
			clamp(
				vec3Round(
					div(pos, offset)
				),
				neg(count),
				count
			),
			offset
		))
	);
}

static Bounds3 repeatBounds(void *p) {
	struct repeat *s = p;
	Bounds3 bounds = s->sdf.bounds;
	double x = (s->offset.x + bounds.radius) * s->count.x;
	double y = (s->offset.y + bounds.radius) * s->count.y;
	double z = (s->offset.z + bounds.radius) * s->count.z;
	return (Bounds3){bounds.center, len(v3(x, y, z))};
}

SDF3 Repeat(double cx, double cy, double cz, double ox, double oy, double oz, SDF3 sdf) {
	struct repeat *s = allot(sizeof(struct repeat));
	s->count = v3(cx, cy, cz);
	s->offset = v3(ox, oy, oz);
	s->sdf = sdf;
	return (SDF3){repeatEvaluate, repeatBounds(s), s};
}
