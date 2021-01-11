
#include "common.h"
#include "glsl.h"

typedef double (*CSGcmp)(double,double,void*);

struct CSG {
	int count;
	SDF3 *sdfs;
	CSGcmp cmp;
	double k;
};

sphere_t csgBounds(void *p) {
	struct CSG *s = p;
	int count = s->count;

	vec3 centers = Zero3;
	double mradius = 0.0;

	int point = 0;
	vec3 points[count*6];

	for (int i = 0; i < count; i++) {
		sphere_t bounds = s->sdfs[i].bounds;
		mradius = MAX(mradius, bounds.radius);
		centers = vec3Add(centers, bounds.center);
		points[point++] = vec3Add(bounds.center, (vec3){bounds.radius, 0, 0});
		points[point++] = vec3Add(bounds.center, (vec3){-bounds.radius, 0, 0});
		points[point++] = vec3Add(bounds.center, (vec3){0, bounds.radius, 0});
		points[point++] = vec3Add(bounds.center, (vec3){0, -bounds.radius, 0});
		points[point++] = vec3Add(bounds.center, (vec3){0, 0, bounds.radius});
		points[point++] = vec3Add(bounds.center, (vec3){0, 0, -bounds.radius});
	}

	vec3 center = vec3Scale(centers, 1.0/(double)count);
	double radius = mradius;

	for (;;) {
		bool encompass = true;
		for (int i = 0; i < point; i++) {
			double d = vec3Len(vec3Sub(points[i], center)) - radius;
			if (d > 0) {
				encompass = false;
				radius += 1.0;
			}
		}
		if (encompass) {
			break;
		}
	}

	return (sphere_t){center, radius};
}

static double csgEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;

	int count = s->count;
	if (!count) return 0.0;

	SDF3 *sdfs = s->sdfs;
	const CSGcmp cmp = s->cmp;

	double dist = SDF3Evaluate(sdfs[0], pos);
	for (int i = 1; i < count; i++) {
		dist = cmp(SDF3Evaluate(sdfs[i], pos), dist, s);
	}
	return dist;
}

static SDF3 csgSDFs(int count, SDF3* sdfs, CSGcmp cmp, double k) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	s->cmp = cmp;
	s->k = k;
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){csgEvaluate, csgBounds(s), s};
}

static double simpleUnion(double a, double b, void* p) {
	return min(a, b);
}

static double smoothUnion(double a, double b, void* p) {
	double k = ((struct CSG*)p)->k;
	double h = max(k - abs(a-b), 0.0 ) / k;
	return min(a, b) - h*h*h*k*(1.0/6.0);
}

static double chamferUnion(double a, double b, void* p) {
	double k = ((struct CSG*)p)->k;
	return min(min(a, b), (a - k + b) * sqrt(0.5));
}

SDF3 csgUnion(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, simpleUnion, k);
}

SDF3 csgSmoothUnion(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, smoothUnion, k);
}

SDF3 csgChamferUnion(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, chamferUnion, k);
}

static double simpleIntersection(double a, double b, void* p) {
	return max(a, b);
}

static double smoothIntersection(double a, double b, void* p) {
	double k = ((struct CSG*)p)->k;
	double h = clamp(0.5 - 0.5*(b-a)/k, 0.0, 1.0);
	return mix(b, a, h) + k*h*(1.0-h);
}

static double chamferIntersection(double a, double b, void* p) {
	double k = ((struct CSG*)p)->k;
	return max(max(a, b), (a + k + b) * sqrt(0.5));
}

SDF3 csgIntersection(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, simpleIntersection, k);
}

SDF3 csgSmoothIntersection(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, smoothIntersection, k);
}

SDF3 csgChamferIntersection(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, chamferIntersection, k);
}

static double simpleDifference(double a, double b, void* p) {
	return max(-a, b);
}

static double smoothDifference(double a, double b, void* p) {
	double k = ((struct CSG*)p)->k;
	double h = clamp(0.5 - 0.5*(b+a)/k, 0.0, 1.0);
	return mix(b, -a, h) + k*h*(1.0-h);
}

static double chamferDifference(double a, double b, void* p) {
	return chamferIntersection(-a, b, p);
}

SDF3 csgDifference(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, simpleDifference, k);
}

SDF3 csgSmoothDifference(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, smoothDifference, k);
}

SDF3 csgChamferDifference(int count, SDF3 *sdfs, double k) {
	return csgSDFs(count, sdfs, chamferDifference, k);
}
