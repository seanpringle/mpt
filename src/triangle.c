
#include "common.h"
#include "glsl.h"

struct triangle {
	vec2 p0, p1, p2;
};

static double triangleEvaluate(void *p, vec2 pos) {
	struct triangle *s = p;
	vec2 e0 = sub(s->p1, s->p0);
	vec2 e1 = sub(s->p2, s->p1);
	vec2 e2 = sub(s->p0, s->p2);
	vec2 v0 = sub(pos, s->p0);
	vec2 v1 = sub(pos, s->p1);
	vec2 v2 = sub(pos, s->p2);
	vec2 pq0 = sub(v0, scale(e0, clamp(dot(v0, e0)/dot(e0, e0), 0.0, 1.0)));
	vec2 pq1 = sub(v1, scale(e1, clamp(dot(v1, e1)/dot(e1, e1), 0.0, 1.0)));
	vec2 pq2 = sub(v2, scale(e2, clamp(dot(v2, e2)/dot(e2, e2), 0.0, 1.0)));
	double ss = sign(e0.x*e2.y - e0.y*e2.x);
	vec2 d = min(min(
		((vec2){dot(pq0, pq0), ss*(v0.x*e0.y-v0.y*e0.x)}),
		((vec2){dot(pq1, pq1), ss*(v1.x*e1.y-v1.y*e1.x)})),
		((vec2){dot(pq2, pq2), ss*(v2.x*e2.y-v2.y*e2.x)}));
	return -sqrt(d.x) * sign(d.y);
}

static Bounds2 triangleBounds(void *p) {
	struct triangle *s = p;
	return (Bounds2){Zero2, max(max(len(s->p0), len(s->p1)), len(s->p2))};
}

SDF2 Triangle(vec2 p0, vec2 p1, vec2 p2) {
	struct triangle *s = allot(sizeof(struct triangle));
	s->p0 = p0;
	s->p1 = p1;
	s->p2 = p2;
	return (SDF2){triangleEvaluate, triangleBounds(s), s};
}
