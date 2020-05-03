
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

material_t shadows() {
	material_t m = matt(Black);
	m.invisible = true;
	return m;
}