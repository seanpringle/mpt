
#include "common.h"

struct diffuse {
	Color color;
};

static bool diffuseLight(void *p, Color *c) {
	struct diffuse *s = p;
	*c = s->color;
	return false;
}

static bool diffuseScatter(void *p, ray_t ray, object_t *object, vec3 hit, int depth, ray_t *bounced, Color *color) {
	struct diffuse *s = p;
	vec3 normal = SDF3Normal(object->sdf, hit);
	vec3 redirection = vec3Unit(vec3Sub(vec3Add(vec3Add(hit, normal), pickVec3(ray.rnd)), hit));
	*bounced = (ray_t){.origin = hit, .direction = redirection, .rnd = ray.rnd};
	*color = s->color;
	return true;
}

material_t matt(Color c) {
	struct diffuse *s = allot(sizeof(struct diffuse));
	s->color = c;
	return (material_t){ .invisible = false, .light = diffuseLight, .scatter = diffuseScatter, .context = s };
}
