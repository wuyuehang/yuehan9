#ifndef TRIANGLE_HPP_
#define TRIANGLE_HPP_

#include "surface.hpp"

using std::cout;
using std::endl;
using std::abs;

class triangle : public surface {
public:
	~triangle() {}
	triangle(vec3 a, vec3 b, vec3 c) {
		_meta[0] = a;
		_meta[1] = b;
		_meta[2] = c;
		_norm = calc_normal();
		if (abs(_norm.length()) < 1e-6) {
			cout << "colinear triangle" << endl;
		}
	}
	triangle(const triangle& tri) {
		_meta[0] = tri.get(0);
		_meta[1] = tri.get(1);
		_meta[2] = tri.get(2);
		_norm = calc_normal();
		if (abs(_norm.length()) < 1e-6) {
			cout << "colinear triangle" << endl;
		}
	}

	vec3 get(int i) const {
		return _meta[i];
	}

	bool intersect(const ray& r, float *t, vec3& n) const {
		// solve t for point intersect with plane located by _meta[0], _meta[1], _meta[2]
		float parallel_factor = r.D().dot(_norm);

		// step 1. check whether ray parallels to triangle or coplanar to triangle
		if (abs(parallel_factor) < 1e-6) {
			cout << "ray is parallel to the triangle" << endl;
			return false;
		}

		// step 2. check whether ray's origin coplanar to triangle
		if (contain_ray_origin(r)) {
			cout << "ray origin (" << r.O() << ") is inside the triangle" << endl;
			n = _norm;
			*t = 0.0;
			return true;
		}

		// step 3. check whether the hit point is inside triangle by barycentric condition
		float temp_t = _norm.dot(_meta[0] - r.O()) / parallel_factor;

		vec3 p = r.parameterize(temp_t);

		vec3 q = p - _meta[2];
		vec3 q0 = _meta[0] - _meta[2];
		vec3 q1 = _meta[1] - _meta[2];

		float q0q0 = q0.length2();
		float q1q1 = q1.length2();
		float q0q1 = q0.dot(q1);

		float determinant = q0q0 * q1q1 - q0q1 * q0q1;

		float qq0 = q.dot(q0);
		float qq1 = q.dot(q1);

		float alpha = (q1q1 * qq0 - q0q1 * qq1) / determinant;
		float beta = (-q0q1 * qq0 + q0q0 * qq1) / determinant;
		float gama = 1.0 - alpha - beta;

		if (0.0 <= alpha && alpha <= 1.0 && 0.0 <= beta && beta <= 1.0 && 0.0 <= gama && gama <= 1.0) {
			*t = temp_t;
			n = _norm;
			return true;
		} else {
			return false;
		}
	}

	// connect on plane point to check whether it's perpencular to normal
	bool contain_ray_origin(const ray& r) const {
		vec3 nb = r.O() - _meta[0];
		if (abs(nb.dot(_norm)) < 1e-6) {
			return true;
		} else {
			return false;
		}
	}

private:
	// we consider ccw as defaut positive surface normal facing us
	vec3 calc_normal() const {
		vec3 nb0 = _meta[1] - _meta[0];
		vec3 nb1 = _meta[2] - _meta[1];
		return nb0.cross(nb1);
	}

	vec3 _meta[3];
	vec3 _norm;
};

#endif
