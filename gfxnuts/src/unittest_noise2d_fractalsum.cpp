#include "noise2d.hpp"
#include "ppm.hpp"

int main()
{
	noise2d a;

	int amp_factor = 8.0;
	unsigned char *texture = new unsigned char [noise2d::MetaWidth*amp_factor*noise2d::MetaHeight*amp_factor*3];
	int NumLayers = 7;
	int layer_factor[NumLayers];
	float base_frequency_factor = 1.382; // be friendly with it
	base_frequency_factor = 0.0682; // look like sky

	float s = 2.0;
	float layer_accumulate = 1.0*(1-pow(1/s, NumLayers))/(1-1/s);

	for (int i = 0; i < NumLayers; i++) {
		layer_factor[i] = pow(s, i);
	}

	// generate 2048x2048 single frequency
	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {

			float fi = float(i)/noise2d::MetaWidth/amp_factor*noise2d::MetaWidth;
			float fj = float(j)/noise2d::MetaHeight/amp_factor*noise2d::MetaHeight;
			float noise_sum = a.lerp(fi*base_frequency_factor, fj*base_frequency_factor);

			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = (unsigned char)(255*noise_sum);
		}
	}

	ppm b;
	b.serialize("unittest_noise2d_fractalsum_2048x2048_single_f.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);

	// generate 2048x2048 multiple mixed frequency
	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {
			float noise_sum = 0.0;

			for (int k = 0; k < NumLayers; k++) {
				float fi = float(i)/noise2d::MetaWidth/amp_factor*noise2d::MetaWidth;
				float fj = float(j)/noise2d::MetaHeight/amp_factor*noise2d::MetaHeight;
				noise_sum += 1.0/layer_factor[k]*a.lerp(fi*base_frequency_factor*layer_factor[k], fj*base_frequency_factor*layer_factor[k]);
			}

			noise_sum /= layer_accumulate;

			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = (unsigned char)(255*noise_sum);
		}
	}

	b.serialize("unittest_noise2d_fractalsum_2048x2048_mixed_f.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);

	delete [] texture;

	return 0;
}
