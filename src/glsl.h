
#define v2(x,y) (vec2){x,y}
#define v3(x,y,z) (vec3){x,y,z}

int iabs(int i);

int ineg(int i);

double dneg(double d);

int imax(int a, int b);

int imin(int a, int b);

double dmax(double a, double b);

double dmin(double a, double b);

double dclamp(double val, double low, double high);

double dround(double val);

#define abs(v) _Generic((v), vec2: vec2Abs, vec3: vec3Abs, int: iabs, double: fabs)(v)
#define neg(v) _Generic((v), vec2: vec2Neg, vec3: vec3Neg, int: ineg, double: dneg)(v)
#define len(v) _Generic((v), vec2: vec2Len, vec3: vec3Len)(v)
#define unit(v) _Generic((v), vec2: vec2Unit, vec3: vec3Unit)(v)
#define scale(v,s) _Generic((v), vec2: vec2Scale, vec3: vec3Scale)(v,s)
#define dot(a,b) _Generic((a), vec2: vec2Dot, vec3: vec3Dot)(a,b)
#define add(a,b) _Generic((a), vec2: vec2Add, vec3: vec3Add)(a,b)
#define sub(a,b) _Generic((a), vec2: vec2Sub, vec3: vec3Sub)(a,b)
#define mul(a,b) _Generic((a), vec2: vec2Mul, vec3: vec3Mul)(a,b)
#define div(a,b) _Generic((a), vec2: vec2Div, vec3: vec3Div)(a,b)
#define mod(a,b) _Generic((a), vec2: vec2Mod, vec3: vec3Mod)(a,b)
#define max(a,b) _Generic((a), vec2: vec2Max, vec3: vec3Max, int: imax, double: dmax)(a,b)
#define min(a,b) _Generic((a), vec2: vec2Min, vec3: vec3Min, int: imin, double: dmin)(a,b)
#define clamp(a,b,c) _Generic((a), vec3: vec3Clamp, double: dclamp)(a,b,c)
#define round(a) _Generic((a), vec3: vec3Round, double: dround)(a)

double sign(double val);
