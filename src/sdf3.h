
typedef double (*sdf3)(void*,vec3);

typedef struct {
	sdf3 evaluate;
	Bounds3 bounds;
	void *context;
} SDF3;

vec3 SDF3Normal(SDF3 sdf, vec3 p);

double sphereDistance(vec3 center, double radius, vec3 pos);

SDF3 Sphere(double diameter);

SDF3 Cube(double x, double y, double z);

SDF3 CubeR(double x, double y, double z, double r);

SDF3 Cylinder(double height, double diameter);

SDF3 CylinderR(double height, double diameter, double r);

SDF3 Torus(double diameter, double section);

SDF3 Cone(double h, double d);

SDF3 ConeR(double h, double d, double r);

SDF3 TriPrism(double h, double w);

SDF3 Pyramid(double h, double w);

SDF3 PyramidR(double h, double w, double r);

SDF3 Capsule(double h, double d1, double d2);

SDF3 Round(double r, SDF3 sdf);

SDF3 Repeat(double cx, double cy, double cz, double ox, double oy, double oz, SDF3 sdf);

SDF3 Elongate(double x, double y, double z, SDF3 sdf);

SDF3 Ellipsoid(double x, double y, double z);

SDF3 Union(int count, ...);

SDF3 Difference(int count, ...);

SDF3 Intersection(int count, ...);

SDF3 Extrude(double h, SDF2 sdf);

SDF3 Revolve(double o, SDF2 sdf);

SDF3 Translate(vec3 v, SDF3 sdf);

SDF3 TranslateX(double t, SDF3 sdf);

SDF3 TranslateY(double t, SDF3 sdf);

SDF3 TranslateZ(double t, SDF3 sdf);

SDF3 Rotate(vec3 v, double deg, SDF3 sdf);

SDF3 RotateX(double deg, SDF3 sdf);

SDF3 RotateY(double deg, SDF3 sdf);

SDF3 RotateZ(double deg, SDF3 sdf);
