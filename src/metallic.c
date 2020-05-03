
#include "common.h"

struct metallic {
	Color color;
	double roughness;
};

static bool metallicLight(void *p, Color *c) {
	struct metallic *s = p;
	*c = s->color;
	return false;
}

static bool metallicScatter(void *p, ray_t ray, object_t *object, vec3 hit, int depth, ray_t *bounced, Color *color) {
	struct metallic *s = p;

	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 reflected = vec3Reflect(vec3Unit(ray.direction), normal);

	if (s->roughness > 0) {
		reflected = vec3Unit(vec3Add(vec3Add(reflected, normal),
			vec3Scale(vec3Sub(pickVec3(ray.rnd), normal), s->roughness)
		));
	}

	if (vec3Dot(reflected, normal) > 0) {
		*bounced = (ray_t){hit, reflected, ray.rnd};
		*color = s->color;
		return true;
	}

	memset(bounced, 0, sizeof(ray_t));
	*color = Naught;
	return false;
}

material_t metal(Color c, double roughness) {
	struct metallic *s = allot(sizeof(struct metallic));
	s->color = c;
	s->roughness = roughness;
	return (material_t){ .invisible = false, .light = metallicLight, .scatter = metallicScatter, .context = s };
}