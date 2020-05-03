
#include "common.h"
#include "glsl.h"

#if 0

SDF3 Cube(double x, double y, double z) {
	return Extrude(z, Rectangle(x, y));
}

#else

struct cube {
	vec3 box;
};

static double cubeDistance(void *ptr, vec3 pos) {
	struct cube *s = ptr;
	vec3 q = sub(abs(pos), s->box);
	return len(max(q, Zero3)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

SDF3 Cube(double x, double y, double z) {
	struct cube *s = allot(sizeof(struct cube));
	s->box = v3(x/2,y/2,z/2);
	return (SDF3){cubeDistance, (Bounds3){Zero3,len(s->box)}, s};
}

#endif