
#include "common.h"
#include "glsl.h"

#if 0

SDF3 Cube(double x, double y, double z) {
	return Extrude(z, Rectangle(x, y));
}

#else

struct state {
	vec3 box;
};

static double distance(void *ptr, vec3 pos) {
	struct state *s = ptr;
	vec3 q = sub(abs(pos), s->box);
	return len(max(q, Zero3)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

SDF3 Cube(double x, double y, double z) {
	struct state *s = allot(sizeof(struct state));
	s->box = v3(x/2,y/2,z/2);
	return (SDF3){distance, (sphere_t){Zero3,len(s->box)}, s};
}

#endif