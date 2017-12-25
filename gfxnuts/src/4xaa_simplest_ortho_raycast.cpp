#include "ppm.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include <vector>

#define W	1024
#define H	1024

using std::vector;

int main()
{
	unsigned char *quad = new unsigned char [W*H*3];
	vector<surface*> objects;
	ppm dumper;

	objects.push_back(new sphere(vec3(-0.5, 0.5, 0), 0.25));
	objects.push_back(new triangle(vec3(0.25, 0.25, 0), vec3(0.75, 0.25, 0), vec3(0.5, 0.75, 0)));
	objects.push_back(new sphere(vec3(0.5, -0.5, 0), 0.25));
	objects.push_back(new triangle(vec3(-0.25, -0.25, 0), vec3(-0.75, -0.25, 0), vec3(-0.5, -0.75, 0)));

	// ray emit order from top-left to bottom left
	// projection plane lays at z = -1.
	vec3 tlp = vec3(-1, 1, -1);
	vec3 xstep = vec3(2.0/W, 0, 0);
	vec3 ystep = vec3(0, -2.0/H, 0);

	vec3 ortho_direction(0, 0, -1);

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			vec3 current_pixel = tlp + i*ystep + j*xstep;

			// 4x order sample pattern
			vector<vec3> subpixels;
			subpixels.push_back(current_pixel + vec3(-0.5/W, +0.5/H, 0));
			subpixels.push_back(current_pixel + vec3(+0.5/W, +0.5/H, 0));
			subpixels.push_back(current_pixel + vec3(-0.5/W, -0.5/H, 0));
			subpixels.push_back(current_pixel + vec3(+0.5/W, +0.5/H, 0));

			// iterate over subpixels to average final color
			unsigned int rgb[3] = { 0 };

			for (auto subp = subpixels.begin(); subp != subpixels.end(); subp++) {
				ray prim_ray = ray(*subp, ortho_direction);

				float t = 1e10;
				surface *found_obj = nullptr;

				for (auto ptr = objects.begin(); ptr != objects.end(); ptr++) {
					float temp_t;
					vec3 unused_normal;

					if ((*ptr)->intersect(prim_ray, &temp_t, unused_normal)) {
						if (temp_t < t) {
							t = temp_t;
							found_obj = *ptr;
						}
					}
				}

				if (found_obj) {
					rgb[0] += 255;
					rgb[1] += 0;
					rgb[2] += 0;
				}
			}

			quad[i*W*3+j*3] = static_cast<unsigned char>(rgb[0] >> 2);
			quad[i*W*3+j*3+1] = static_cast<unsigned char>(rgb[1] >> 2);
			quad[i*W*3+j*3+2] = static_cast<unsigned char>(rgb[2] >> 2);
		}
	}

	dumper.serialize("4xaa_simplest_ortho_raycast.ppm", W, H, quad);

	vector<surface*>::iterator ptr;
	for (ptr = objects.begin(); ptr != objects.end(); ptr++) {
		*ptr = nullptr;
		delete *ptr;
	}

	delete [] quad;

	return 0;
}
