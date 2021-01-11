
#include "common.h"
#include "glsl.h"

ray_t emitPerspective(double imageX, double imageY, int imageW, int imageH, struct random_data *rnd) {
	camera_t *c = &camera;

	double aspect = (double)imageW / (double)imageH;

	double px = (imageX / ((double)imageW-1)) * 2 - 1;
	double py = (imageY / ((double)imageH-1)) * 2 - 1;

	vec3 direction = unit(
		add(add(scale(c->w, c->m), scale(c->u, -px * aspect)), scale(c->v, -py))
	);

	ray_t ray = { .origin = c->origin, .direction = direction, .rnd = rnd };

	if (c->aperture > 0) {
		vec3 focus = add(c->origin, scale(direction, c->focus));
		double angle = randomNormalized(ray.rnd) * 2 * M_PI;
		double radius = randomNormalized(ray.rnd) * c->aperture;
		ray.origin = add(ray.origin, scale(c->u, cos(angle)*radius));
		ray.origin = add(ray.origin, scale(c->v, sin(angle)*radius));
		ray.direction = unit(sub(focus, ray.origin));
	}
	return ray;
}

void perspective(vec3 lookFrom, vec3 lookAt, vec3 vup, double vfov, vec3 focus, double aperture) {
	camera_t *c = &camera;
	c->origin = lookFrom;
	c->target = lookAt;
	c->w = unit(sub(lookAt, lookFrom));
	c->u = unit(cross(vup, c->w));
	c->v = unit(cross(c->w, c->u));
	c->m = 1.0 / tan(vfov*M_PI/360.0);
	c->focus = len(sub(focus, c->origin));
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

	origin = add(origin, scale(scale(c->u, -px * aspect), c->scaleU));
	origin = add(origin, scale(scale(c->v, -py), c->scaleV));

	return (ray_t){ .origin = origin, .direction = direction, .rnd = rnd };
}

void orthographic(vec3 lookFrom, vec3 lookAt, vec3 vup, double scaleU, double scaleV) {
	camera_t *c = &camera;
	c->origin = lookFrom;
	c->target = lookAt;
	c->w = unit(sub(lookAt, lookFrom));
	c->u = unit(cross(vup, c->w));
	c->v = unit(cross(c->w, c->u));
	c->scaleU = scaleU/2;
	c->scaleV = scaleV/2;
	c->emit = emitOrthographic;
}
