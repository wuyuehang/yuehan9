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
			ray prim_ray(tlp + i*ystep + j*xstep, ortho_direction);
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
				quad[i*W*3+j*3] = 255;
				quad[i*W*3+j*3+1] = 0;
				quad[i*W*3+j*3+2] = 0;
			}
		}
	}

	dumper.serialize("simplest_ortho_raycast.ppm", W, H, quad);

	vector<surface*>::iterator ptr;
	for (ptr = objects.begin(); ptr != objects.end(); ptr++) {
		*ptr = nullptr;
		delete *ptr;
	}

	delete [] quad;

	return 0;
}
