
typedef double (*sdf3)(void*,vec3);

typedef struct {
	sdf3 evaluate;
	sphere_t bounds;
	void *context;
} SDF3;

double SDF3Evaluate(SDF3 sdf, vec3 p);

vec3 SDF3Normal(SDF3 sdf, vec3 p);

SDF3 rounded(double r, SDF3 sdf);

SDF3 repeat(double cx, double cy, double cz, double ox, double oy, double oz, SDF3 sdf);

SDF3 extend(double x, double y, double z, SDF3 sdf);

SDF3 csgUnion(int count, SDF3 *sdfs, double k);

SDF3 csgSmoothUnion(int count, SDF3 *sdfs, double k);

SDF3 csgChamferUnion(int count, SDF3 *sdfs, double k);

SDF3 csgDifference(int count, SDF3 *sdfs, double k);

SDF3 csgSmoothDifference(int count, SDF3 *sdfs, double k);

SDF3 csgChamferDifference(int count, SDF3 *sdfs, double k);

SDF3 csgIntersection(int count, SDF3 *sdfs, double k);

SDF3 csgSmoothIntersection(int count, SDF3 *sdfs, double k);

SDF3 csgChamferIntersection(int count, SDF3 *sdfs, double k);

SDF3 extrude(double h, SDF2 sdf);

SDF3 revolve(double o, SDF2 sdf);

SDF3 translate(vec3 v, SDF3 sdf);

SDF3 rotate(vec3 v, double deg, SDF3 sdf);
