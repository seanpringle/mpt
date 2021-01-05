#define _GNU_SOURCE
#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <stdatomic.h>
#include <math.h>
#include <unistd.h>

#include <cairo.h>

#define MAX(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b;       \
})

#define MIN(a,b)             \
({                           \
    __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b;       \
})

#define notef(...) { fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); }
#define fatalf(...) { notef(__VA_ARGS__); exit(EXIT_FAILURE); }
#define assertf(cond,...) assert((cond) || ({ notef(__VA_ARGS__); false; }))
#define ensuref(cond,...) if (!(cond)) { notef(__VA_ARGS__); exit(EXIT_FAILURE); }

void* allot(size_t);
void reset();

#include "vec2.h"
#include "vec3.h"
#include "color.h"

extern vec2 Zero2;
extern vec3 Zero3;

typedef struct {
	vec3 origin;
	vec3 direction;
	struct random_data *rnd;
} ray_t;

typedef struct {
	vec3 origin;
	vec3 target;
	vec3 u;
	vec3 v;
	vec3 w;
	double m;
	double focus;
	double aperture;
	double scaleU;
	double scaleV;
	ray_t (*emit)(double imageX, double imageY, int imageW, int imageH, struct random_data *rnd);
} camera_t;

typedef struct {
	Color color;
	int rays;
	double alpha; // candidate for invisible shadows-only surface
} pixel_t;

typedef struct {
	vec2 center;
	double radius;
} circle_t;

typedef struct {
	vec3 center;
	double radius;
} sphere_t;

#include "sdf2.h"
#include "sdf3.h"

struct _material_t;
struct _object_t;

typedef struct _material_t {
	bool light;
	bool lightDirect;
	bool invisible;
	bool diffuse;
	bool specular;
	bool dielectric;
	Color color;
	double roughness;
	double refractiveIndex;
} material_t;

typedef struct _object_t {
	SDF3 sdf;
	struct _material_t material;
	struct _object_t *next;
} object_t;

typedef struct {
	int count;
	object_t **objects;
} group_t;

typedef struct {
	int seed;
	int width; // in pixels
	int height; // in pixels
	int passes; // number of render passes
	int bounces; // max shadow ray bounces
	double horizon; // max scene distance from 0,0,0 to limit marching rays
	double threshold; // distance from SDF considered close enough to be a hit
	Color ambient; // color when rays stop before reaching a light
	double shadowH; // shadow alpha upper limit on invisible surfaces (dark center)
	double shadowL; // shadow alpha lower limit on invisible surfaces (prenumbra cut-off)
	double shadowD; // shadow darkness (light brightness multipler)
	double shadowR; // shadow sharpness (light radius multipler)
	bool useAlphaMap;
	bool *alphaMap;
} scene_t;

struct matrix44 {
	double x00, x01, x02, x03;
	double x10, x11, x12, x13;
	double x20, x21, x22, x23;
	double x30, x31, x32, x33;
};

extern scene_t scene;
extern camera_t camera;
extern object_t *objects;
extern int objectCount;

float randomNormalized(struct random_data *rnd);

vec3 pickVec3(struct random_data *rnd);

bool bounce(ray_t ray, object_t *object, vec3 hit, ray_t *bounced);

material_t matt(Color c);

material_t light(Color c);

material_t metal(Color c, double roughness);

material_t glass(Color c, double refractiveIndex);

material_t shadows();

void object(material_t, SDF3);

uint32_t* output(pixel_t *raster);

void render(pixel_t *raster, int workers);

void trace(ray_t ray, int depth, object_t *bypass, Color *rcolor, int *rbounces, double *ralpha);

bool march(ray_t ray, object_t *bypass, object_t **thing, vec3 *hit);

void perspective(vec3 lookFrom, vec3 lookAt, vec3 vup, double vfov, vec3 focus, double aperture);

void orthographic(vec3 lookFrom, vec3 lookAt, vec3 vup, double scaleU, double scaleV);

ray_t emit(double imageX, double imageY, int imageW, int imageH, struct random_data *rnd);

group_t neighbours(vec3 pos);

void prepare();

void destroy();

void script(int argc, char **argv);

struct matrix44 translation(vec3 v);

struct matrix44 rotation(vec3 v, double a);

vec3 matrixMul(struct matrix44 a, vec3 b);
