
#include "common.h"
#include "glsl.h"

struct state {
	vec3 r;
};

static double ellipsoidEvaluate(void *ptr, vec3 p) {
	struct state *s = ptr;
	vec3 r = s->r;
	double k0 = len(div(p, r));
	double k1 = len(div(p, mul(r, r)));
	return k0 * (k0 - 1.0) / k1;
}

static sphere_t ellipsoidBounds(void *p) {
	struct state *s = p;
	return (sphere_t){Zero3, max(s->r.x, max(s->r.y, s->r.z)) * 2};
}

SDF3 ellipsoid(double x, double y, double z) {
	struct state *s = allot(sizeof(struct state));
	s->r = (vec3){x, y, z};
	return (SDF3){ellipsoidEvaluate, ellipsoidBounds(s), s};
}
