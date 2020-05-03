
typedef struct {
	double x, y, z;
} vec3;

double vec3Len(vec3 va);

vec3 vec3Scale(vec3 va, double s);

vec3 vec3Abs(vec3 va);

vec3 vec3Neg(vec3 va);

vec3 vec3Round(vec3 va);

vec3 vec3Unit(vec3 va);

double vec3Dot(vec3 va, vec3 vb);

vec3 vec3Mul(vec3 va, vec3 vb);

vec3 vec3Div(vec3 va, vec3 vb);

vec3 vec3Sub(vec3 va, vec3 vb);

vec3 vec3Add(vec3 va, vec3 vb);

vec3 vec3Min(vec3 va, vec3 vb);

vec3 vec3Max(vec3 va, vec3 vb);

vec3 vec3Cross(vec3 va, vec3 vb);

vec3 vec3Mod(vec3 va, vec3 vb);

vec3 vec3Reflect(vec3 v, vec3 n);

bool vec3Refract(vec3 v, vec3 n, double niOverNt, vec3 *refracted);

vec3 vec3Clamp(vec3 v, vec3 l, vec3 h);

