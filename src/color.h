
#include <stdint.h>

typedef struct {
	double r, g, b;
} Color;

#define rgb(r,g,b) ((Color){r,g,b})
#define shine(c,m) colorScale(c,m)

#define White (Color){1,1,1}
#define Black (Color){0.001,0.001,0.001}
#define Naught (Color){0,0,0}

#define isNaught(c) (c.r < 0.000001 && c.g < 0.000001 && c.b < 0.000001)

// non-alpha-premultiplied
typedef struct {
	uint8_t r, g, b, a;
} NRGBA;

Color colorScale(Color ca, double s);

Color colorMul(Color ca, Color cb);

Color colorDiv(Color ca, Color cb);

Color colorSub(Color ca, Color cb);

Color colorAdd(Color ca, Color cb);

double colorBrightness(Color c);

Color hex(int x);

uint32_t NRGBAtoARGB32(NRGBA rgba);
