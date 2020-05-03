
typedef double (*sdf3)(void*,vec3);

typedef struct {
	sdf3 evaluate;
	sphere_t bounds;
	void *context;
} SDF3;

vec3 SDF3Normal(SDF3 sdf, vec3 p);

double sphereDistance(vec3 center, double radius, vec3 pos);

SDF3 sphere(double diameter);

SDF3 cube(double x, double y, double z);

SDF3 cubeR(double x, double y, double z, double r);

SDF3 cylinder(double height, double diameter);

SDF3 cylinderR(double height, double diameter, double r);

SDF3 torus(double diameter, double section);

SDF3 cone(double h, double d);

SDF3 coneR(double h, double d, double r);

SDF3 triPrism(double h, double w);

SDF3 pyramid(double h, double w);

SDF3 pyramidR(double h, double w, double r);

SDF3 capsule(double h, double d1, double d2);

SDF3 rounded(double r, SDF3 sdf);

SDF3 repeat(double cx, double cy, double cz, double ox, double oy, double oz, SDF3 sdf);

SDF3 elongate(double x, double y, double z, SDF3 sdf);

SDF3 ellipsoid(double x, double y, double z);

SDF3 combine(int count, ...);

SDF3 subtract(int count, ...);

SDF3 intersect(int count, ...);

SDF3 extrude(double h, SDF2 sdf);

SDF3 revolve(double o, SDF2 sdf);

SDF3 translate(vec3 v, SDF3 sdf);

SDF3 translateX(double t, SDF3 sdf);

SDF3 translateY(double t, SDF3 sdf);

SDF3 translateZ(double t, SDF3 sdf);

SDF3 rotate(vec3 v, double deg, SDF3 sdf);

SDF3 rotateX(double deg, SDF3 sdf);

SDF3 rotateY(double deg, SDF3 sdf);

SDF3 rotateZ(double deg, SDF3 sdf);
