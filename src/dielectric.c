
#include "common.h"

struct dielectric {
	Color color;
	double refractiveIndex;
};

static bool dielectricLight(void *p, Color *c) {
	struct dielectric *s = p;
	*c = s->color;
	return false;
}

static double schlick(double cosine, double refInd) {
	double r0 = (1.0 - refInd) / (1.0 + refInd);
	r0 = r0 * r0;
	return r0 + (1.0-r0)*pow(1.0-cosine, 5);
}

static bool dielectricScatter(void *p, ray_t ray, object_t *object, vec3 hit, int depth, ray_t *bounced, Color *color) {
	struct dielectric *s = p;

	vec3 normal = SDF3Normal(object->sdf, hit);

	vec3 outwardNormal = normal;
	double niOverNt = 1.0 / s->refractiveIndex;
	double cosine = -vec3Dot(vec3Unit(ray.direction), normal) / vec3Len(ray.direction);

	if (vec3Dot(ray.direction, normal) > 0) {
		outwardNormal = vec3Neg(normal);
		niOverNt = s->refractiveIndex;
		cosine = s->refractiveIndex * vec3Dot(vec3Unit(ray.direction), normal) / vec3Len(ray.direction);
	}

	vec3 direction = vec3Reflect(vec3Unit(ray.direction), normal);

	if (randomNormalized(ray.rnd) >= schlick(cosine, s->refractiveIndex)) {
		vec3 refracted;
		if (vec3Refract(ray.direction, outwardNormal, niOverNt, &refracted)) {
			direction = refracted;
		}
	}

	*bounced = (ray_t){hit, direction, ray.rnd};
	*color = s->color;
	return true;
}

material_t glass(Color c, double refractiveIndex) {
	struct dielectric *s = allot(sizeof(struct dielectric));
	s->color = c;
	s->refractiveIndex = refractiveIndex;
	return (material_t){ .invisible = false, .light = dielectricLight, .scatter = dielectricScatter, .context = s };
}