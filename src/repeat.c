
#include "common.h"
#include "glsl.h"

struct state {
	vec3 count;
	vec3 offset;
	SDF3 sdf;
};

static double repeatEvaluate(void *p, vec3 pos) {
	struct state *s = p;
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

static sphere_t repeatBounds(void *p) {
	struct state *s = p;
	sphere_t bounds = s->sdf.bounds;
	double x = (s->offset.x + bounds.radius) * s->count.x;
	double y = (s->offset.y + bounds.radius) * s->count.y;
	double z = (s->offset.z + bounds.radius) * s->count.z;
	return (sphere_t){bounds.center, len(v3(x, y, z))};
}

SDF3 repeat(double cx, double cy, double cz, double ox, double oy, double oz, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->count = v3(cx, cy, cz);
	s->offset = v3(ox, oy, oz);
	s->sdf = sdf;
	return (SDF3){repeatEvaluate, repeatBounds(s), s};
}
