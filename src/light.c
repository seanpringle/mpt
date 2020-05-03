
#include "common.h"

struct lstate {
	Color color;
};

static bool lightLight(void *p, Color *c) {
	struct lstate *s = p;
	*c = s->color;
	return true;
}

static bool lightScatter(void *p, ray_t ray, object_t *object, vec3 hit, int depth, ray_t *bounced, Color *color) {
	memset(bounced, 0, sizeof(ray_t));
	*color = Naught;
	return false;
}

material_t light(Color c) {
	struct lstate *s = allot(sizeof(struct lstate));
	s->color = c;
	return (material_t){ .invisible = false, .light = lightLight, .scatter = lightScatter, .context = s };
}