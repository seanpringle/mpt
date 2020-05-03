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
} Ray;

typedef struct {
	vec3 origin;
	vec3 u;
	vec3 v;
	vec3 w;
	double m;
	double focus;
	double aperture;
} Camera;

typedef struct {
	Color color;
	int rays;
	double alpha; // candidate for invisible shadows-only surface
} Pixel;

typedef struct {
	vec2 center;
	double radius;
} Bounds2;

typedef struct {
	vec3 center;
	double radius;
} Bounds3;

#include "sdf2.h"
#include "sdf3.h"

struct _Material;
struct _Object;

typedef bool (*materialLight)(void*, Color*);
typedef bool (*materialScatter)(void*, Ray, struct _Object*, vec3, int, Ray*, Color*);

typedef struct _Material {
	bool invisible;
	materialLight light;
	materialScatter scatter;
	void *context;
} Material;

typedef struct _Object {
	SDF3 sdf;
	struct _Material material;
	struct _Object *next;
} Object;

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
} Scene;

extern Scene scene;
extern Camera camera;
extern Object *objects;
extern int objectCount;
extern Pixel *raster;

#include "material.h"

float randomNormalized(struct random_data *rnd);

void place(Material, SDF3);

uint32_t* output();

void render(int workers);

void trace(Ray ray, int depth, Object *bypass, Color *rcolor, int *rbounces, double *ralpha);

bool march(Ray ray, Object *bypass, Object **thing, vec3 *hit);

void perspective(vec3 lookFrom, vec3 lookAt, vec3 vup, double vfov, vec3 focus, double aperture);

Ray emit(int imageX, int imageY, int imageW, int imageH, double jitterU, double jitterV, struct random_data *rnd);
