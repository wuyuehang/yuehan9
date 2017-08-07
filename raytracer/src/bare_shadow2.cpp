#include "ray.hpp"
#include "sphere.hpp"
#include "vec3.hpp"

#include <iostream>
#include <fstream>
#include <vector>

using std::cout;
using std::endl;
using std::vector;
using std::ofstream;

#define W_EQUAL_H	1

#ifdef W_EQUAL_H
#define W	1280
#define H 1280
#define DIMENSION	"1280 1280"
#else
#define W	1280
#define H 1024
#define DIMENSION	"1280 1024"
#endif

// reflection point in vector v by the normal
vec3 reflection(vec3 v, vec3 n)
{
	return (v - 2.0*v.dot(n)/n.lenght()*n);
}

// camera or eye origins at (0, 0, 0), looking at the negative z.
//
// generate ray from origin to projection plane (z = -1), orders from top-left to
// bottom right.
//
// the projection plane is normalized in interval by x [-1, 1] and y [-1, 1].
// thus, the top-left pixel is (-1, 1) and bottom-right is (1, -1) in cartesian

int main()
{
	// output file
	ofstream pfile;
	pfile.open("bare_shadow2.ppm");

	// ppm ascii header
	// objects container
	pfile << "P3" << endl;
	pfile << DIMENSION << endl;
	pfile << "255" << endl;

	// add objects into the scene
	vector<sphere> objects;
	objects.push_back(sphere(vec3(0, 0, -4), 1.0)); // center ball
	objects.push_back(sphere(vec3(0.25, 1, -3), 0.2));	// up right ball
	objects.push_back(sphere(vec3(-0.25, 1, -3), 0.2));	// up top ball
	objects.push_back(sphere(vec3(0.6, 0.9, -2.75), 0.18));	// up top ball
	objects.push_back(sphere(vec3(-0.6, 0.9, -2.75), 0.18));	// up top ball

	// camera or eye location
	vec3 camera = vec3(0, 0, 0);

	// light location
	vec3 light = vec3(0, 2, -2);

	// scanline steps from top-left to bottom left
	// warn, projection plane lies at z = -1, remember
	// to add objects behind it, or will show nothing
	vec3 tlp = vec3(-1, 1, -1);
	vec3 u = vec3(2.0/W, 0, 0);
	vec3 v = vec3(0, -2.0/H, 0);

	// iterating per pixel, simpliest ray tracer casts primary ray only
	// to project 3D targets to the plane
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			// from eye or camera to projection plane
			ray primary_ray(camera, tlp + i*v + j*u);

			sphere *found_obj = nullptr;
			float t = 10000.0; // store minimal t in the ray
			float temp = t + 1.0; // store temporariy hit t

			for (int k = 0; k < objects.size(); k++) {
				temp = objects[k].intersect(primary_ray);

				if (temp > 0.0 && temp < t) {
					t = temp;
					found_obj = &objects[k];
				}
			}

			if (found_obj) {
				vec3 hit_point = primary_ray.parameterize(t);
				// generate shadow ray or secondary ray just by reflection on normal
				// vector of surface
				ray shadow_ray(hit_point, reflection(primary_ray.direction(), hit_point - found_obj->center()));

				bool isshadow = false;

				for (int k = 0; k < objects.size(); k++) {
					// should check intersection between "other" objects
					if (objects[k].intersect(shadow_ray) > 0 && found_obj != &objects[k]) {
					//if (objects[k].intersect(shadow_ray) > 0) {
						isshadow = true;
						break;
					}
				}

				if (isshadow) {
					pfile << "0 0 0 " << endl;
				} else {
					pfile << "255 0 0 " << endl;
				}
			} else {
				pfile << "0 0 0 " << endl;
			}
		}

		pfile << endl;
	}

	pfile.close();

	return 0;
}
