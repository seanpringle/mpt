
#include "common.h"
#include "glsl.h"

vec3 pickVec3(struct random_data *rnd) {
	for (;;) {
		vec3 v = (vec3){
			randomNormalized(rnd)*2 - 1,
			randomNormalized(rnd)*2 - 1,
			randomNormalized(rnd)*2 - 1,
		};
		if (dot(v, v) < 1.0) {
			return v;
		}
	}
}

static bool diffuseBounce(ray_t ray, object_t *object, vec3 hit, ray_t *bounced) {
	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 redirect = unit(sub(add(add(hit, normal), pickVec3(ray.rnd)), hit));
	*bounced = (ray_t){.origin = hit, .direction = redirect, .rnd = ray.rnd};
	return true;
}

static bool specularBounce(ray_t ray, object_t *object, vec3 hit, ray_t *bounced) {
	material_t m = object->material;

	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 redirect = vec3Reflect(unit(ray.direction), normal);

	if (m.roughness > 0) {
		redirect = unit(add(add(redirect, normal),
			scale(sub(pickVec3(ray.rnd), normal), m.roughness)
		));
	}

	if (dot(redirect, normal) > 0) {
		*bounced = (ray_t){hit, redirect, ray.rnd};
		return true;
	}
	return false;
}

static double schlick(double cosine, double refInd) {
	double r0 = (1.0 - refInd) / (1.0 + refInd);
	r0 = r0 * r0;
	return r0 + (1.0-r0)*pow(1.0-cosine, 5);
}

static bool dielectricBounce(ray_t ray, object_t *object, vec3 hit, ray_t *bounced) {
	material_t m = object->material;

	vec3 normal = SDF3Normal(object->sdf, hit);

	vec3 outwardNormal = normal;
	double niOverNt = 1.0 / m.refractiveIndex;
	double cosine = -dot(unit(ray.direction), normal) / len(ray.direction);

	if (dot(ray.direction, normal) > 0) {
		outwardNormal = neg(normal);
		niOverNt = m.refractiveIndex;
		cosine = m.refractiveIndex * dot(unit(ray.direction), normal) / len(ray.direction);
	}

	vec3 direction = vec3Reflect(unit(ray.direction), normal);

	if (randomNormalized(ray.rnd) >= schlick(cosine, m.refractiveIndex)) {
		vec3 refracted;
		if (vec3Refract(ray.direction, outwardNormal, niOverNt, &refracted)) {
			direction = refracted;
		}
	}

	*bounced = (ray_t){hit, direction, ray.rnd};
	return true;
}

bool bounce(ray_t ray, object_t *object, vec3 hit, ray_t *bounced) {
	material_t m = object->material;
	if (m.diffuse) {
		return diffuseBounce(ray, object, hit, bounced);
	}
	if (m.specular) {
		return specularBounce(ray, object, hit, bounced);
	}
	if (m.dielectric) {
		return dielectricBounce(ray, object, hit, bounced);
	}
	return false;
}

material_t light(Color c) {
	return (material_t){.light = true, .color = c};
}

material_t matt(Color c) {
	return (material_t){.diffuse = true, .color = c};
}

material_t metal(Color c, double roughness) {
	return (material_t){.specular = true, .color = c, .roughness = roughness};
}

material_t glass(Color c, double refractiveIndex) {
	return (material_t){.dielectric = true, .color = c, .refractiveIndex = refractiveIndex};
}

material_t shadows() {
	return (material_t){.invisible = true, .diffuse = true};
}
