
#include "common.h"
#include <threads.h>

float randomNormalized(struct random_data *rnd) {
	int r = 0;
	assert(0 == random_r(rnd, &r));
	return (float)(r%1000) / 1000.0;
}

void prepare() {
	srand(scene.seed);
}

void object(material_t mat, SDF3 sdf) {
	object_t *o = allot(sizeof(object_t));
	o->sdf = sdf;
	o->material = mat;
	o->next = objects;
	objects = o;
	objectCount++;
}

static void merge(pixel_t *dst, pixel_t *src) {
	for (int y = 0; y < scene.height; y++) {
		for (int x = 0; x < scene.width; x++) {
			pixel_t *dpixel = &dst[y*scene.width+x];
			pixel_t *spixel = &src[y*scene.width+x];
			dpixel->color = colorAdd(dpixel->color, spixel->color);
			dpixel->rays += spixel->rays;
			dpixel->alpha += spixel->alpha;
		}
	}
}

struct workerJob {
	_Atomic bool done;
	_Atomic float progress;
	pixel_t *raster;
	int seed;
};

static int workerRun(void *context) {
	struct workerJob *job = context;

	char state[64];
	struct random_data rnd;
	memset(state, 0, sizeof(state));
	memset(&rnd, 0, sizeof(rnd));
	assert(0 == initstate_r(job->seed, state, sizeof(state), &rnd));

	const int grid = 3;
	const double cell = 1.0 / (double)grid;

	for (int y = 0; y < scene.height; y++) {
		job->progress = (float)y / (float)scene.height;
		for (int x = 0; x < scene.width; x++) {
			for (int cy = 0; cy < grid; cy++) {
				for (int cx = 0; cx < grid; cx++) {
					double u = (double)x + (((double)cx * cell) + (cell/2.0)) + ((randomNormalized(&rnd)-0.5) * cell);
					double v = (double)y + (((double)cy * cell) + (cell/2.0)) + ((randomNormalized(&rnd)-0.5) * cell);
					ray_t ray = emit(u, v, scene.width, scene.height, &rnd);
					Color color; int bounces; double alpha;
					trace(ray, 0, NULL, &color, &bounces, &alpha);
					pixel_t *pixel = &job->raster[y*scene.width+x];
					pixel->color = colorAdd(pixel->color, color);
					pixel->alpha += alpha;
					pixel->rays++;
				}
			}
		}
	}
	job->done = true;
	return 0;
}

void render(pixel_t *raster, int workers) {
	int seed = random();

	thrd_t threads[workers];
	struct workerJob jobs[workers];

	for (int i = 0; i < workers; i++) {
		jobs[i] = (struct workerJob){
			.seed = seed + i,
			.raster = calloc(scene.width * scene.height, sizeof(pixel_t)),
		};
		thrd_create(&threads[i], workerRun, &jobs[i]);
	}

 	if (ttyname(STDOUT_FILENO)) {
		for (bool done = false; !done; ) {
			done = true;
			float progress = 0;
			for (int i = 0; i < workers; i++) {
				progress += jobs[i].progress;
				done = done && jobs[i].done;
			}
			progress /= (double)workers;
			progress = done ? 1: progress;

			int bar = 50;
			int fill = floor(progress*(float)bar);
			fprintf(stdout, "\r");
			for (int i = 0; i < fill; i++) {
				fprintf(stdout, "*");
			}
			for (int i = fill; i < bar; i++) {
				fprintf(stdout, ".");
			}
			fflush(stdout);

			thrd_sleep(&(struct timespec){.tv_sec=1}, NULL);
		}
		fprintf(stdout, "\n");
	}

	for (int i = 0; i < workers; i++) {
		thrd_join(threads[i], NULL);
		merge(raster, jobs[i].raster);
		free(jobs[i].raster);
	}
}

static NRGBA nrgba(pixel_t *raster, int x, int y) {
	pixel_t pixel = raster[y*scene.width+x];

	// average
	Color c = colorScale(pixel.color, 1.0/(double)pixel.rays);

	// gamma correction
	c = (Color){sqrt(c.r), sqrt(c.g), sqrt(c.b)};

	NRGBA nrgba = {0,0,0,0};

	if (c.r > 0 || c.g > 0 || c.b > 0) {
		double alpha = pixel.alpha / (double)pixel.rays;

		if (scene.shadowL > 0 && alpha < scene.shadowL) {
			alpha = 0.0;
		}

		if (scene.shadowH > 0 && alpha > scene.shadowH) {
			alpha = 1.0;
		}

		uint8_t r = MAX(0, MIN((double)(0xff), c.r*(double)(0xff)));
		uint8_t g = MAX(0, MIN((double)(0xff), c.g*(double)(0xff)));
		uint8_t b = MAX(0, MIN((double)(0xff), c.b*(double)(0xff)));
		uint8_t a = MAX(0, MIN((double)(0xff), alpha*(double)(0xff)));
		nrgba = (NRGBA){r, g, b, a};
	}

	return nrgba;
}

uint32_t* output(pixel_t *raster) {
	uint32_t *frame = calloc(scene.width * scene.height, sizeof(uint32_t));

	for (int y = 0; y < scene.height; y++) {
		for (int x = 0; x < scene.width; x++) {
			frame[y*scene.width+x] = NRGBAtoARGB32(nrgba(raster, x, y));
		}
	}

	return frame;
}