#ifndef NOISE2D_HPP_
#define NOISE2D_HPP_

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

using std::rand;
using std::cout;
using std::endl;
using std::srand;

class noise2d
{
public:
	static const unsigned int MetaWidth = 256;
	static const unsigned int MetaHeight = 256;

	~noise2d() {}
	noise2d(unsigned int seed = 1900) {
		srand(seed);

		for (unsigned int i = 0; i < MetaWidth*MetaHeight; i++)
			_meta[i] = float(double(rand())/RAND_MAX);
	}

	void dump_meta(void) const {
		for (unsigned int i = 0; i < MetaHeight; i++) {
			for (unsigned int j = 0; j < MetaWidth; j++) {
				cout << _meta[i*MetaWidth+j] << ", ";
			}
			cout << endl;
		}
	}

	float lerp(float x, float y) const {
		int xwrap = (int)x - (x < 0.0 && x != int(x));
		float xfrac = x - xwrap;
		int xleft = (xwrap%MetaWidth + MetaWidth) % MetaWidth;
		int xright = (xleft == (MetaWidth-1)) ? 0 : (xleft+1);

		int ywrap = (int)y - (y < 0.0 && y != int(y));
		float yfrac = y - ywrap;
		int yleft = (ywrap%MetaHeight + MetaHeight) % MetaHeight;
		int yright = (yleft == (MetaHeight-1)) ? 0 : (yleft+1);

#if 0
		cout << _meta[yleft*MetaHeight+xleft] << ", " << _meta[yleft*MetaHeight+xright] << endl;
		cout << _meta[yright*MetaHeight+xleft] << ", " << _meta[yright*MetaHeight+xright] << endl;

		float xorient1 = _linear_interp(_meta[yleft*MetaWidth+xleft], _meta[yleft*MetaWidth+xright], xfrac);

		float xorient2 = _linear_interp(_meta[yright*MetaWidth+xleft], _meta[yright*MetaWidth+xright], xfrac);

		float yorient = _linear_interp(xorient1, xorient2, yfrac);
		cout << xorient1 << ", " << xorient2 << ", " << yorient << endl;
#else
		float xorient1 = _linear_interp(_meta[yleft*MetaWidth+xleft], _meta[yleft*MetaWidth+xright], xfrac);

		float xorient2 = _linear_interp(_meta[yright*MetaWidth+xleft], _meta[yright*MetaWidth+xright], xfrac);

		float yorient = _linear_interp(xorient1, xorient2, yfrac);

		assert(yorient >= 0.0);
#endif
		return yorient;
	}

private:
	float _meta[MetaWidth*MetaHeight];

	inline float _linear_interp(float left, float right, float portion) const {
		return left * (1 - portion) + right * portion;
	}

};

#endif
