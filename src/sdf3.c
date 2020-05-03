
#include "common.h"
#include "glsl.h"

vec3 Zero3;

double sphereDistance(vec3 center, double radius, vec3 pos) {
	return len(sub(pos, center)) - radius;
}

vec3 SDF3Normal(SDF3 sdf, vec3 p) {
	double step = 0.000001;
	vec3 gradient = (vec3){
		sdf.evaluate(sdf.context, (vec3){p.x + step, p.y, p.z}) - sdf.evaluate(sdf.context, (vec3){p.x - step, p.y, p.z}),
		sdf.evaluate(sdf.context, (vec3){p.x, p.y + step, p.z}) - sdf.evaluate(sdf.context, (vec3){p.x, p.y - step, p.z}),
		sdf.evaluate(sdf.context, (vec3){p.x, p.y, p.z + step}) - sdf.evaluate(sdf.context, (vec3){p.x, p.y, p.z - step}),
	};
	return vec3Unit(gradient);
}

SDF3 Cylinder(double height, double diameter) {
	return Extrude(height, Circle(diameter));
}

SDF3 Torus(double diameter, double section) {
	return Revolve(diameter/2, Circle(section));
}

SDF3 TriPrism(double h, double w) {
	return Extrude(h, Triangle(
		(vec2){0, w/2}, (vec2){-w/2, -w/2}, (vec2){w/2, -w/2}
	));
}

SDF3 Pyramid(double h, double w) {
	SDF3 prism = TriPrism(h, w);
	return RotateX(-90, Intersect(2, prism, RotateY(90, prism)));
}

SDF3 Capsule(double h, double d1, double d2) {
	return Revolve(0, Stadium(h, d1, d2));
}

SDF3 CubeR(double x, double y, double z, double r) {
	return Round(r, Cube(x-r*2, y-r*2, z-r*2));
}

SDF3 CylinderR(double h, double d, double r) {
	return Round(r, Cylinder(h-r*2, d-r*2));
}

SDF3 ConeR(double h, double d, double r) {
	return Round(r, Cone(h-r*2, d-r*2));
}

SDF3 PyramidR(double h, double d, double r) {
	return Round(r, Pyramid(h-r*2, d-r*2));
}
