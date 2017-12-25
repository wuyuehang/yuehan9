#ifndef RAY_H_
#define RAY_H_

#include "vec3.hpp"

class ray {
public:
	~ray() {}
	// original m, direction n
	ray(const vec3& m, const vec3& n) {
		meta[0] = m;
		meta[1] = n;
	}

	vec3 O() const { return meta[0]; }
	vec3 D() const { return meta[1]; }

	// subsitute t to locate the point on ray
	vec3 parameterize(float t) const {
		return vec3(meta[0] + t * meta[1]);
	}

private:
	vec3 meta[2];
};

#endif
