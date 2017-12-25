#ifndef SPHERE_HPP_
#define SPHERE_HPP_

#include "surface.hpp"

using std::signbit;
using std::cout;
using std::endl;

class sphere : public surface {
public:
	~sphere() {}
	sphere(const vec3& o, float r) {
		radius = r;
		heart = o;
	}
	sphere(const sphere& s) {
		radius = s.R();
		heart = s.center();
	}

	vec3 center() const { return heart; }
	float R() const { return radius; }

	// return parameterize t to locate point at ray.
	// return hitpoint's normal vector
	bool intersect(const ray& r, float *t, vec3& n) const {
		if (contain_ray_origin(r)) {
			cout << "ray origin (" << r.O() << ") is inside the sphere" << endl;
		}

		vec3 o2c = r.O() - heart;
		float a = r.D().length2();
		float b = 2.0*r.D().dot(o2c);
		float c = o2c.length2() - radius*radius;

		float discrimination = b*b - 4*a*c;

		// future: set t internal for sanity
		//if (discrimination > 0.0) {
		if (!signbit(discrimination)) {
			*t = 0.5*(-b-sqrt(discrimination))/a;
			n = r.parameterize(*t) - heart;
			return true;
		} else {
			return false;
		}
	}

	// return true if the ray's origin is inside the sphere
	bool contain_ray_origin(const ray& r) const {
		vec3 o2c = r.O() - heart;
		return (o2c.length2() < radius*radius);
	}

private:
	vec3 heart;
	float radius;
};

#endif
