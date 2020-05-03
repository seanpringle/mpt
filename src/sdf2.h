
typedef double (*sdf2)(void*,vec2);

typedef struct {
	sdf2 evaluate;
	Bounds2 bounds;
	void *context;
} SDF2;

SDF2 Circle(double diameter);

SDF2 Rectangle(double x, double y);

SDF2 Triangle(vec2 p0, vec2 p1, vec2 p2);

SDF2 Polygon(int n, double r);

SDF2 Stadium(double h, double r1, double r2);

SDF2 Parabola(double w, double h);

SDF2 Hexagram(double diameter);
