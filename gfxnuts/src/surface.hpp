#ifndef SURFACE_HPP_
#define SURFACE_HPP_

#include "ray.hpp"
#include "vec3.hpp"

class surface {
public:
	virtual bool intersect(const ray& r, float *t, vec3& n) const = 0;
	virtual bool contain_ray_origin(const ray& r) const = 0;
};

#endif
