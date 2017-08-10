#include <fstream>
#include <iostream>
#include "vec3.hpp"

using std::endl;
using std::ofstream;

#define W	500
#define H	500
#define DIM "500 500"

void calculus_line_equation(vec3& start, vec3& end, float& A, float& B, float& C)
{
	A = start.y() - end.y();
	B = end.x() - start.x();
	C = start.x() * end.y() - end.x() * start.y();
}

// use right hand ruler
// cw: cross product points into the screen, negative z
// ccw: cross product point out from the screen , positive z
bool triangle_is_ccw(vec3 u, vec3 v)
{
	// check z's sign
	float cross_z = u.x() * v.y() - u.y() * v.x();
	return (cross_z > 0.0);
}

int main()
{
	ofstream pfile;
	pfile.open("triangle_swr.ppm");

	pfile << "P3" << endl;
	pfile << DIM << endl;
	pfile << "255" << endl;

#if 0
	// dont care z plane
	// cw triangle
	vec3 v0 = vec3(100, 100, 0);
	vec3 v1 = vec3(200, 400, 0);
	vec3 v2 = vec3(300, 100, 0);
#else
	// ccw triangle
	vec3 v0 = vec3(100, 100, 0);
	vec3 v1 = vec3(300, 100, 0);
	vec3 v2 = vec3(200, 400, 0);

#endif

	float A01, B01, C01;
	calculus_line_equation(v0, v1, A01, B01, C01);

	float A12, B12, C12;
	calculus_line_equation(v1, v2, A12, B12, C12);

	float A20, B20, C20;
	calculus_line_equation(v2, v0, A20, B20, C20);

	bool is_ccw = triangle_is_ccw(v1-v0, v2-v0);

	// to align with cartesian coordinate,
	// we origin (0, 0) from left bottom,
	// while pixel storage starts from top left
	// so a point (i, j) in cartesian coordinate relates
	// with (i, H - j) in pixel memory address
	for ( int i = 0; i < H; i++ ) {
		for ( int j = 0; j < W; j++ ) {
			// paint "positive" side of the line equaltion
			float test_line01 = A01*j + B01*(H-i) + C01;
			float test_line12 = A12*j + B12*(H-i) + C12;
			float test_line20 = A20*j + B20*(H-i) + C20;

			// if triangle is ccw, the inner is on the positive side
			// if triangle is cw, the inner is on the negative side
			if ( !is_ccw && test_line01 < 0.0 && test_line12 < 0.0 && test_line20 < 0.0) {
				pfile << "255 0 0 " << endl; // paint cw triangle as red
			} else if ( is_ccw && test_line01 > 0.0 && test_line12 > 0.0 && test_line20 > 0.0) {
				pfile << "0 0 255 " << endl; // paint ccw triangle as blue
			} else {
				pfile << "0 0 0 " << endl; // outside the triangle
			}
		}
	}

	pfile.close();

	return 0;
}
