
#include "common.h"
#include "glsl.h"

vec3 Zero3;

double sphereDistance(vec3 center, double radius, vec3 pos) {
	return len(sub(pos, center)) - radius;
}

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

SDF3 cylinder(double height, double diameter) {
	return extrude(height, circle(diameter));
}

SDF3 torus(double diameter, double section) {
	return revolve(diameter/2, circle(section));
}

SDF3 triPrism(double h, double w) {
	return extrude(h, triangle(
		(vec2){0, w/2}, (vec2){-w/2, -w/2}, (vec2){w/2, -w/2}
	));
}

SDF3 pyramid(double h, double w) {
	SDF3 prism = triPrism(h, w);
	return rotateX(-90, intersect(2, prism, rotateY(90, prism)));
}

SDF3 capsule(double h, double d1, double d2) {
	return revolve(0, stadium(h, d1, d2));
}

SDF3 cubeR(double x, double y, double z, double r) {
	return rounded(r, cube(x-r*2, y-r*2, z-r*2));
}

SDF3 cylinderR(double h, double d, double r) {
	return rounded(r, cylinder(h-r*2, d-r*2));
}

SDF3 coneR(double h, double d, double r) {
	return rounded(r, cone(h-r*2, d-r*2));
}

SDF3 pyramidR(double h, double d, double r) {
	return rounded(r, pyramid(h-r*2, d-r*2));
}
