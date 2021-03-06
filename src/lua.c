
#include "common.h"
#include "mesh.h"
#include "lua-lib.h"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <sys/sysinfo.h>

size_t used = 0;
size_t mark = 0;
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
object_t *objects;
int objectCount;
int threads;

#define lua_ensuref(L,cond,...) ({ \
	if (!(cond)) { \
		char msg[1024]; \
		snprintf(msg, 1024, __VA_ARGS__); \
		lua_pushstring(L, msg); \
		lua_error(L); \
	} \
})

void require_args(lua_State *lua, const char *func, int args) {
  lua_ensuref(lua, lua_gettop(lua) >= args, "%s() expected %d arguments", func, args);
}

material_t pop_material(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, !lua_isnil(lua, -1), "expected material");
	material_t material = *((material_t*)lua_touserdata(lua, -1));
	lua_pop(lua, 1);
	return material;
}

void get_key_or_idx(lua_State *lua, const char *key, int idx) {
	lua_pushstring(lua, key);
	lua_gettable(lua, -2);
	if (lua_isnil(lua, -1)) {
		lua_pop(lua, 1);
		lua_pushnumber(lua, idx);
		lua_gettable(lua, -2);
	}
	lua_ensuref(lua, !lua_isnil(lua, -1), "expected table key %s or index %d", key, idx);
}

double pop_double(lua_State *lua) {
	require_args(lua, __func__, 1);
	double d = lua_tonumber(lua, -1);
	lua_pop(lua, 1);
	return d;
}

bool pop_bool(lua_State *lua) {
	require_args(lua, __func__, 1);
	bool b = lua_toboolean(lua, -1);
	lua_pop(lua, 1);
	return b;
}

Color pop_color(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, lua_istable(lua, -1), "color must be a table");
	Color color = {0,0,0};
		get_key_or_idx(lua, "r", 1);
		color.r = pop_double(lua);
		get_key_or_idx(lua, "g", 2);
		color.g = pop_double(lua);
		get_key_or_idx(lua, "b", 3);
		color.b = pop_double(lua);
	lua_pop(lua, 1);
	return color;
}

vec2 pop_vec2(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, lua_istable(lua, -1), "vec2 must be a table");
	vec2 v = {0,0};
		get_key_or_idx(lua, "x", 1);
		v.x = pop_double(lua);
		get_key_or_idx(lua, "y", 2);
		v.y = pop_double(lua);
	lua_pop(lua, 1);
	return v;
}

vec3 pop_vec3(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, lua_istable(lua, -1), "vec3 must be a table");
	vec3 v = {0,0,0};
		get_key_or_idx(lua, "x", 1);
		v.x = pop_double(lua);
		get_key_or_idx(lua, "y", 2);
		v.y = pop_double(lua);
		get_key_or_idx(lua, "z", 3);
		v.z = pop_double(lua);
	lua_pop(lua, 1);
	return v;
}

SDF2 pop_sdf2(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, lua_isuserdata(lua, -1), "SDF must be user data");
	SDF2 sdf = *((SDF2*)lua_touserdata(lua, -1));
	lua_pop(lua, 1);
	return sdf;
}

SDF3 pop_sdf3(lua_State *lua) {
	require_args(lua, __func__, 1);
	lua_ensuref(lua, lua_isuserdata(lua, -1), "SDF must be user data");
	SDF3 sdf = *((SDF3*)lua_touserdata(lua, -1));
	lua_pop(lua, 1);
	return sdf;
}

int wrap_scene(lua_State *lua) {
	scene.useAlphaMap = pop_bool(lua);

	scene.shadowR = pop_double(lua);
	scene.shadowD = pop_double(lua);
	scene.shadowL = pop_double(lua);
	scene.shadowH = pop_double(lua);

	scene.ambient = pop_color(lua);
	scene.threshold = pop_double(lua);
	scene.horizon = pop_double(lua);
	scene.seed = pop_double(lua);
	scene.bounces = pop_double(lua);
	scene.passes = pop_double(lua);
	scene.height = pop_double(lua);
	scene.width = pop_double(lua);

	used = mark;
	objectCount = 0;
	objects = NULL;
	return 0;
}

int wrap_perspective(lua_State *lua) {
	double aperture = pop_double(lua);
	vec3 focus = pop_vec3(lua);
	double fov = pop_double(lua);
	vec3 vup = pop_vec3(lua);
	vec3 lookAt = pop_vec3(lua);
	vec3 lookFrom = pop_vec3(lua);
	perspective(lookFrom, lookAt, vup, fov, focus, aperture);
	return 0;
}

