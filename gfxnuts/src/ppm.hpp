#ifndef PPM_HPP_
#define PPM_HPP_

#include <fstream>
#include <iostream>

using std::ofstream;
using std::endl;
using std::cout;
using std::hex;

class ppm {
public:
	~ppm() {}
	ppm() {}

	void serialize(const char *filename, int w, int h, unsigned char *img) {
		ofstream pfile;
		pfile.open(filename);

		pfile << "P6" << endl;
		// column(w):row(h)
		pfile << w << " " << h << endl;
		pfile << "255" << endl;

		// left->right && top->bottom
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				pfile << hex << img[i*w*3+3*j];
				pfile << hex << img[i*w*3+3*j+1];
				pfile << hex << img[i*w*3+3*j+2];
			}
		}

		pfile.close();
	}
};

#endif
