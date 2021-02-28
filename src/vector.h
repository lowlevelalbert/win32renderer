#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
	double x;
	double y;
} vec2_t;

typedef struct {
	double x;
	double y;
	double z;

} vec3_t;


vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

#endif
