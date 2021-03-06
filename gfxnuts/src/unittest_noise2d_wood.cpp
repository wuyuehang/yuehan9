#include "noise2d.hpp"
#include "ppm.hpp"

int main()
{
	noise2d a;

	int amp_factor = 8.0;
	unsigned char *texture = new unsigned char [noise2d::MetaWidth*amp_factor*noise2d::MetaHeight*amp_factor*3];
	int NumLayers = 7;
	int layer_factor[NumLayers];
	float base_frequency_factor = 0.2; // be friendly with it

	float s = 2.0;
	float layer_accumulate = 1.0*(1-pow(1/s, NumLayers))/(1-1/s);

	for (int i = 0; i < NumLayers; i++) {
		layer_factor[i] = pow(s, i);
	}

	ppm b;

	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {
			float noise_sum = 0.0;

			for (int k = 0; k < NumLayers; k++) {
				float fi = float(i)/noise2d::MetaWidth/amp_factor*noise2d::MetaWidth;
				float fj = float(j)/noise2d::MetaHeight/amp_factor*noise2d::MetaHeight;

				float v = a.lerp(fi*base_frequency_factor*layer_factor[k], fj*base_frequency_factor*layer_factor[k]);
				//v = fabs(2.0*v-1.0);
				noise_sum += 1.0/layer_factor[k]*v;
			}

			noise_sum /= layer_accumulate;

			noise_sum *= 20;
			noise_sum = noise_sum - int(noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*noise_sum);;
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = 0;
		}
	}

	b.serialize("unittest_noise2d_wood_2048x2048_mixed_f.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);

	for (int i = 0; i < noise2d::MetaHeight*amp_factor; i++) {
		for (int j = 0; j < noise2d::MetaWidth*amp_factor; j++) {

			float fi = float(i)/noise2d::MetaWidth/amp_factor*noise2d::MetaWidth;
			float fj = float(j)/noise2d::MetaHeight/amp_factor*noise2d::MetaHeight;

			float noise_sum = a.lerp(fi*base_frequency_factor, fj*base_frequency_factor);

			noise_sum *= 10;
			noise_sum = noise_sum - int(noise_sum);

			texture[3*i*noise2d::MetaWidth*amp_factor+j*3] = (unsigned char)(255*noise_sum);
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+1] = (unsigned char)(255*noise_sum);;
			texture[3*i*noise2d::MetaWidth*amp_factor+j*3+2] = 0;
		}
	}

	b.serialize("unittest_noise2d_wood_2048x2048_single_f.ppm", noise2d::MetaWidth*amp_factor, noise2d::MetaHeight*amp_factor, texture);
	delete [] texture;

	return 0;
}
