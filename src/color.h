
#include <stdint.h>

typedef struct {
	double r, g, b;
} Color;

#define White (Color){1,1,1}
#define Black (Color){0.001,0.001,0.001}
#define Naught (Color){0,0,0}

typedef struct {
	uint8_t r, g, b, a;
} NRGBA;

Color colorScale(Color ca, double s);

Color colorMul(Color ca, Color cb);

Color colorDiv(Color ca, Color cb);

Color colorSub(Color ca, Color cb);

Color colorAdd(Color ca, Color cb);

Color colorMin(Color ca, Color cb);

Color colorMax(Color ca, Color cb);

double colorBrightness(Color c);

Color RGB(uint8_t r, uint8_t g, uint8_t b);

Color Hex(int x);

uint32_t NRGBAtoARGB32(NRGBA rgba);
