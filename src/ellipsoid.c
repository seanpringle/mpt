
#include "common.h"
#include "glsl.h"

struct ellipsoid {
	vec3 r;
};

static double ellipsoidEvaluate(void *ptr, vec3 p) {
	struct ellipsoid *s = ptr;
	vec3 r = s->r;
	double k0 = len(div(p, r));
	double k1 = len(div(p, mul(r, r)));
	return k0 * (k0 - 1.0) / k1;
}

static Bounds3 ellipsoidBounds(void *p) {
	struct ellipsoid *s = p;
	return (Bounds3){Zero3, max(s->r.x, max(s->r.y, s->r.z)) * 2};
}

SDF3 Ellipsoid(double x, double y, double z) {
	struct ellipsoid *s = allot(sizeof(struct ellipsoid));
	s->r = (vec3){x, y, z};
	return (SDF3){ellipsoidEvaluate, ellipsoidBounds(s), s};
}
