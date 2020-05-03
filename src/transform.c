
#include "common.h"

struct matrix44 {
	double x00, x01, x02, x03;
	double x10, x11, x12, x13;
	double x20, x21, x22, x23;
	double x30, x31, x32, x33;
};

vec3 matrixMul(struct matrix44 a, vec3 b) {
	double x = a.x00*b.x + a.x01*b.y + a.x02*b.z + a.x03;
	double y = a.x10*b.x + a.x11*b.y + a.x12*b.z + a.x13;
	double z = a.x20*b.x + a.x21*b.y + a.x22*b.z + a.x23;
	return (vec3){x, y, z};
}

double determinant(struct matrix44 a) {
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

struct matrix44 inverse(struct matrix44 a) {
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
	struct matrix44 M;
	struct matrix44 I;
};

static double transformEvaluate(void *p, vec3 pos) {
	struct state *s = p;
	struct matrix44 m = s->I;
	return s->sdf.evaluate(s->sdf.context, matrixMul(m, pos));
}

static sphere_t transformBounds(void *p) {
	struct state *s = p;
	struct matrix44 m = s->M;
	return (sphere_t){matrixMul(m, s->sdf.bounds.center), s->sdf.bounds.radius};
}

SDF3 Translate(vec3 v, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->sdf = sdf;
	s->M = translation(v);
	s->I = inverse(s->M);
	return (SDF3){transformEvaluate, transformBounds(s), s};
}

SDF3 TranslateX(double t, SDF3 sdf) {
	return Translate((vec3){t,0,0}, sdf);
}

SDF3 TranslateY(double t, SDF3 sdf) {
	return Translate((vec3){0,t,0}, sdf);
}

SDF3 TranslateZ(double t, SDF3 sdf) {
	return Translate((vec3){0,0,t}, sdf);
}

SDF3 Rotate(vec3 v, double deg, SDF3 sdf) {
	struct state *s = allot(sizeof(struct state));
	s->sdf = sdf;
	s->M = rotation(v, deg);
	s->I = inverse(s->M);
	return (SDF3){transformEvaluate, transformBounds(s), s};
}

SDF3 RotateX(double deg, SDF3 sdf) {
	return Rotate((vec3){1,0,0}, deg, sdf);
}

SDF3 RotateY(double deg, SDF3 sdf) {
	return Rotate((vec3){0,1,0}, deg, sdf);
}

SDF3 RotateZ(double deg, SDF3 sdf) {
	return Rotate((vec3){0,0,1}, deg, sdf);
}

