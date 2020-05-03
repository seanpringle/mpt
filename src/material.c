
#include "common.h"

vec3 pickVec3(struct random_data *rnd) {
	for (;;) {
		vec3 v = (vec3){
			randomNormalized(rnd)*2 - 1,
			randomNormalized(rnd)*2 - 1,
			randomNormalized(rnd)*2 - 1,
		};
		if (vec3Dot(v, v) < 1.0) {
			return v;
		}
	}
}

struct light {
	Color color;
};

static bool lightLight(void *p, Color *c) {
	struct light *s = p;
	*c = s->color;
	return true;
}

static bool lightScatter(void *p, Ray ray, Object *object, vec3 hit, int depth, Ray *bounced, Color *color) {
	memset(bounced, 0, sizeof(Ray));
	*color = Naught;
	return false;
}

Material Light(Color c) {
	struct light *s = allot(sizeof(struct light));
	s->color = c;
	return (Material){ .invisible = false, .light = lightLight, .scatter = lightScatter, .context = s };
}

struct diffuse {
	Color color;
};

static bool diffuseLight(void *p, Color *c) {
	struct diffuse *s = p;
	*c = s->color;
	return false;
}

static bool diffuseScatter(void *p, Ray ray, Object *object, vec3 hit, int depth, Ray *bounced, Color *color) {
	struct diffuse *s = p;
	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 redirection = vec3Unit(vec3Sub(vec3Add(vec3Add(hit, normal), pickVec3(ray.rnd)), hit));
	*bounced = (Ray){.origin = hit, .direction = redirection, .rnd = ray.rnd};
	*color = s->color;
	return true;
}

Material Matt(Color c) {
	struct diffuse *s = allot(sizeof(struct diffuse));
	s->color = c;
	return (Material){ .invisible = false, .light = diffuseLight, .scatter = diffuseScatter, .context = s };
}

struct metallic {
	Color color;
	double roughness;
};

static bool metallicLight(void *p, Color *c) {
	struct metallic *s = p;
	*c = s->color;
	return false;
}

static bool metallicScatter(void *p, Ray ray, Object *object, vec3 hit, int depth, Ray *bounced, Color *color) {
	struct metallic *s = p;

	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 reflected = vec3Reflect(vec3Unit(ray.direction), normal);

	if (s->roughness > 0) {
		reflected = vec3Unit(vec3Add(vec3Add(reflected, normal),
			vec3Scale(vec3Sub(pickVec3(ray.rnd), normal), s->roughness)
		));
	}

	if (vec3Dot(reflected, normal) > 0) {
		*bounced = (Ray){hit, reflected, ray.rnd};
		*color = s->color;
		return true;
	}

	memset(bounced, 0, sizeof(Ray));
	*color = Naught;
	return false;
}

Material Metal(Color c, double roughness) {
	struct metallic *s = allot(sizeof(struct metallic));
	s->color = c;
	s->roughness = roughness;
	return (Material){ .invisible = false, .light = metallicLight, .scatter = metallicScatter, .context = s };
}

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

static bool dielectricScatter(void *p, Ray ray, Object *object, vec3 hit, int depth, Ray *bounced, Color *color) {
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

	*bounced = (Ray){hit, direction, ray.rnd};
	*color = s->color;
	return true;
}

Material Glass(Color c, double refractiveIndex) {
	struct dielectric *s = allot(sizeof(struct dielectric));
	s->color = c;
	s->refractiveIndex = refractiveIndex;
	return (Material){ .invisible = false, .light = dielectricLight, .scatter = dielectricScatter, .context = s };
}

Material ShadowCatcher() {
	Material m = Matt(Black);
	m.invisible = true;
	return m;
}