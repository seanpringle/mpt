
#include "common.h"
#include "glsl.h"

struct ellipse {
	vec2 ab;
};

static double ellipseEvaluate(void *pp, vec2 p) {
	struct ellipse *es = pp;
	vec2 ab = es->ab;

	p = abs(p);

	if (p.x > p.y) {
		p = v2(p.y, p.x);
		ab = v2(ab.y, ab.x);
	}

	double l = ab.y*ab.y - ab.x*ab.x;
	double m = ab.x*p.x/l;
	double m2 = m*m;
	double n = ab.y*p.y/l;
	double n2 = n*n;
	double c = (m2+n2-1.0)/3.0;
	double c3 = c*c*c;
	double q = c3 + m2*n2*2.0;
	double d = c3 + m2*n2;
	double g = m + m*n2;
	double co;

	if(d < 0.0) {
		double h = acos(q/c3)/3.0;
		double s = cos(h);
		double t = sin(h)*sqrt(3.0);
		double rx = sqrt( -c*(s + t + 2.0) + m2 );
		double ry = sqrt( -c*(s - t + 2.0) + m2 );
		co = (ry+sign(l)*rx+abs(g)/(rx*ry)- m)/2.0;
	} else {
		double h = 2.0*m*n*sqrt( d );
		double s = sign(q+h)*pow(abs(q+h), 1.0/3.0);
		double u = sign(q-h)*pow(abs(q-h), 1.0/3.0);
		double rx = -s - u - c*4.0 + 2.0*m2;
		double ry = (s - u)*sqrt(3.0);
		double rm = sqrt( rx*rx + ry*ry );
		co = (ry/sqrt(rm-rx)+2.0*g/rm-m)/2.0;
	}

	vec2 r = mul(ab, v2(co, sqrt(1.0-co*co)));
	return len(sub(r,p)) * sign(p.y-r.y);
}

static circle_t ellipseBounds(void *p) {
	struct ellipse *s = p;
	return (circle_t){Zero2, max(s->ab.x, s->ab.y)};
}

SDF2 ellipse(double w, double h) {
	struct ellipse *s = allot(sizeof(struct ellipse));
	s->ab = v2(w/2, h/2);
	return (SDF2){ellipseEvaluate, ellipseBounds(s), s};
}