int wrap_orthographic(lua_State *lua) {
	double scaleV = pop_double(lua);
	double scaleU = pop_double(lua);
	vec3 vup = pop_vec3(lua);
	vec3 lookAt = pop_vec3(lua);
	vec3 lookFrom = pop_vec3(lua);
	orthographic(lookFrom, lookAt, vup, scaleU, scaleV);
	return 0;
}

int wrap_object(lua_State *lua) {
	SDF3 sdf = pop_sdf3(lua);
	material_t material = pop_material(lua);
	object(material, sdf);
	return 0;
}

int wrap_mesh(lua_State *lua) {
	SDF3 sdf = pop_sdf3(lua);
	int steps = (int)pop_double(lua);
	const char *path = lua_tostring(lua, -1);

	mesh_t* mesh = meshMarch(sdf, get_nprocs(), steps, 0);
	meshWriteObjFile(mesh, path);
	meshFree(mesh);

	lua_pop(lua, 1);
	return 0;
}

int wrap_light(lua_State *lua) {
	Color color = pop_color(lua);
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = light(color);
	return 1;
}

int wrap_sun(lua_State *lua) {
	Color color = pop_color(lua);
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = light(color);
	material->lightDirect = true;
	return 1;
}

int wrap_matt(lua_State *lua) {
	Color color = pop_color(lua);
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = matt(color);
	return 1;
}

int wrap_metal(lua_State *lua) {
	double roughness = pop_double(lua);
	Color color = pop_color(lua);
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = metal(color, roughness);
	return 1;
}

int wrap_glass(lua_State *lua) {
	double refractiveIndex = pop_double(lua);
	Color color = pop_color(lua);
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = glass(color, refractiveIndex);
	return 1;
}

int wrap_shadows(lua_State *lua) {
	material_t *material = lua_newuserdata(lua, sizeof(material_t));
	*material = shadows();
	return 1;
}

int wrap_circle(lua_State *lua) {
	double d = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = circle(d);
	return 1;
}

int wrap_rectangle(lua_State *lua) {
	double h = pop_double(lua);
	double w = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = rectangle(w, h);
	return 1;
}

int wrap_ellipse(lua_State *lua) {
	double h = pop_double(lua);
	double w = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = ellipse(w, h);
	return 1;
}

