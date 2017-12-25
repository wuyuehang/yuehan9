#include "ppm.hpp"

#define w	2
#define h 2
int main()
{
	unsigned char *quad = new unsigned char [w*h*3];

	quad[0] = 255;
	quad[1] = 0;
	quad[2] = 0;

	quad[3] = 0;
	quad[4] = 255;
	quad[5] = 0;

	quad[6] = 0;
	quad[7] = 0;
	quad[8] = 255;

	quad[9] = 255;
	quad[10] = 255;
	quad[11] = 0;

	ppm dumper;
	dumper.serialize("unittest_ppm.ppm", w, h, quad);

	delete [] quad;

	return 0;
}
