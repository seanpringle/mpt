
vec3 pickVec3(struct random_data *rnd);

material_t Matt(Color c);

material_t Light(Color c);

material_t Metal(Color c, double roughness);

material_t Glass(Color c, double refractiveIndex);

material_t ShadowCatcher();
