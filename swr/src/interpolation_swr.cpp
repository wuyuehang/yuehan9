#include <fstream>
#include <iostream>
#include "vec3.hpp"

using std::cout;
using std::endl;
using std::ofstream;
using std::max;
using std::min;

#define W	500
#define H	500
#define DIM	"500 500"

void calculus_barycentric(vec3 I, vec3 v1, vec3 v2, vec3 v3, float& alpha, float& beta)
{
	float factor_alpha = (I.x() - v3.x())*(v2.y() - v3.y()) - (v2.x() - v3.x())*(I.y() - v3.y());
	float factor_beta  = (I.x() - v3.x())*(v1.y() - v3.y()) - (v1.x() - v3.x())*(I.y() - v3.y());
	float factor_common = (v2.x() - v3.x())*(v1.y() - v3.y()) - (v1.x() - v3.x())*(v2.y() - v3.y());

	alpha = factor_alpha / (-factor_common);
	beta = factor_beta / factor_common;
}

void calculus_bounding_box(vec3 v1, vec3 v2, vec3 v3, int& xmin, int& xmax, int& ymin, int& ymax)
{
	xmin = min(v1.x(), min(v2.x(), v3.x()));
	xmax = max(v1.x(), max(v2.x(), v3.x()));

	ymin = min(v1.y(), min(v2.y(), v3.y()));
	ymax = max(v1.y(), max(v2.y(), v3.y()));
}

vec3 calculus_linear_interpolation(vec3 v1, vec3 v2, vec3 v3, float alpha, float beta)
{
	return alpha*v1 + beta*v2 + (1.0 - alpha - beta)*v3;
}

int main()
{
	ofstream pfile;
	pfile.open("interpolation_swr.ppm");

	pfile << "P3" << endl;
	pfile << DIM << endl;
	pfile << "255" << endl;

#if 1
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

	int xmin = -1;
	int xmax = -1;
	int ymin = -1;
	int ymax = -1;

	calculus_bounding_box(v0, v1, v2, xmin, xmax, ymin, ymax);
	cout << "xmin " << xmin << endl;
	cout << "xmax " << xmax << endl;
	cout << "ymin " << ymin << endl;
	cout << "ymax " << ymax << endl;

	// if there's a reserved data buffer, we can shorten
	// the intervals of the loop boundary
#if 0
	for (int i = ymin; i <= ymax; i++) {
		for (int j = xmin; j <= xmax; j++) {
#else
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
#endif
			if (i < ymin || i > ymax || j < xmin || j > xmax) {
				pfile << "0 0 0 ";
				continue;
			}

			float alpha = -1;
			float beta = -1;
			float gama = -1;
			vec3 I = vec3(j, H-i, 0);

			calculus_barycentric(I, v0, v1, v2, alpha, beta);

			gama = 1.0 - alpha - beta;

			if ((alpha <= 1.0 && alpha >= 0.0) &&
					(beta <= 1.0 && beta >= 0.0) &&
					(gama <= 1.0 && gama >= 0.0)) {
				vec3 color = calculus_linear_interpolation(vec3(1.0, 0.0, 0.0),
						vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0), alpha, beta);
				pfile << floor(color.r() * 255.0) << " " << floor(color.g() * 255.0)
					<< " " << floor(color.b() * 255.0) << " ";
			} else {
				pfile << "0 0 0 ";
			}
		}
		pfile << endl;
	}

	pfile.close();

	return 0;
}
