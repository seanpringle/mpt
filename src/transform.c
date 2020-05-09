
#include "common.h"

inline vec3 matrixMul(struct matrix44 a, vec3 b) {
	double x = a.x00*b.x + a.x01*b.y + a.x02*b.z + a.x03;
	double y = a.x10*b.x + a.x11*b.y + a.x12*b.z + a.x13;
	double z = a.x20*b.x + a.x21*b.y + a.x22*b.z + a.x23;
	return (vec3){x, y, z};
}

static double determinant(struct matrix44 a) {
	return
		a.x00*a.x11*a.x22*a.x33 - a.x00*a.x11*a.x23*a.x32 +
		a.x00*a.x12*a.x23*a.x31 - a.x00*a.x12*a.x21*a.x33 +
		a.x00*a.x13*a.x21*a.x32 - a.x00*a.x13*a.x22*a.x31 -
		a.x01*a.x12*a.x23*a.x30 + a.x01*a.x12*a.x20*a.x33 -
		a.x01*a.x13*a.x20*a.x32 + a.x01*a.x13*a.x22*a.x30 -
		a.x01*a.x10*a.x22*a.x33 + a.x01*a.x10*a.x23*a.x32 +
		a.x02*a.x13*a.x20*a.x31 - a.x02*a.x13*a.x21*a.x30 +
		a.x02*a.x10*a.x21*a.x33 - a.x02*a.x10*a.x23*a.x31 +
		a.x02*a.x11*a.x23*a.x30 - a.x02*a.x11*a.x20*a.x33 -
		a.x03*a.x10*a.x21*a.x32 + a.x03*a.x10*a.x22*a.x31 -
		a.x03*a.x11*a.x22*a.x30 + a.x03*a.x11*a.x20*a.x32 -
		a.x03*a.x12*a.x20*a.x31 + a.x03*a.x12*a.x21*a.x30
	;
}

static struct matrix44 inverse(struct matrix44 a) {
	struct matrix44 m;
	double d = determinant(a);
	m.x00 = (a.x12*a.x23*a.x31 - a.x13*a.x22*a.x31 + a.x13*a.x21*a.x32 - a.x11*a.x23*a.x32 - a.x12*a.x21*a.x33 + a.x11*a.x22*a.x33) / d;
	m.x01 = (a.x03*a.x22*a.x31 - a.x02*a.x23*a.x31 - a.x03*a.x21*a.x32 + a.x01*a.x23*a.x32 + a.x02*a.x21*a.x33 - a.x01*a.x22*a.x33) / d;
	m.x02 = (a.x02*a.x13*a.x31 - a.x03*a.x12*a.x31 + a.x03*a.x11*a.x32 - a.x01*a.x13*a.x32 - a.x02*a.x11*a.x33 + a.x01*a.x12*a.x33) / d;
	m.x03 = (a.x03*a.x12*a.x21 - a.x02*a.x13*a.x21 - a.x03*a.x11*a.x22 + a.x01*a.x13*a.x22 + a.x02*a.x11*a.x23 - a.x01*a.x12*a.x23) / d;
	m.x10 = (a.x13*a.x22*a.x30 - a.x12*a.x23*a.x30 - a.x13*a.x20*a.x32 + a.x10*a.x23*a.x32 + a.x12*a.x20*a.x33 - a.x10*a.x22*a.x33) / d;
	m.x11 = (a.x02*a.x23*a.x30 - a.x03*a.x22*a.x30 + a.x03*a.x20*a.x32 - a.x00*a.x23*a.x32 - a.x02*a.x20*a.x33 + a.x00*a.x22*a.x33) / d;
	m.x12 = (a.x03*a.x12*a.x30 - a.x02*a.x13*a.x30 - a.x03*a.x10*a.x32 + a.x00*a.x13*a.x32 + a.x02*a.x10*a.x33 - a.x00*a.x12*a.x33) / d;
	m.x13 = (a.x02*a.x13*a.x20 - a.x03*a.x12*a.x20 + a.x03*a.x10*a.x22 - a.x00*a.x13*a.x22 - a.x02*a.x10*a.x23 + a.x00*a.x12*a.x23) / d;
	m.x20 = (a.x11*a.x23*a.x30 - a.x13*a.x21*a.x30 + a.x13*a.x20*a.x31 - a.x10*a.x23*a.x31 - a.x11*a.x20*a.x33 + a.x10*a.x21*a.x33) / d;
	m.x21 = (a.x03*a.x21*a.x30 - a.x01*a.x23*a.x30 - a.x03*a.x20*a.x31 + a.x00*a.x23*a.x31 + a.x01*a.x20*a.x33 - a.x00*a.x21*a.x33) / d;
	m.x22 = (a.x01*a.x13*a.x30 - a.x03*a.x11*a.x30 + a.x03*a.x10*a.x31 - a.x00*a.x13*a.x31 - a.x01*a.x10*a.x33 + a.x00*a.x11*a.x33) / d;
	m.x23 = (a.x03*a.x11*a.x20 - a.x01*a.x13*a.x20 - a.x03*a.x10*a.x21 + a.x00*a.x13*a.x21 + a.x01*a.x10*a.x23 - a.x00*a.x11*a.x23) / d;
	m.x30 = (a.x12*a.x21*a.x30 - a.x11*a.x22*a.x30 - a.x12*a.x20*a.x31 + a.x10*a.x22*a.x31 + a.x11*a.x20*a.x32 - a.x10*a.x21*a.x32) / d;
	m.x31 = (a.x01*a.x22*a.x30 - a.x02*a.x21*a.x30 + a.x02*a.x20*a.x31 - a.x00*a.x22*a.x31 - a.x01*a.x20*a.x32 + a.x00*a.x21*a.x32) / d;
	m.x32 = (a.x02*a.x11*a.x30 - a.x01*a.x12*a.x30 - a.x02*a.x10*a.x31 + a.x00*a.x12*a.x31 + a.x01*a.x10*a.x32 - a.x00*a.x11*a.x32) / d;
	m.x33 = (a.x01*a.x12*a.x20 - a.x02*a.x11*a.x20 + a.x02*a.x10*a.x21 - a.x00*a.x12*a.x21 - a.x01*a.x10*a.x22 + a.x00*a.x11*a.x22) / d;
	return m;
}

