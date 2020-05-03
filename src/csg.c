
#include "common.h"

struct CSG {
	int count;
	SDF3 *sdfs;
};

Bounds3 csgBounds(void *p) {
	struct CSG *s = p;
	int count = s->count;

	vec3 centers = Zero3;
	double mradius = 0.0;

	int point = 0;
	vec3 points[count*6];

	for (int i = 0; i < count; i++) {
		Bounds3 bounds = s->sdfs[i].bounds;
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

	return (Bounds3){center, radius};
}

static double bounds3Distance(Bounds3 b, vec3 p) {
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
		double d = sdf.evaluate(sdf.context, pos);
		if (i == 0 || d < dist) {
			dist = d;
		}
	}
	return dist;
}

SDF3 Union(int count, ...) {
	va_list aa;
	va_start(aa, count);
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	for (int i = 0; i < count; i++) {
		s->sdfs[i] = va_arg(aa, SDF3);
	}
	va_end(aa);
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
		double d = sdf.evaluate(sdf.context, pos);
		if (i == 0) {
			dist = d;
		} else
		if (-d > dist) {
			dist = -d;
		}
	}
	return dist;
}

SDF3 Difference(int count, ...) {
	va_list aa;
	va_start(aa, count);
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	for (int i = 0; i < count; i++) {
		s->sdfs[i] = va_arg(aa, SDF3);
	}
	va_end(aa);
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
		double d = sdf.evaluate(sdf.context, pos);
		if (i == 0 || d > dist) {
			dist = d;
		}
	}
	return dist;
}

SDF3 Intersection(int count, ...) {
	va_list aa;
	va_start(aa, count);
	struct CSG *s = allot(sizeof(struct CSG));
	s->count = count;
	s->sdfs = allot(count * sizeof(SDF3));
	for (int i = 0; i < count; i++) {
		s->sdfs[i] = va_arg(aa, SDF3);
	}
	va_end(aa);
	return (SDF3){intersectionEvaluate, csgBounds(s), s};
}
