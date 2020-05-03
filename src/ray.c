
#include "common.h"

static Color directLight(ray_t ray, vec3 pos) {
	Color color;
	for (object_t *t = objects; t != NULL; t = t->next) {
		Color light;
		material_t material = t->material;
		if (material.light(material.context, &light)) {
			sphere_t bounds = t->sdf.bounds;
			vec3 center = vec3Add(bounds.center, vec3Scale(pickVec3(ray.rnd), bounds.radius * scene.shadowR));
			ray_t lr = {.origin = pos, .direction = vec3Unit(vec3Sub(center, pos)), .rnd = ray.rnd};
			object_t *thing;
			vec3 hit;
			if (march(lr, NULL, &thing, &hit) && thing == t) {
				color = colorAdd(color, light);
			}
		}
	}
	return color;
}

void trace(ray_t ray, int depth, object_t *bypass, Color *rcolor, int *rbounces, double *ralpha) {

	Color color = Naught;
	int bounces = 0;
	double alpha = 1.0;

	ray_t shadow;
	Color scolor;
	int sbounces;
	double salpha;

	object_t *thing;
	vec3 hit;

	if (march(ray, bypass, &thing, &hit)) {
		material_t material = thing->material;

		if (depth == 0 && material.invisible) {
			alpha = scene.shadowH;
			if (colorBrightness(directLight(ray, hit)) > 0.0) {
				// an invisible material under direct light for a primary ray cannot catch shadows
				alpha = 0.0;
			}
		}

		if (alpha > 0.0 && depth < scene.bounces) {
			Color attenuation;
			if (material.scatter(material.context, ray, thing, hit, depth, &shadow, &attenuation)) {
				trace(shadow, depth+1, thing, &scolor, &sbounces, &salpha);

				// invisible surfaces pass through ambient lighting for non-primary ray hits
				if (depth > 0 && material.invisible) {
					attenuation = White;
					scolor = scene.ambient;
				}

				color = colorAdd(color, colorMul(attenuation, scolor));

				// invisible surfaces use nested shadow ray colors only to weight their own shadow alpha,
				// giving a soft-shadow prenumbra effect similar to real shadows on normal materials
				if (depth == 0 && material.invisible) {
					alpha = MIN(scene.shadowH, MAX(0.0, (alpha-(colorBrightness(scolor)/scene.shadowD))));
					color = attenuation;
				}

				bounces += sbounces;
			}

			Color light;
			if (material.light(material.context, &light)) {
				color = colorAdd(color, light);
			}

			bounces++;
		}

		*rcolor = color;
		*rbounces = bounces;
		*ralpha = alpha;
		return;
	}

	if (depth > 0) {
		*rcolor = scene.ambient;
		*rbounces = 0;
		*ralpha = 1.0;
		return;
	}

	*rcolor = Naught;
	*rbounces = 0;
	*ralpha = 0.0;
}

// ray marching by sphere tracing
bool march(ray_t ray, object_t *bypass, object_t **thing, vec3 *hit) {

	vec3 pos = ray.origin;

	// refracted rays bypass one object, then act normally
	while (bypass != NULL) {
		double dist = bypass->sdf.evaluate(bypass->sdf.context, pos);
		if (dist > 0) break;

		dist = MAX(fabs(dist), scene.threshold);
		pos = vec3Add(pos, vec3Scale(ray.direction, dist));
	}

	// shadow acne
	pos = vec3Add(pos, vec3Scale(ray.direction, scene.threshold * 10));

	// find all possible targets
	object_t* targets[objectCount];
	int count = 0;

	for (object_t *t = objects; t != NULL; t = t->next) {
		sphere_t bounds = t->sdf.bounds;

		// since objects have a bounding sphere, behave like a non-SDF
		// ray tracer and do a line-sphere intersection test to quickly
		// rule them in or out
		vec3 to = vec3Sub(ray.origin, bounds.center);
		double b = vec3Dot(to, ray.direction);
		double c = vec3Dot(to, to) - (bounds.radius*bounds.radius);
		double d = b*b - c;
		if (d > 0) {
			d = sqrt(d);
			double t1 = -b - d;
			if (t1 > scene.threshold) {
				targets[count++] = t;
				continue;
			}
			double t2 = -b + d;
			if (t2 > scene.threshold) {
				targets[count++] = t;
				continue;
			}
		}
	}

	if (count > 0) {
		// now behave like a path tracer and evaluate SDFs
		while (vec3Len(vec3Sub(pos, ray.origin)) < scene.horizon) {
			object_t *near = NULL;
			double dist = 0;

			for (int i = 0; i < count; i++) {
				object_t *t = targets[i];
				if (near != NULL) {
					double bd = sphereDistance(t->sdf.bounds.center, t->sdf.bounds.radius, pos);
					if (bd > dist) {
						continue;
					}
				}
				double d = t->sdf.evaluate(t->sdf.context, pos);
				if (d < dist || near == NULL) {
					near = t;
					dist = d;
					continue;
				}
			}

			if (dist < scene.threshold) {
				*thing = near;
				*hit = pos;
				return near != NULL;
			}

			pos = vec3Add(pos, vec3Scale(ray.direction, dist));
		}
	}

	*thing = NULL;
	*hit = Zero3;
	return false;
}

