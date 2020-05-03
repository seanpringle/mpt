
#include "common.h"
#include "glsl.h"
#include <sys/sysinfo.h>

float randomNormalized(struct random_data *rnd) {
	int r = 0;
	assert(0 == random_r(rnd, &r));
	return (float)(r%1000) / 1000.0;
}

bool strEq(const char *a, const char *b) {
	return strcmp(a, b) == 0;
}

int64_t parseInt(const char *s) {
	return strtoll(s, NULL, 10);
}

double parseDouble(const char *s) {
	return strtod(s, NULL);
}

size_t used = 0;
uint8_t buffer[1024*1024];

void* allot(size_t n) {
	if (n%8 != 0) {
		n += (8-(n%8));
	}
	assert(used + n <= sizeof(buffer));
	void *ptr = &buffer[used];
	memset(ptr, 0, n);
	used += n;
	return ptr;
}

Scene scene;
Camera camera;
Pixel *raster;
Object *objects;
int objectCount;

void reset() {
	used = 0;

	memset(&scene, 0, sizeof(Scene));
	memset(&camera, 0, sizeof(Camera));

	free(raster);
	raster = NULL;

	objects = NULL;
	objectCount = 0;
}

void things(vec3 relative, Material material) {

	place(material,
		Translate(add(relative, v3(0,0,500)), Sphere(1000))
	);

	place(material,
		Translate(add(relative, v3(0,2000,500)), Cube(1000, 1000, 1000))
	);

	place(material,
		Translate(add(relative, v3(-1500,0,500)), Cylinder(1000, 1000))
	);

	place(material,
		Translate(add(relative, v3(1500,0,500)), Torus(1000, 200))
	);

	place(material,
		Translate(add(relative, v3(-1500,2000,500)), Pyramid(1000, 1000))
	);

	place(material,
		Translate(add(relative, v3(1500,2000,500)), Cone(1000, 1000))
	);

	for (int i = 3; i <= 8; i++) {
		place(material,
			Translate(add(relative, v3((double)(i-3)*600-1450, -1250, 250)), Extrude(500, Polygon(i, 250)))
		);
	}
}

void test() {
	reset();

	scene.seed = 123456789;
	scene.width = 1280;
	scene.height = 720;
	scene.passes = 1;
	scene.samples = 1;
	scene.bounces = 16;
	scene.horizon = 100000;
	scene.threshold = 0.0001;
	scene.ambient = colorScale(White, 0.05);

	perspective(
		v3(0, -8000, 8000),
		v3(0, -1000, 500),
		v3(0, 0, 1),
		40,
		Zero3,
		0.0
	);

	Material steel = Metal((Color){0.4, 0.4, 0.4}, 0.95);
	Material stainless = Metal((Color){0.4, 0.4, 0.4}, 0.3);
	Material gold = Metal((Color){0.93, 0.78, 0.31}, 0.0);
	Material copper = Metal((Color){0.68, 0.45, 0.41}, 0.8);
	Material brass = Metal((Color){0.80, 0.58, 0.45}, 0.9);

	place(
		Matt((Color){0.16, 0.12, 0.09}),
		Translate((vec3){0, 0, -5}, Cube(25000, 25000, 10))
	);

	place(
		Light(colorScale(White, 4)),
		Translate((vec3){-7500, 0, 20000}, Sphere(20000))
	);

	place(brass,
		Translate(v3(-5500, -1200, 500),
			Difference(4,
				Intersection(2,
					Sphere(1000),
					Cube(800, 800, 800)
				),
				Cylinder(1002, 400),
				RotateX(90, Cylinder(1002, 400)),
				RotateY(90, Cylinder(1002, 400))
			)
		)
	);

	place(brass,
		Translate(v3(5500, -1200, 500),
			Union(4,
				Intersection(2,
					Sphere(1000),
					Cube(800, 800, 800)
				),
				Cylinder(1250, 400),
				RotateX(90, Cylinder(1250, 400)),
				RotateY(90, Cylinder(1250, 400))
			)
		)
	);

	place(brass,
		Translate(v3(6500, 3750, 500),
			Round(100, Cube(800, 800, 800))
		)
	);

	place(brass,
		Translate(v3(-6500, 3750, 500),
			Round(100, Cylinder(800, 1000))
		)
	);

	place(steel,
		Translate(v3(6000, 900, 600),
			Repeat(1, 1, 1, 400, 400, 400, Sphere(400))
		)
	);

	place(copper,
		Translate(v3(-6000, 900, 500),
			Capsule(1000, 1000, 500)
		)
	);

	SDF3 bowlShape = Revolve(0, Parabola(2000, 2000));

	place(Glass((Color){1.0, 0.5, 0.5}, 1.5),
		Union(2,
			TranslateZ(100, CylinderR(200, 1000, 10)),
			Difference(2, bowlShape, TranslateZ(200, bowlShape))
		)
	);

	things(v3(-2750, -2750, 0), steel);
	things(v3(2750, -2750, 0), copper);
	things(v3(-3000, 2250, 0), stainless);
	things(v3(3000, 2250, 0), gold);

	for (int i = 0; i < 9; i++) {
		if (i >= 3 && i < 6) continue;
		Color color = (i%2 == 0) ? (Color){0.5, 1.0, 0.5}: (Color){0.5, 0.5, 1.0};
		place(Glass(color, 1.5),
			Translate(v3(0, (double)(i)*1000-3500, 250), Sphere(500))
		);
	}

	srand(scene.seed);
	raster = calloc(scene.width * scene.height, sizeof(Pixel));

	int workers = 1;//get_nprocs();
	int batches = ceil((float)scene.passes/(float)workers);

	fprintf(stderr, "%d passes, %d workers, %d batches, %d samples\n", scene.passes, workers, batches, scene.samples);

	for (int batch = 0; scene.passes == 0 || batch < batches; batch++) {
		fprintf(stderr, "batch %d of %d\n", batch+1, batches);
		render(workers);

		uint32_t *frame = output();

		cairo_surface_t *surface = cairo_image_surface_create_for_data(
			(unsigned char*)frame,
			CAIRO_FORMAT_ARGB32,
			scene.width,
			scene.height,
			scene.width*sizeof(uint32_t)
		);

		assert(CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(surface, "out.png"));
		cairo_surface_destroy(surface);

		free(frame);
	}

	reset();
}

int main(int argc, char **argv) {
	bool doTest = false;

	for (off_t i = 1; i < argc; i++) {
		if (strEq(argv[i], "--help")) {
			fprintf(stdout,
				"--test : test scene\n"
			);
			return EXIT_SUCCESS;
		}
		if (strEq(argv[i], "--test")) {
			doTest = true;
			continue;
		}
		fatalf("unexpected argument: %s", argv[i]);
	}

	if (doTest) {
		test();
	}
}