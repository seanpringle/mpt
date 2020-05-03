
#include "common.h"
#include "glsl.h"

#if 0

SDF3 Sphere(double diameter) {
	return Revolve(0, Circle(diameter));
}

#else

struct state {
	double radius;
};

static double distance(void *ptr, vec3 pos) {
	struct state *s = ptr;
	return len(pos) - s->radius;
}

SDF3 Sphere(double diameter) {
	struct state *s = allot(sizeof(struct state));
	s->radius = diameter/2;
	return (SDF3){distance, (sphere_t){Zero3,s->radius}, s};
}

#endif