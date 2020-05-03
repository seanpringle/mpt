
typedef struct {
	double x, y;
} vec2;

double vec2Len(vec2 va);

vec2 vec2Scale(vec2 va, double s);

vec2 vec2Abs(vec2 va);

vec2 vec2Neg(vec2 va);

vec2 vec2Unit(vec2 va);

double vec2Dot(vec2 va, vec2 vb);

vec2 vec2Mul(vec2 va, vec2 vb);

vec2 vec2Div(vec2 va, vec2 vb);

vec2 vec2Sub(vec2 va, vec2 vb);

vec2 vec2Add(vec2 va, vec2 vb);

vec2 vec2Min(vec2 va, vec2 vb);

vec2 vec2Max(vec2 va, vec2 vb);

vec2 vec2Mod(vec2 va, vec2 vb);
