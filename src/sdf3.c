
#include "common.h"
#include "glsl.h"

double SDF3Evaluate(SDF3 sdf, vec3 p) {
	return sdf.evaluate(sdf.context, p);
}

vec3 SDF3Normal(SDF3 sdf, vec3 p) {
	double step = 0.000001;
	vec3 gradient = v3(
		SDF3Evaluate(sdf, v3(p.x + step, p.y, p.z)) - SDF3Evaluate(sdf, v3(p.x - step, p.y, p.z)),
		SDF3Evaluate(sdf, v3(p.x, p.y + step, p.z)) - SDF3Evaluate(sdf, v3(p.x, p.y - step, p.z)),
		SDF3Evaluate(sdf, v3(p.x, p.y, p.z + step)) - SDF3Evaluate(sdf, v3(p.x, p.y, p.z - step))
	);
	return vec3Unit(gradient);
}

