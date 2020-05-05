
#include "common.h"

struct CSG {
	int count;
	SDF3 *sdfs;
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

static double bounds3Distance(sphere_t b, vec3 p) {
	return vec3Len(vec3Sub(p, b.center)) - b.radius;
}

static double unionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		if (i > 0) {
			double bd = bounds3Distance(sdf.bounds, pos);
			if (bd > dist) {
				continue;
			}
		}
		double d = SDF3Evaluate(sdf, pos);
		if (i == 0 || d < dist) {
			dist = d;
		}
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

static double differenceEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		if (i > 0) {
			double bd = bounds3Distance(sdf.bounds, pos);
			if (-bd < dist) {
				continue;
			}
		}
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

SDF3 subtractSDFs(int count, SDF3 *sdfs) {
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	memmove(s->sdfs, sdfs, count * sizeof(SDF3));
	return (SDF3){differenceEvaluate, csgBounds(s), s};
}

static double intersectionEvaluate(void *p, vec3 pos) {
	struct CSG *s = p;
	int count = s->count;
	SDF3 *sdfs = s->sdfs;

	double dist = 0;
	for (int i = 0; i < count; i++) {
		SDF3 sdf = sdfs[i];
		if (i > 0) {
			double bd = bounds3Distance(sdf.bounds, pos);
			if (bd > dist) {
				continue;
			}
		}
		double d = SDF3Evaluate(sdf, pos);
		if (i == 0 || d > dist) {
			dist = d;
		}
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
