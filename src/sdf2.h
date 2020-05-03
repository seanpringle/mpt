
typedef double (*sdf2)(void*,vec2);

typedef struct {
	sdf2 evaluate;
	circle_t bounds;
	void *context;
} SDF2;

double SDF2Evaluate(SDF2 sdf, vec2 p);

SDF2 circle(double diameter);

SDF2 rectangle(double x, double y);

SDF2 triangle(vec2 p0, vec2 p1, vec2 p2);

SDF2 polygon(int n, double d);

SDF2 stadium(double h, double d1, double d2);

SDF2 parabola(double w, double h);

SDF2 hexagram(double diameter);
