#include "triangle.hpp"

int main()
{
	vec3 m0(0.0, 1.0, 1.0);
	vec3 m1(1.0, 2.0, 0.0);
#if 0
	vec3 m2(3.0, 4.0, -2);
#else
	vec3 m2(0.5, 1.5, 0.5);
#endif
	triangle tri_colinear(m0, m1, m2);

	tri_colinear = triangle(vec3(0, 1, 0), vec3(0, 1, 0), vec3(1.0, -1, -3));

	m0 = vec3(1, 0, 0);
	m1 = vec3(0, 1, 0);
	m2 = vec3(0, 0, 1);

	triangle tri(m0, m1, m2);

	float t;
	vec3 n;
	ray parallel_r = ray(m0, 3.0*(m0 - (m1 + m2)*0.5));
	tri.intersect(parallel_r, &t, n);

	parallel_r = ray(2*m0, 2*m0-2*m2);
	tri.intersect(parallel_r, &t, n);

	ray contain_r = ray((m0+m1)*0.5, vec3(1, 1, 1));
	tri.intersect(contain_r, &t, n);
	tri.contain_ray_origin(contain_r);

	vec3 ori(2, 3, 4);
	ray intersect_at_edge = ray(ori, (0.25*m1+0.75*m0)-ori);
	if (tri.intersect(intersect_at_edge, &t, n)) {
		cout << intersect_at_edge.parameterize(t) << endl;
	}

	ray intersect_at_vertice = ray(ori, m2-ori);
	if (tri.intersect(intersect_at_vertice, &t, n)) {
		cout << intersect_at_vertice.parameterize(t) << endl;
	}

	ray intersect_inside = ray(ori, 0.2*m0+0.3*m1+0.5*m2-ori);
	if (tri.intersect(intersect_inside, &t, n)) {
		cout << intersect_inside.parameterize(t) << endl;
	}

	ray intersect_outside = ray(ori, 0.4*m0-0.8*m1+1.4*m2-ori);
	if (!tri.intersect(intersect_outside, &t, n)) {
		cout << intersect_outside.parameterize(t) << endl;
	}

	return 0;
}
