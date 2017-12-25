#include "noise1d.hpp"

int main()
{
#if 0
	cout << "[test default seed]" << endl;
	noise1d a;
	a.dump_meta();
#endif

	cout << endl << "[test non default seed]" << endl;
	noise1d b(100);
	b.dump_meta();

#if 0
	cout << endl << "[test small ruler]" << endl;
	float small_ruler[noise1d::MetaNum >> 1];

	for (int i = 0; i < (noise1d::MetaNum >> 1); i++) {
		float scale = (10.0) * i / ((noise1d::MetaNum >> 1));
		cout << "No." << i << " in(" << scale << ") out(" << b.lerp(scale) << ")" << endl;
	}

	cout << endl << "[test big ruler]" << endl;
	float big_ruler[noise1d::MetaNum << 1];

	for (int i = 0; i < (noise1d::MetaNum << 1); i++) {
		float scale = (10.0) * i / ((noise1d::MetaNum << 1) - 1);
		cout << "No.(" << i << ") in(" << scale << ") out(" << b.lerp(scale) << ")" << endl;
	}

	cout << endl << "[test fraction big ruler]" << endl;
	float frac_ruler[33];

	for (int i = 0; i < 33; i++) {
		float scale = (10.0) * i / (33 - 1);
		cout << "No.(" << i << ") in(" << scale << ") out(" << b.lerp(scale) << ")" << endl;
	}
#endif

	cout << endl << "[test period]" << endl;
	cout << b.lerp(-18.1) << endl;
	cout << b.lerp(-8.1) << endl;
	cout << b.lerp(1.9) << endl;

	cout << b.lerp(8.1) << endl;
	cout << b.lerp(18.1) << endl;

	cout << b.lerp(-0.5) << endl;
	cout << b.lerp(9.5) << endl;
	return 0;
}
