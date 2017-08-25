#include <fstream>
#include <iostream>
#include <ctime>
#include <sys/time.h>

using std::cout;
using std::endl;
using std::ofstream;
using std::srand;
using std::rand;
using std::time;

int *datum = nullptr;

#define W	2048
#define H	2048
#define LINE_NUM 100000
#define REPEAT 50
#define TEST_FUNCTIONAL 1

void ppm_ascii_writer(const char *name, int w, int h)
{
	ofstream pfile;
	pfile.open(name);

	pfile << "P3" << endl;
	pfile << w << " " << h << endl;
	pfile << "255" << endl;

	for (int i = 0; i < h; i++) {
		for (int j  = 0; j < w; j++) {
			pfile << datum[i*w*3+j*3] << " " << datum[i*w*3+j*3+1] << " " << datum[i*w*3+j*3+2] << " ";
		}
		pfile << endl;
	}

	pfile.close();

	return;
}

bool xmajor(int headx, int heady, int endx, int endy)
{
	return (abs(headx - endx) >= abs(heady - endy));
}

// choose which implementation for line rasterization backup
#if 1
#define NAIVE naive_opt
#else
#define NAIVE naive
#endif

// a small optimization by extract some common factors
void naive_opt(int headx, int heady, int endx, int endy)
{
	int dx = headx - endx;
	int dy = heady - endy;
	int adx = abs(dx);
	int ady = abs(dy);

	if (adx >= ady) {
			float slope = dy * 1.0 / dx;
			int startx;
			int step = adx;
			float starty;

			if (dx > 0) {
				startx = endx;
				starty = endy;
			} else {
				startx = headx;
				starty = heady;
			}

			for (int i = 1; i < step; i++) {
				startx += 1;
				starty += slope;
				datum[startx*3 + int(starty)*3*W] = 255;
			}
	} else {
		float slope = dx * 1.0 / dy;
		int starty;
		int step = ady;
		float startx;

		if (dy > 0) {
			starty = endy;
			startx = endx;
		} else {
			starty = heady;
			startx = headx;
		}

		for (int i = 1; i < step; i++) {
			starty += 1;
			startx += slope;
			datum[int(startx)*3 + starty*3*W + 2] = 255;
		}
	}
}

void naive(int headx, int heady, int endx, int endy)
{
	if (xmajor(headx, heady, endx, endy)) {
			float slope = (heady - endy) * 1.0 / (headx - endx);
			int steps = abs(headx - endx);
			int startx;
			float starty;

			if (headx > endx) {
				startx = endx;
				starty = endy;
			} else {
				startx = headx;
				starty = heady;
			}

			for (int i = 1; i < steps; i++) {
				startx += 1;
				starty += slope;
				datum[startx*3 + int(starty)*3*W] = 255;
			}
	} else {
		float slope = (headx - endx) * 1.0 / (heady - endy);
		int steps = abs(heady - endy);
		int starty;
		float startx;

		if (heady > endy) {
			starty = endy;
			startx = endx;
		} else {
			starty = heady;
			startx = headx;
		}

		for (int i = 1; i < steps; i++) {
			starty += 1;
			startx += slope;
			datum[int(startx)*3 + starty*3*W + 2] = 255;
		}
	}
}

int *line_segments_generation()
{
	int *lines = new int[LINE_NUM*4];

	srand(time(0));

	for (int i = 0; i < LINE_NUM; i++) {
		lines[4*i] = rand() % W;
		lines[4*i + 1] = rand() % H;
		lines[4*i + 2] = rand() % W;
		lines[4*i + 3] = rand() % H;
	}

	return lines;
}

int main()
{
	datum = new int[W*H*3];

#if TEST_FUNCTIONAL
	NAIVE(0, 0, W, H);
	NAIVE(0, 0, W, H >> 1);
	NAIVE(0, 0, W, H >> 2);
	NAIVE(0, 0, W, H >> 3);

	NAIVE(0, 0, W >> 1, H);
	NAIVE(0, 0, W >> 2, H);
	NAIVE(0, 0, W >> 3, H);

	// write out
	ppm_ascii_writer("alias_line.ppm", W, H);

	delete [] datum;
#else

	int *lines = line_segments_generation();

	struct timeval t;
	long elapsed[2];

	// naive_opt version
	gettimeofday(&t, nullptr);
	elapsed[0] = t.tv_sec * 1000 + t.tv_usec / 1000;

	for (int k = 0; k < REPEAT; k++) {
		for (int i = 0; i < LINE_NUM; i++) {
			naive_opt(lines[4*i], lines[4*i+1], lines[4*i+2], lines[4*i+3]);
		}
	}

	gettimeofday(&t, nullptr);
	elapsed[0] = t.tv_sec * 1000 + t.tv_usec / 1000 - elapsed[0];
	cout << "[naive_opt version] average " << elapsed[0] * 1.0 / REPEAT  << " milliseconds" << endl;

	// naive version
	gettimeofday(&t, nullptr);
	elapsed[1] = t.tv_sec * 1000 + t.tv_usec / 1000;

	for (int k = 0; k < REPEAT; k++) {
		for (int i = 0; i < LINE_NUM; i++) {
			naive(lines[4*i], lines[4*i+1], lines[4*i+2], lines[4*i+3]);
		}
	}

	gettimeofday(&t, nullptr);
	elapsed[1] = t.tv_sec * 1000 + t.tv_usec / 1000 - elapsed[1];
	cout << "[naive version] average " << elapsed[1] * 1.0 / REPEAT << " milliseconds" << endl;

	cout << "#### improve by " << (elapsed[1] - elapsed[0]) * 1.0 / REPEAT << " milliseconds" << endl;
	delete [] lines;
	delete [] datum;
#endif

	return 0;
}
