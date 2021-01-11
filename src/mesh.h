
struct _mesh;
typedef struct _mesh mesh_t;

void meshFree(mesh_t* mesh);

void meshWriteObjFile(mesh_t* mesh, const char* fileName);

mesh_t* meshMarch(SDF3 sdf, int workers, int steps, double isoLevel);
