#ifndef VEC3_HPP_
#define VEC3_HPP_

#include <iostream>
#include <cassert>
#include <cmath>

using std::ostream;

class vec3 {
public:
	// constructor and destructor
	~vec3() {}
	vec3() {
		meta[0] = 0.0;
		meta[1] = 0.0;
		meta[2] = 0.0;
	}
	vec3(const vec3& v) {
		meta[0] = v.x();
		meta[1] = v.y();
		meta[2] = v.z();
	}
	vec3(float o, float p, float q) {
		meta[0] = o;
		meta[1] = p;
		meta[2] = q;
	}
	vec3(const vec3 *v) {
		meta[0] = v->x();
		meta[1] = v->y();
		meta[2] = v->z();
	}

	// meta data access and index
	float x() const { return meta[0]; }
	float y() const { return meta[1]; }
	float z() const { return meta[2]; }
	float r() const { return meta[0]; }
	float g() const { return meta[1]; }
	float b() const { return meta[2]; }
	float operator[](int i) const { assert(i >= 0 && i <= 2); return meta[i]; }

	// ostream serialize
	friend ostream& operator<<(ostream& os, const vec3& v) {
		os << v.x() << ", " << v.y() << ", " << v.z();
		return os;
	}

	friend ostream& operator<<(ostream& os, const vec3 *v) {
		os << v->x() << ", " << v->y() << ", " << v->z();
		return os;
	}

	// arithmetic without side effect
	const vec3 operator+() const { return *this; }
	const vec3 operator+(const vec3& v) const {
		return vec3(meta[0] + v.x(), meta[1] + v.y(), meta[2] + v.z());
	}
	const vec3 operator-() const { return vec3(-meta[0], -meta[1], -meta[2]); }
	const vec3 operator-(const vec3& v) const {
		return vec3(meta[0] - v.x(), meta[1] - v.y(), meta[2] - v.z());
	}
	const vec3 operator*(const float s) const {
		return vec3(meta[0] * s, meta[1] * s, meta[2] * s);
	}
	friend const vec3 operator*(double s, const vec3& v) {
		return vec3(v.x() * s, v.y() * s, v.z() * s);
	}

	// arithmetic with side effect
	vec3& operator+=(const vec3& v) {
		meta[0] += v.x();
		meta[1] += v.y();
		meta[2] += v.z();
		return *this;
	}

	vec3& operator-=(const vec3& v) {
		meta[0] -= v.x();
		meta[1] -= v.y();
		meta[2] -= v.z();
		return *this;
	}

	vec3& operator*=(const float s) {
		meta[0] *= s;
		meta[1] *= s;
		meta[2] *= s;
		return *this;
	}

	// geometry arithmetic, keep calculation eveytime we get since "+=, *=, -=, normalize
	// operations has side effect
	float length() const {
		return sqrt(meta[0] * meta[0] + meta[1] * meta[1] + meta[2] * meta[2]);
	}

	float length2() const {
		return (meta[0] * meta[0] + meta[1] * meta[1] + meta[2] * meta[2]);
	}

	vec3& normalize() {
		float l = this->length();
		meta[0] /= l;
		meta[1] /= l;
		meta[2] /= l;
		return *this;
	}

	vec3 cross(const vec3& v) {
		// perform this x v
		return vec3(meta[1] * v.z() - v.y() * meta[2],
				meta[2] * v.x() - v.z() * meta[0],
				meta[0] * v.y() - v.x() * meta[1]);
	}

	float dot(const vec3& v) const {
		return (meta[0] * v.x() + meta[1] * v.y() + meta[2] * v.z());
	}

private:
	float meta[3];
};

#endif
