
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

scene_t scene;
camera_t camera;
pixel_t *raster;
object_t *objects;
int objectCount;

void reset() {
	used = 0;

	memset(&scene, 0, sizeof(scene_t));
	memset(&camera, 0, sizeof(camera_t));

	free(raster);
	raster = NULL;

	objects = NULL;
	objectCount = 0;
}

void things(vec3 relative, material_t material) {

	object(material,
		translate(add(relative, v3(0,0,500)), sphere(1000))
	);

	object(material,
		translate(add(relative, v3(0,2000,500)), cube(1000, 1000, 1000))
	);

	object(material,
		translate(add(relative, v3(-1500,0,500)), cylinder(1000, 1000))
	);

	object(material,
		translate(add(relative, v3(1500,0,600)), rotateX(90, torus(1000, 200)))
	);

	object(material,
		translate(add(relative, v3(-1500,2000,500)), pyramid(1000, 1000))
	);

	object(material,
		translate(add(relative, v3(1500,2000,500)), cone(1000, 1000))
	);

	for (int i = 3; i <= 8; i++) {
		object(material,
			translate(add(relative, v3((double)(i-3)*600-1450, -1250, 250)), extrude(500, polygon(i, 500)))
		);
	}
}

void test() {
	reset();

	scene.seed = 123456789;
	scene.width = 1920;
	scene.height = 1080;
	scene.passes = 0;
	scene.samples = 10;
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

	material_t steel = metal(rgb(0.4, 0.4, 0.4), 0.95);
	material_t stainless = metal(rgb(0.4, 0.4, 0.4), 0.3);
	material_t gold = metal(rgb(0.93, 0.78, 0.31), 0.0);
	material_t copper = metal(rgb(0.68, 0.45, 0.41), 0.8);
	material_t brass = metal(rgb(0.80, 0.58, 0.45), 0.9);

	object(
		matt(rgb(0.16, 0.12, 0.09)),
		translate(v3(0, 0, -5), cube(25000, 25000, 10))
	);

	object(
		light(shine(White, 4)),
		translate(v3(-7500, 0, 20000), sphere(20000))
	);

	object(brass,
		translate(v3(-5500, -1200, 500),
			subtract(4,
				intersect(2,
					sphere(1000),
					cube(800, 800, 800)
				),
				cylinder(1002, 400),
				rotateX(90, cylinder(1002, 400)),
				rotateY(90, cylinder(1002, 400))
			)
		)
	);

	object(brass,
		translate(v3(5500, -1200, 500),
			combine(4,
				intersect(2,
					sphere(1000),
					cube(800, 800, 800)
				),
				cylinder(1250, 400),
				rotateX(90, cylinder(1250, 400)),
				rotateY(90, cylinder(1250, 400))
			)
		)
	);

	object(brass,
		translate(v3(6500, 3750, 500),
			rounded(100, cube(800, 800, 800))
		)
	);

	object(brass,
		translate(v3(-6500, 3750, 500),
			rounded(100, cylinder(800, 1000))
		)
	);

	object(steel,
		translate(v3(6000, 900, 600),
			repeat(1, 1, 1, 400, 400, 400, sphere(400))
		)
	);

	object(copper,
		translate(v3(-6000, 900, 500),
			capsule(750, 1000, 500)
		)
	);

	SDF3 bowlShape = revolve(0, parabola(2000, 1500));

	object(glass(rgb(1.0, 0.5, 0.5), 1.5),
		combine(2,
			translateZ(100, cylinderR(200, 1000, 10)),
			subtract(2, bowlShape, translateZ(200, bowlShape))
		)
	);

	for (int i = 0; i < 8; i++) {
		object(glass(White, 1.5),
			translateY(-100, rotateZ(i*45, translate(v3(700, 0, 100), sphere(200))))
		);
	}

	things(v3(-2750, -2750, 0), steel);
	things(v3(2750, -2750, 0), copper);
	things(v3(-3000, 2250, 0), stainless);
	things(v3(3000, 2250, 0), gold);

	for (int i = 0; i < 9; i++) {
		if (i >= 3 && i < 6) continue;
		Color color = (i%2 == 0) ? rgb(0.5, 1.0, 0.5): rgb(0.5, 0.5, 1.0);
		object(glass(color, 1.5),
			translate(v3(0, (double)(i)*1000-4000, 250), sphere(500))
		);
	}

	srand(scene.seed);
	raster = calloc(scene.width * scene.height, sizeof(pixel_t));

	int workers = get_nprocs();
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