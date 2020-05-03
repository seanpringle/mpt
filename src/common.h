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
#define assertf(cond,...) assertf((cond) || ({ notef(__VA_ARGS__); false; }))
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
	vec3 u;
	vec3 v;
	vec3 w;
	double m;
	double focus;
	double aperture;
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

typedef bool (*materiallight)(void*, Color*);
typedef bool (*materialScatter)(void*, ray_t, struct _object_t*, vec3, int, ray_t*, Color*);

typedef struct _material_t {
	bool invisible;
	materiallight light;
	materialScatter scatter;
	void *context;
} material_t;

typedef struct _object_t {
	SDF3 sdf;
	struct _material_t material;
	struct _object_t *next;
} object_t;

typedef struct {
	int64_t seed;
	int width; // in pixels
	int height; // in pixels
	int passes; // number of render passes
	int samples; // number of jittered samples per pixel
	int bounces; // max shadow ray bounces
	double horizon; // max scene distance from 0,0,0 to limit marching rays
	double threshold; // distance from SDF considered close enough to be a hit
	Color ambient; // color when rays stop before reaching a light
	double shadowH; // shadow alpha upper limit on invisible surfaces (dark center)
	double shadowL; // shadow alpha lower limit on invisible surfaces (prenumbra cut-off)
	double shadowD; // shadow darkness (light brightness multipler)
	double shadowR; // shadow sharpness (light radius multipler)
} scene_t;

extern scene_t scene;
extern camera_t camera;
extern object_t *objects;
extern int objectCount;
extern pixel_t *raster;

float randomNormalized(struct random_data *rnd);

vec3 pickVec3(struct random_data *rnd);

material_t matt(Color c);

material_t light(Color c);

material_t metal(Color c, double roughness);

material_t glass(Color c, double refractiveIndex);

material_t shadows();

void object(material_t, SDF3);

uint32_t* output();

void render(int workers);

void trace(ray_t ray, int depth, object_t *bypass, Color *rcolor, int *rbounces, double *ralpha);

bool march(ray_t ray, object_t *bypass, object_t **thing, vec3 *hit);

void perspective(vec3 lookFrom, vec3 lookAt, vec3 vup, double vfov, vec3 focus, double aperture);

ray_t emit(int imageX, int imageY, int imageW, int imageH, double jitterU, double jitterV, struct random_data *rnd);
