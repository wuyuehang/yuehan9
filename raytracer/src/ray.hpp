#ifndef RAY_H
#define RAY_H

#include "vec3.hpp"

class ray
{

public:
	// original m, direction n
	ray(const vec3& m, const vec3& n) {
		meta[0] = m;
		meta[1] = n;
	}

	~ray() {}

	vec3 origin() const { return meta[0]; }
	vec3 direction() const { return meta[1]; }

	// parameter t to locate the point on ray
	vec3 parameterize(float t) {
		return vec3(meta[0] + t * meta[1]);
	}

private:
	vec3 meta[2];
};

#endif
