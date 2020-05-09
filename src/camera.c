
#include "common.h"

ray_t emitPerspective(double imageX, double imageY, int imageW, int imageH, struct random_data *rnd) {
	camera_t *c = &camera;

	double aspect = (double)imageW / (double)imageH;

	double px = (imageX / ((double)imageW-1)) * 2 - 1;
	double py = (imageY / ((double)imageH-1)) * 2 - 1;

	vec3 direction = vec3Unit(
		vec3Add(vec3Add(vec3Scale(c->w, c->m), vec3Scale(c->u, -px * aspect)), vec3Scale(c->v, -py))
	);

	ray_t ray = { .origin = c->origin, .direction = direction, .rnd = rnd };

	if (c->aperture > 0) {
		vec3 focus = vec3Add(c->origin, vec3Scale(direction, c->focus));
		double angle = randomNormalized(ray.rnd) * 2 * M_PI;
		double radius = randomNormalized(ray.rnd) * c->aperture;
		ray.origin = vec3Add(ray.origin, vec3Scale(c->u, cos(angle)*radius));
		ray.origin = vec3Add(ray.origin, vec3Scale(c->v, sin(angle)*radius));
		ray.direction = vec3Unit(vec3Sub(focus, ray.origin));
	}
	return ray;
}

void perspective(vec3 lookFrom, vec3 lookAt, vec3 vup, double vfov, vec3 focus, double aperture) {
	camera_t *c = &camera;
	c->origin = lookFrom;
	c->target = lookAt;
	c->w = vec3Unit(vec3Sub(lookAt, lookFrom));
	c->u = vec3Unit(vec3Cross(vup, c->w));
	c->v = vec3Unit(vec3Cross(c->w, c->u));
	c->m = 1.0 / tan(vfov*M_PI/360.0);
	c->focus = vec3Len(vec3Sub(focus, c->origin));
	c->aperture = aperture;
	c->emit = emitPerspective;
}

ray_t emitOrthographic(double imageX, double imageY, int imageW, int imageH, struct random_data *rnd) {
	camera_t *c = &camera;

	double aspect = (double)imageW / (double)imageH;

	vec3 direction = c->w;

	double px = (imageX / ((double)imageW-1)) * 2 - 1;
	double py = (imageY / ((double)imageH-1)) * 2 - 1;

	vec3 origin = c->origin;

	origin = vec3Add(origin, vec3Scale(vec3Scale(c->u, -px * aspect), c->scale));
	origin = vec3Add(origin, vec3Scale(vec3Scale(c->v, -py), c->scale));

	return (ray_t){ .origin = origin, .direction = direction, .rnd = rnd };
}

void orthographic(vec3 lookFrom, vec3 lookAt, vec3 vup, double scale) {
	camera_t *c = &camera;
	c->origin = lookFrom;
	c->target = lookAt;
	c->w = vec3Unit(vec3Sub(lookAt, lookFrom));
	c->u = vec3Unit(vec3Cross(vup, c->w));
	c->v = vec3Unit(vec3Cross(c->w, c->u));
	c->scale = scale/2;
	c->emit = emitOrthographic;
}
