#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "ray.hpp"
#include <cassert>
#include <ctime>
#include <cstdlib>

using std::srand;
using std::rand;
using std::time;

class sphere
{

public:
	sphere(vec3 o, float rad) {
		heart = o;
		radius = rad;
		srand(time(0));
	}
	~sphere() {}

	// member access
	vec3 center() { return heart; }
	float R() { return radius; }

	// calculate intersection between ray and sphere.
	// if hit, return the parameter t (subsitute ray equation to get hit point;
	// if not hit, return a negative value.
	float intersect(const ray& r) {
		vec3 c2r = r.origin() - heart;
		float a = r.direction().dot(r.direction());
		float b = 2.0 * r.direction().dot(c2r);
		float c = c2r.dot(c2r) - radius * radius;
		float discriminat = b*b - 4*a*c;

		if (discriminat > 0.0)
			return 0.5*(-b-sqrt(discriminat))/a;
		else
			return -1000.0;
	}

#if 0
	vec3 get_diffuse(vec3 hit_point, vec3 light_loc) {
		assert(metal == false);

		vec3 normal = hit_point - heart;

		// cc is center of tangent unit sphere
		vec3 cc =  hit_point + 1.0 / normal.len() * normal;

		// emulate diffuse material by generate a tangent unit
		// sphere at the hit point and, random a point inside the
		// unit sphere. with this random point to disturb the original
		// normal vector point from the object out to hit point
		vec3 disturbance = vec3(rand() % 100 / 100.0, rand() % 100 / 100.0, rand() % 100 / 100.0);

		vec3 disturb_normal = cc + disturbance - hit_point;

		disturb_normal.norm();

		vec3 h2l = light_loc - hit_point;
		h2l.norm();

		float diffuse_lumiance_weight = h2l.dot(disturb_normal);

		float red, green, blue;
		if (diffuse_lumiance_weight < 0.0) {
			red = 0;
		} else {
			red = diffuse_lumiance_weight;
		}
		green = 0;
		blue = 0;

		return vec3(red, green, blue);
	}
#endif

private:
	vec3 heart;
	float radius;
};

#endif
