#include "sphere.hpp"
#include <cassert>

using std::cout;
using std::endl;

int main()
{
	// unit sphere at origin
	sphere a(vec3(0.0, 0.0, 0.0), 1.0);

	ray r(vec3(1, 0, -10.0), vec3(0.0, 0.0, 1.0));

	float t;
	vec3 n;
	if (a.intersect(r, &t, n)) {
		cout << "t: " << t << endl;
		cout << "hit: " << r.parameterize(t) << endl;
		cout << "normal: " << n << endl << endl;
	} else {
		assert(0);
	}

	r = ray(vec3(0, 0, 0), vec3(0.0, 0.0, 1.0));

	if (a.intersect(r, &t, n)) {
		cout << "t: " << t << endl;
		cout << "hit: " << r.parameterize(t) << endl;
		cout << "normal: " << n << endl << endl;
	} else {
		assert(0);
	}

	r = ray(vec3(sqrt(2), 0, 0), vec3(0, 0, sqrt(2)) - vec3(sqrt(2), 0, 0));
	if (a.intersect(r, &t, n)) {
		cout << "t: " << t << endl;
		cout << "hit: " << r.parameterize(t) << endl;
		cout << "normal: " << n << endl << endl;
	} else {
		assert(0);
	}

	r = ray(vec3(2, 0, 0), vec3(0.0, 0, 1));
	if (a.intersect(r, &t, n)) {
		assert(0);
	}

	return 0;
}
