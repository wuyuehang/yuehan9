#ifndef NOISE1D_HPP_
#define NOISE1D_HPP_

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>

using std::rand;
using std::cout;
using std::endl;
using std::srand;

class noise1d {
public:
	~noise1d() {}
	noise1d(unsigned int seed = 1900) {
		srand(seed);

		for (unsigned int i = 0; i < MetaNum; i++)
			_meta[i] = float(double(rand()) / RAND_MAX);
	}

	void dump_meta() const {
		for (unsigned int i = 0; i < MetaNum; i++)
			cout << i << ": " << _meta[i] << endl;
	}

	float lerp_positive(const float x) {

		int xleft = int(x);
		float fraction = x - xleft;

		return _cosine_interp(_meta[xleft%MetaNum], _meta[(xleft+1)%MetaNum], fraction);
	}

	float lerp_negative(const float x) {

		int xleft = int(x) - (x != int(x));
		float fraction = x - xleft;

		int lidx = xleft % MetaNum + MetaNum;
		int ridx = (lidx == (MetaNum-1)) ? 0 : (lidx+1);
		return _cosine_interp(_meta[lidx], _meta[ridx], fraction);
	}

	float lerp(const float x) {
		if (x >= 0.0) {
			lerp_positive(x);
		} else {
			lerp_negative(x);
		}
	}

	static const int MetaNum = 10;

private:
	inline float _cosine_interp(float left, float right, float portion) const {
		float t = (1.0 - cos(portion*M_PI))*0.5;
		return left * (1 - t) + right * t;
	}

	inline float _linear_interp(float left, float right, float portion) const {
		return left * (1 - portion) + right * portion;
	}

	float _meta[MetaNum];
};
#endif