int wrap_triangle(lua_State *lua) {
	vec2 p3 = pop_vec2(lua);
	vec2 p2 = pop_vec2(lua);
	vec2 p1 = pop_vec2(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = triangle(p1, p2, p3);
	return 1;
}

int wrap_polygon(lua_State *lua) {
	double d = pop_double(lua);
	double i = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = polygon(i, d);
	return 1;
}

int wrap_stadium(lua_State *lua) {
	double d2 = pop_double(lua);
	double d1 = pop_double(lua);
	double h = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = stadium(h, d1, d2);
	return 1;
}

int wrap_parabola(lua_State *lua) {
	double h = pop_double(lua);
	double w = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = parabola(w, h);
	return 1;
}

int wrap_hexagram(lua_State *lua) {
	double d = pop_double(lua);
	SDF2 *sdf = lua_newuserdata(lua, sizeof(SDF2));
	*sdf = hexagram(d);
	return 1;
}

int wrap_rounded(lua_State *lua) {
	SDF3 inner = pop_sdf3(lua);
	double radius = pop_double(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = rounded(radius, inner);
	return 1;
}

int wrap_repeat(lua_State *lua) {
	SDF3 inner = pop_sdf3(lua);
	double a = pop_double(lua);
	double b = pop_double(lua);
	double c = pop_double(lua);
	double d = pop_double(lua);
	double e = pop_double(lua);
	double f = pop_double(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = repeat(f, e, d, c, b, a, inner);
	return 1;
}

int wrap_extend(lua_State *lua) {
	SDF3 inner = pop_sdf3(lua);
	double z = pop_double(lua);
	double y = pop_double(lua);
	double x = pop_double(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = extend(x, y, z, inner);
	return 1;
}

int wrap_translate(lua_State *lua) {
	SDF3 inner = pop_sdf3(lua);
	vec3 pos = pop_vec3(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = translate(pos, inner);
	return 1;
}

int wrap_rotate(lua_State *lua) {
	SDF3 inner = pop_sdf3(lua);
	double deg = pop_double(lua);
	vec3 axis = pop_vec3(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = rotate(axis, deg, inner);
	return 1;
}

int wrap_revolve(lua_State *lua) {
	SDF2 inner = pop_sdf2(lua);
	double offset = pop_double(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = revolve(offset, inner);
	return 1;
}

int wrap_extrude(lua_State *lua) {
	SDF2 inner = pop_sdf2(lua);
	double height = pop_double(lua);
	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = extrude(height, inner);
	return 1;
}

void make_csg(lua_State *lua, SDF3 (*csg)(int, SDF3*, double)) {
	int count = lua_objlen(lua, -1);
	SDF3 sdfs[count];
	for (int i = 1; i <= count; i++) {
		lua_pushnumber(lua, i);
		lua_gettable(lua, -2);
		lua_ensuref(lua, !lua_isnil(lua, -1), "CSG nil SDF");
		sdfs[i-1] = pop_sdf3(lua);
	}
	lua_pop(lua, 1);

	double k = lua_gettop(lua) > 0 ? pop_double(lua): 0.0;

	SDF3 *sdf = lua_newuserdata(lua, sizeof(SDF3));
	*sdf = csg(count, sdfs, k);
}

int wrap_union(lua_State *lua) {
	make_csg(lua, csgUnion);
	return 1;
}

int wrap_union_smooth(lua_State *lua) {
	make_csg(lua, csgSmoothUnion);
	return 1;
}

int wrap_union_chamfer(lua_State *lua) {
	make_csg(lua, csgChamferUnion);
	return 1;
}

int wrap_difference(lua_State *lua) {
	make_csg(lua, csgDifference);
	return 1;
}

int wrap_difference_smooth(lua_State *lua) {
	make_csg(lua, csgSmoothDifference);
	return 1;
}

int wrap_difference_chamfer(lua_State *lua) {
	make_csg(lua, csgChamferDifference);
	return 1;
}

int wrap_intersection(lua_State *lua) {
	make_csg(lua, csgIntersection);
	return 1;
}

int wrap_intersection_smooth(lua_State *lua) {
	make_csg(lua, csgSmoothIntersection);
	return 1;
}

int wrap_intersection_chamfer(lua_State *lua) {
	make_csg(lua, csgChamferIntersection);
	return 1;
}

int wrap_rotation(lua_State *lua) {
	vec3 pos = pop_vec3(lua);
	double deg = pop_double(lua);
	vec3 axis = pop_vec3(lua);
	vec3 res = matrixMul(rotation(axis, deg), pos);
	lua_pushnumber(lua, res.x);
	lua_pushnumber(lua, res.y);
	lua_pushnumber(lua, res.z);
	return 3;
}

struct frame_t {
	size_t width;
	size_t height;
	uint32_t data;
};

int frame_png(lua_State *lua) {
	struct frame_t *frame = lua_touserdata(lua, -1);
	const char *path = lua_tostring(lua, -2);

	cairo_surface_t *surface = cairo_image_surface_create_for_data(
		(unsigned char*)(&frame->data),
		CAIRO_FORMAT_ARGB32,
		frame->width,
		frame->height,
		frame->width*sizeof(uint32_t)
	);

	ensuref(CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(surface, path), "%s", path);
	cairo_surface_destroy(surface);
	lua_pop(lua, 2);
	return 0;
}

int frames_montage(lua_State *lua) {
	int frames = lua_objlen(lua, -1);
	int rows = lua_tonumber(lua, -2);
	int cols = lua_tonumber(lua, -3);

	lua_remove(lua, -2);
	lua_remove(lua, -2);

	lua_pushnumber(lua, 1);
	lua_gettable(lua, -2);
	lua_ensuref(lua, !lua_isnil(lua, -1), "montage expected frame?");

	struct frame_t *subframe = lua_touserdata(lua, -1);

	int subheight = subframe->width;
	int subwidth = subframe->height;

	lua_pop(lua, 1);

	int width = subwidth*cols;
	int height = subheight*rows;
	int pitch = width*sizeof(uint32_t);

	fprintf(stderr, "montage\n");
	fprintf(stderr, "frames       : %d\n", frames);
	fprintf(stderr, "frame width  : %d\n", subwidth);
	fprintf(stderr, "frame height : %d\n", subheight);
	fprintf(stderr, "sheet width  : %d\n", width);
	fprintf(stderr, "sheet height : %d\n", height);

	struct frame_t *frame = lua_newuserdata(lua, sizeof(struct frame_t) + (height*pitch));
	frame->width = width;
	frame->height = height;

	memset(&frame->data, 0, height*pitch);

	cairo_surface_t *target = cairo_image_surface_create_for_data(
		(unsigned char*)(&frame->data),
		CAIRO_FORMAT_ARGB32,
		frame->width,
		frame->height,
		frame->width*sizeof(uint32_t)
	);

	cairo_t *cr = cairo_create(target);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

	for (int i = 0; i < frames; i++) {
		lua_pushnumber(lua, i+1);
		lua_gettable(lua, -3);
		lua_ensuref(lua, !lua_isnil(lua, -1), "montage expected frame?");

		struct frame_t *subframe = lua_touserdata(lua, -1);

		cairo_surface_t *source = cairo_image_surface_create_for_data(
			(unsigned char*)(&subframe->data),
			CAIRO_FORMAT_ARGB32,
			subframe->width,
			subframe->height,
			subframe->width*sizeof(uint32_t)
		);

		int xd = i%cols*subwidth;
		int yd = i/cols*subheight;

		cairo_set_source_surface(cr, source, xd, yd);
		cairo_rectangle(cr, xd, yd, subwidth, subheight);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_fill(cr);

		cairo_surface_destroy(source);
		lua_pop(lua, 1);
	}

	lua_remove(lua, -2);

	cairo_surface_destroy(target);
	cairo_destroy(cr);

	return 1;
}

int wrap_render(lua_State *lua) {
	const char *path = lua_tostring(lua, -1);

	int batches = ceil((float)scene.passes/(float)threads);

	bool tty = ttyname(STDOUT_FILENO) != NULL;

 	if (tty) {
		fprintf(stdout, "seed      : %d\n", scene.seed);
		fprintf(stdout, "width     : %d\n", scene.width);
		fprintf(stdout, "height    : %d\n", scene.height);
		fprintf(stdout, "passes    : %d\n", scene.passes);
		fprintf(stdout, "bounces   : %d\n", scene.bounces);
		fprintf(stdout, "horizon   : %d\n", (int)floor(scene.horizon));
		fprintf(stdout, "threshold : %f\n", scene.threshold);
		fprintf(stdout, "objects   : %d\n", objectCount);
		fprintf(stdout, "alphaMap  : %s\n", scene.useAlphaMap ? "yes": "no");
		fprintf(stdout, "threads   : %d\n", threads);
	}

 	prepare();

	pixel_t *raster = calloc(scene.width * scene.height, sizeof(pixel_t));

	for (int batch = 0; scene.passes == 0 || batch < batches; batch++) {
 		if (tty) {
			fprintf(stdout, "batch %d of %d %s\n", batch+1, batches, path);
		}

		render(raster, threads);
		uint32_t *frame = output(raster);

		cairo_surface_t *surface = cairo_image_surface_create_for_data(
			(unsigned char*)frame,
			CAIRO_FORMAT_ARGB32,
			scene.width,
			scene.height,
			scene.width*sizeof(uint32_t)
		);

		ensuref(CAIRO_STATUS_SUCCESS == cairo_surface_write_to_png(surface, path), "%s", path);
		cairo_surface_destroy(surface);

		free(frame);
	}

	lua_pop(lua, 1);

	size_t bytes = scene.height * scene.width * sizeof(uint32_t);
	struct frame_t *frame = lua_newuserdata(lua, sizeof(struct frame_t) + bytes);
	frame->width = scene.width;
	frame->height = scene.height;

	memset(&frame->data, 0, bytes);

	uint32_t *export = output(raster);
	memmove(&frame->data, export, bytes);

	free(export);
	free(raster);

	destroy();
	return 1;
}

int traceback(lua_State *lua) {
	lua_getglobal(lua, "debug");
	lua_getfield(lua, -1, "traceback");
	lua_pushvalue(lua, 1);
	lua_pushinteger(lua, 2);
	lua_call(lua, 2, 1);
	fprintf(stderr, "%s\n", lua_tostring(lua, -1));
	return 1;
}

int main(int argc, char **argv) {

	threads = get_nprocs();

	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "--threads")) {
			ensuref(i < argc-2, "usage --threads N");
			threads = strtol(argv[i+1], NULL, 10);
			continue;
		}
	}

	lua_State *lua = luaL_newstate();
	luaL_openlibs(lua);

	lua_newtable(lua);
	for (int i = 0; i < argc; i++) {
		lua_pushnumber(lua, i+1);
		lua_pushstring(lua, argv[i]);
		lua_settable(lua, -3);
	}
	lua_setglobal(lua, "args");

	lua_pushcfunction(lua, wrap_scene);
	lua_setglobal(lua, "_scene");

	lua_pushcfunction(lua, wrap_perspective);
	lua_setglobal(lua, "_perspective");

	lua_pushcfunction(lua, wrap_orthographic);
	lua_setglobal(lua, "_orthographic");

	lua_pushcfunction(lua, wrap_rotation);
	lua_setglobal(lua, "_v3rotate");

	lua_pushcfunction(lua, wrap_render);
	lua_setglobal(lua, "render");

	lua_pushcfunction(lua, wrap_mesh);
	lua_setglobal(lua, "mesh");

	lua_pushcfunction(lua, frames_montage);
	lua_setglobal(lua, "montage");

	lua_pushcfunction(lua, frame_png);
	lua_setglobal(lua, "png");

	lua_pushcfunction(lua, wrap_object);
	lua_setglobal(lua, "object");

	lua_pushcfunction(lua, wrap_light);
	lua_setglobal(lua, "light");

	lua_pushcfunction(lua, wrap_sun);
	lua_setglobal(lua, "sun");

	lua_pushcfunction(lua, wrap_matt);
	lua_setglobal(lua, "matt");

	lua_pushcfunction(lua, wrap_metal);
	lua_setglobal(lua, "metal");

	lua_pushcfunction(lua, wrap_glass);
	lua_setglobal(lua, "glass");

	lua_pushcfunction(lua, wrap_shadows);
	lua_setglobal(lua, "shadows");

	lua_pushcfunction(lua, wrap_circle);
	lua_setglobal(lua, "circle");

	lua_pushcfunction(lua, wrap_rectangle);
	lua_setglobal(lua, "rectangle");

	lua_pushcfunction(lua, wrap_ellipse);
	lua_setglobal(lua, "ellipse");

	lua_pushcfunction(lua, wrap_triangle);
	lua_setglobal(lua, "triangle");

	lua_pushcfunction(lua, wrap_polygon);
	lua_setglobal(lua, "polygon");

	lua_pushcfunction(lua, wrap_stadium);
	lua_setglobal(lua, "stadium");

	lua_pushcfunction(lua, wrap_parabola);
	lua_setglobal(lua, "parabola");

	lua_pushcfunction(lua, wrap_hexagram);
	lua_setglobal(lua, "hexagram");

	lua_pushcfunction(lua, wrap_rounded);
	lua_setglobal(lua, "rounded");

	lua_pushcfunction(lua, wrap_union);
	lua_setglobal(lua, "csg_union");

	lua_pushcfunction(lua, wrap_union_smooth);
	lua_setglobal(lua, "csg_union_smooth");

	lua_pushcfunction(lua, wrap_union_chamfer);
	lua_setglobal(lua, "csg_union_chamfer");

	lua_pushcfunction(lua, wrap_difference);
	lua_setglobal(lua, "csg_difference");

	lua_pushcfunction(lua, wrap_difference_smooth);
	lua_setglobal(lua, "csg_difference_smooth");

	lua_pushcfunction(lua, wrap_difference_chamfer);
	lua_setglobal(lua, "csg_difference_chamfer");

	lua_pushcfunction(lua, wrap_intersection);
	lua_setglobal(lua, "csg_intersection");

	lua_pushcfunction(lua, wrap_intersection_smooth);
	lua_setglobal(lua, "csg_intersection_smooth");

	lua_pushcfunction(lua, wrap_intersection_chamfer);
	lua_setglobal(lua, "csg_intersection_chamfer");

	lua_pushcfunction(lua, wrap_translate);
	lua_setglobal(lua, "translate");

	lua_pushcfunction(lua, wrap_rotate);
	lua_setglobal(lua, "rotate");

	lua_pushcfunction(lua, wrap_revolve);
	lua_setglobal(lua, "revolve");

	lua_pushcfunction(lua, wrap_extrude);
	lua_setglobal(lua, "extrude");

	lua_pushcfunction(lua, wrap_repeat);
	lua_setglobal(lua, "reprise");

	lua_pushcfunction(lua, wrap_extend);
	lua_setglobal(lua, "extend");

	lua_pushcfunction(lua, traceback);

	char *lib = malloc(src_lib_lua_len+1);
	memmove(lib, src_lib_lua, src_lib_lua_len);
	lib[src_lib_lua_len] = 0;

	int rc = luaL_loadstring(lua, lib);
	if (rc == 0) {
		ensuref(0 == lua_pcall(lua, 0, LUA_MULTRET, 1), "abort runtime");
	} else {
		fatalf("abort parse time: %s", lua_tostring(lua, -1));
	}

	mark = used;
	char *source = argv[argc-1];

	rc = luaL_loadfile(lua, source);
	if (rc == 0) {
		ensuref(0 == lua_pcall(lua, 0, LUA_MULTRET, 1), "abort runtime");
	} else {
		fatalf("abort parse time: %s", lua_tostring(lua, -1));
	}

	lua_close(lua);
	return EXIT_SUCCESS;
}