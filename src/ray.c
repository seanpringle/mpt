
#include "common.h"
#include "glsl.h"

// Sample all lights directly visible from a point.
static Color directLight(ray_t ray, vec3 pos) {
	Color color = Naught;

	for (object_t *t = objects; t != NULL; t = t->next) {
		material_t material = t->material;

		if (material.light && material.lightDirect) {
			sphere_t bounds = t->sdf.bounds;
			vec3 center = add(bounds.center, scale(pickVec3(ray.rnd), bounds.radius * scene.shadowR));
			ray_t lr = {.origin = pos, .direction = unit(sub(center, pos)), .rnd = ray.rnd};

			object_t *thing = NULL;
			if (march(lr, NULL, &thing, NULL) && thing == t) {
				color = colorAdd(color, material.color);
			}
		}
	}
	return color;
}

// Trace a ray through the scene.
void trace(ray_t ray, int depth, object_t *bypass, Color *rcolor, int *rbounces, double *ralpha) {

	Color color = Naught;
	int bounces = 0;
	double alpha = 1.0;

	ray_t shadow;
	Color scolor = Naught;
	int sbounces = 0;
	double salpha = 0;

	object_t *thing = NULL;
	vec3 hit = Zero3;

	if (march(ray, bypass, &thing, &hit)) {
		material_t material = thing->material;

		if (depth == 0 && material.invisible) {
			alpha = scene.shadowH;
			if (colorBrightness(directLight(ray, hit)) > 0.0) {
				// An invisible material under direct light for a primary ray cannot catch shadows.
				alpha = 0.0;
			}
		}

		if (alpha > 0.0 && depth < scene.bounces) {
			Color attenuation = material.color;

			if (bounce(ray, thing, hit, &shadow)) {
				trace(shadow, depth+1, thing, &scolor, &sbounces, &salpha);

				// Ambient lighting is added when a shadow ray fails to find anything. This could be
				// due to crossing the scene horizon, so ambient is in effect the skybox colour, or
				// because a ray exceeded the bounce limit, making ambient a fake omnipresent light
				// source guaranteed to be hit by default.
				if (sbounces == 0) {
					scolor = scene.ambient;
				}

				// Invisible surfaces pass through ambient lighting for non-primary ray hits.
				if (depth > 0 && material.invisible) {
					attenuation = White;
					scolor = scene.ambient;
					//alpha = 1.0;
				}

				color = colorAdd(color, colorMul(attenuation, scolor));

				// Invisible surfaces use nested shadow ray colors only to weight their own shadow alpha,
				// giving a soft-shadow prenumbra effect similar to real shadows on normal materials.
				if (depth == 0 && material.invisible) {
					alpha = MIN(scene.shadowH, MAX(0.0, (alpha-(colorBrightness(scolor)/scene.shadowD))));
					color = attenuation;
					//color = Black;
				}

				bounces += sbounces;
			}

			if (material.light) {
				color = colorAdd(color, material.color);
			}

			bounces++;
		}

		*rcolor = color;
		*rbounces = bounces;
		*ralpha = alpha;
		return;
	}

	*rcolor = Naught;
	*rbounces = 0;
	*ralpha = 0.0;
}

// March a ray by sphere tracing.
bool march(ray_t ray, object_t *bypass, object_t **thing, vec3 *hit) {
	vec3 pos = ray.origin;

	// Refracted rays pass through one object, then act normally.
	while (bypass != NULL) {
		double dist = SDF3Evaluate(bypass->sdf, pos);
		if (dist > 0) break;
		dist = MAX(abs(dist), scene.threshold);
		pos = add(pos, scale(ray.direction, dist));
	}

	// Avoid shadow acne by moving the origin away. For a primary ray the origin
	// is the camera which is very unlikely to have an object pressed up against
	// its lens. For a secondary ray the origin is within scene.threshold distance
	// from the primary object hit which would immediately cause a false double hit.
	//
	// Note also that the multiple 10 could be as low as 2 but there's a secondary
	// performance issue: starting too close to the threshold means more sphere
	// marching steps just to move away from the previous object, because marching
	// steps know only about absolute distance and not direction.
	pos = add(pos, scale(ray.direction, scene.threshold * 10));

	// There are two approaches to marching:
	//
	// a) For each march iteration query a spatial index for nearest neighbours.
	//    The index must support at least bounding boxes; points won't do.
	//    Faster, more complex, can be cache unfriendly, uses more memory, requires
	//    a reentrant index or locks if multithreading.
	//
	// b) First reduce the possible set of objects to be tested, then for each march
	//    iteration check only that subset. Slower, less complex, cache friendly,
	//    uses little memory, easy to multithread.
	//
	// For a small number of objects nobody cares about indexing so (b) is a cheap
	// but minor improvement over doing naive brute-force checks on all objects.
	//
	// For a large number of objects the choice isn't clear and depends on the
	// layout of objects in the scene. (a) is a good general purpose solution but
	// (b) isn't necessarily far behind. Therefore since most examples in the wild
	// talk about (a) we'll give (b) a try and see what happens. Perhaps other
	// unexpected opportunities for optimization will appear...

	// Identify all objects with bounding spheres that intersect the ray.
	object_t* targets[objectCount];
	int count = 0;

	for (object_t *t = objects; t != NULL; t = t->next) {
		sphere_t bounds = t->sdf.bounds;

		// Since objects have a bounding sphere, behave like a non-SDF ray tracer
		// and do a line-sphere intersection test to rule objects in or out before
		// marching.
		vec3 to = sub(ray.origin, bounds.center);
		double b = dot(to, ray.direction);
		double c = dot(to, to) - (bounds.radius*bounds.radius);
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
		// Now behave like a path tracer and march, but evaluating SDFs only for
		// the identified targets.
		while (
			pos.x < scene.horizon && pos.x > -scene.horizon &&
			pos.y < scene.horizon && pos.y > -scene.horizon &&
			pos.z < scene.horizon && pos.z > -scene.horizon
		){
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
				double d = SDF3Evaluate(t->sdf, pos);
				if (d < dist || near == NULL) {
					near = t;
					dist = d;
					continue;
				}
			}

			if (dist < scene.threshold) {
				if (thing != NULL) *thing = near;
				if (hit != NULL) *hit = pos;
				return near != NULL;
			}

			pos = add(pos, scale(ray.direction, dist));
		}
	}

	if (thing != NULL) *thing = NULL;
	if (hit != NULL) *hit = Zero3;
	return false;
}

