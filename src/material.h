
vec3 pickVec3(struct random_data *rnd);

Material Matt(Color c);

Material Light(Color c);

Material Metal(Color c, double roughness);

Material Glass(Color c, double refractiveIndex);

Material ShadowCatcher();