struct matrix44 translation(vec3 v) {
	return (struct matrix44){
		1, 0, 0, v.x,
		0, 1, 0, v.y,
		0, 0, 1, v.z,
		0, 0, 0, 1,
	};
}

struct matrix44 rotation(vec3 v, double a) {
	a *= M_PI / 180.0;
	v = vec3Unit(v);
	double s = sin(a);
	double c = cos(a);
	double m = 1 - c;
	return (struct matrix44){
		m*v.x*v.x + c,     m*v.x*v.y + v.z*s, m*v.z*v.x - v.y*s, 0,
		m*v.x*v.y - v.z*s, m*v.y*v.y + c,     m*v.y*v.z + v.x*s, 0,
		m*v.z*v.x + v.y*s, m*v.y*v.z - v.x*s, m*v.z*v.z + c,     0,
		0,                 0,                 0,                 1,
	};
}

struct state {
	SDF3 sdf;
	struct matrix44 im;
};

static double transformEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	return SDF3Evaluate(s->sdf, matrixMul(s->im, pos));
}

static sphere_t transformBounds(void *p, struct matrix44 m) {
	struct state *s = p;
	return (sphere_t){matrixMul(m, s->sdf.bounds.center), s->sdf.bounds.radius};
}

SDF3 translate(vec3 v, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->sdf = sdf;
	struct matrix44 m = translation(v);
	s->im = inverse(m);
	return (SDF3){transformEvaluate, transformBounds(s, m), s};
}

SDF3 translateX(double t, SDF3 sdf) {
	return translate((vec3){t,0,0}, sdf);
}

SDF3 translateY(double t, SDF3 sdf) {
	return translate((vec3){0,t,0}, sdf);
}

SDF3 translateZ(double t, SDF3 sdf) {
	return translate((vec3){0,0,t}, sdf);
}

SDF3 rotate(vec3 v, double deg, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->sdf = sdf;
	struct matrix44 m = rotation(v, deg);
	s->im = inverse(m);
	return (SDF3){transformEvaluate, transformBounds(s, m), s};
}

SDF3 rotateX(double deg, SDF3 sdf) {
	return rotate((vec3){1,0,0}, deg, sdf);
}

SDF3 rotateY(double deg, SDF3 sdf) {
	return rotate((vec3){0,1,0}, deg, sdf);
}

SDF3 rotateZ(double deg, SDF3 sdf) {
	return rotate((vec3){0,0,1}, deg, sdf);
}

