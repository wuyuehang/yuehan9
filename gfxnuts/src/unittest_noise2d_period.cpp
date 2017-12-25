#include "noise2d.hpp"
#include "ppm.hpp"

int main()
{
	noise2d a;
	//a.dump_meta();

	cout << "\nref coordinate" << endl;
	a.lerp(0.5, 1.2);

	cout << "\nref coordinate equal positive grids" << endl;
	cout << a.lerp(0.5+noise2d::MetaWidth, 1.2+noise2d::MetaHeight) << endl;

	cout << "\nref coordinate non-equal positive grids" << endl;
	cout << a.lerp(0.5+noise2d::MetaWidth, 1.2+2*noise2d::MetaHeight) << endl;

	cout << "\nref coordinate random positive grids" << endl;
	cout << a.lerp(0.5+1234*noise2d::MetaWidth, 1.2+9876*noise2d::MetaHeight) << endl;

	cout << "\nref coordinate negative grids" << endl;
	cout << a.lerp(0.5-noise2d::MetaWidth, 1.2) << endl;

	cout << "\nref coordinate negative grids" << endl;
	cout << a.lerp(0.5-noise2d::MetaWidth, 1.2-2*noise2d::MetaHeight) << endl;

	cout << "\nref coordinate random negative grids" << endl;
	cout << a.lerp(0.5-7654*noise2d::MetaWidth, 1.2-9812*noise2d::MetaHeight) << endl;

	int amp_factor = 2;
	unsigned char *texture = new unsigned char [noise2d::MetaWidth*amp_factor*noise2d::MetaHeight*amp_factor*3];

	// generate 512x512 without outline
	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = (unsigned char)(255*a.lerp(i, j));
		}
	}

	ppm b;
	b.serialize("unittest_noise2d_period_512x512_noedge.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);

	// generate 512x512 with outline
	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = (unsigned char)(255*a.lerp(i, j));
			// emphasiz the outline
			if (i%noise2d::MetaWidth == 0 || j%noise2d::MetaHeight == 0) {
				texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = 0;
				texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = 0;
			}
		}
	}
	b.serialize("unittest_noise2d_period_512x512_edge.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);

	// generate 256x256 without outline
	for (int i = 0; i < noise2d::MetaHeight; i++) {
		for (int j = 0; j < noise2d::MetaWidth; j++) {
			texture[3*i*noise2d::MetaWidth+j*3] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth+j*3+1] = (unsigned char)(255*a.lerp(i, j));
			texture[3*i*noise2d::MetaWidth+j*3+2] = (unsigned char)(255*a.lerp(i, j));
		}
	}
	b.serialize("unittest_noise2d_period_256x256_noedge.ppm", noise2d::MetaWidth, noise2d::MetaHeight, texture);

	delete [] texture;

	return 0;
}
