
#include "common.h"
#include "glsl.h"

struct CSG {
	int count;
	SDF3 *sdfs;
	union {
		double k;
	};
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

// https://www.iquilezles.org/www/articles/smin/smin.htm
static double smoothUnion(double a, double b, double k) {
	double h = max(k - abs(a-b), 0.0 ) / k;
	return min(a, b) - h*h*h*k*(1.0/6.0);
}

static double unionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		if (i == 0 || d < dist) {
			dist = d;
		}
	}
	return dist;
}

static double smoothUnionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		dist = (i == 0) ? d: smoothUnion(dist, d, s->k);
	}
	return dist;
}

SDF3 combineSDFs(int count, SDF3 *sdfs) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){unionEvaluate, csgBounds(s), s};
}

SDF3 smoothCombineSDFs(int count, SDF3 *sdfs, double k) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	s->k = k;
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){smoothUnionEvaluate, csgBounds(s), s};
}

static double differenceEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		if (i == 0) {
			dist = d;
		} else
		if (-d > dist) {
			dist = -d;
		}
	}
	return dist;
}

static double smoothDifference(double d1, double d2, double k) {
	double h = clamp(0.5 - 0.5*(d2+d1)/k, 0.0, 1.0);
	return mix( d2, -d1, h ) + k*h*(1.0-h);
}

static double smoothDifferenceEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		dist = (i == 0) ? d: smoothDifference(dist, d, s->k);
	}
	return dist;
}

SDF3 subtractSDFs(int count, SDF3 *sdfs) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){differenceEvaluate, csgBounds(s), s};
}

SDF3 smoothSubtractSDFs(int count, SDF3 *sdfs, double k) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	s->k = k;
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){smoothDifferenceEvaluate, csgBounds(s), s};
}

static double intersectionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		if (i == 0 || d > dist) {
			dist = d;
		}
	}
	return dist;
}

static double smoothIntersection(double d1, double d2, double k) {
	double h = clamp(0.5 - 0.5*(d2-d1)/k, 0.0, 1.0);
	return mix(d2, d1, h) + k*h*(1.0-h);
}

static double smoothIntersectionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		double d = SDF3Evaluate(sdf, pos);
		dist = (i == 0) ? d: smoothIntersection(dist, d, s->k);
	}
	return dist;
}

SDF3 intersectSDFs(int count, SDF3 *sdfs) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){intersectionEvaluate, csgBounds(s), s};
}

SDF3 smoothIntersectSDFs(int count, SDF3 *sdfs, double k) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	s->k = k;
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){smoothIntersectionEvaluate, csgBounds(s), s};
}

