#include "vec3.hpp"
#include <iostream>

using std::cout;
using std::endl;

int main()
{
	vec3 a =  +vec3(1, -2, 3);
	cout << a << endl;

	a =  -vec3(1, -2, 3);
	cout << a << endl;

	cout << a * 1.5 << endl;
	cout << 1.5 * a << endl;

	a += 1.5*a;
	cout << a << endl;

	a -= 0.5*a;
	cout << a << endl;

	cout << a.length() << endl;
	cout << a.length2() << endl;

	cout << a.normalize() << endl;

	cout << a.length() << endl;
	cout << a.length2() << endl;

	cout << a.dot(a) << endl;
	cout << a.cross(a) << endl;

	vec3 i(1.0, 0.0, 0.0);
	vec3 j(0.0, 1.0, 0.0);
	vec3 k(0.0, 0.0, 1.0);

	cout << "ixj " << i.cross(j) << endl;
	cout << "jxk " << j.cross(k) << endl;
	cout << "kxi " << k.cross(i) << endl;

	return 0;
}
