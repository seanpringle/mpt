
#include "common.h"
#include "glsl.h"

#if 0

SDF3 Sphere(double diameter) {
	return Revolve(0, Circle(diameter));
}

#else

struct sphere {
	double radius;
};

static double distance(void *ptr, vec3 pos) {
	struct sphere *s = ptr;
	return len(pos) - s->radius;
}

SDF3 Sphere(double diameter) {
	struct sphere *s = allot(sizeof(struct sphere));
	s->radius = diameter/2;
	return (SDF3){distance, (Bounds3){Zero3,s->radius}, s};
}

#endif