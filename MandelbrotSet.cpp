#include <iostream>
#include <fstream>
#include <thread>
using namespace std;

const char mandelbrot(const float& re, const float& im, const int& I)
{
	float z_re = 0, z_im = 0;
	char notOOB = 49;
	for (int i = 0; i < I; i++)
	{
		const float z_re_temp = (z_re * z_re - z_im * z_im) + re;
		const float z_im_temp = (2 * z_re * z_im) + im;
		if (abs(z_re_temp) > 2 || abs(z_im_temp) > 2)
		{
			notOOB = 48;
			break;
		}
		z_re = z_re_temp;
		z_im = z_im_temp;
	}
	return notOOB;
}

void mandelbrot_thread(const int& START, const int& END, const int& H, const int& W, char** MSET, const int& H_offset, const int& W_offset, const int& I, const float& P)
{
	for (int im = START; im < END; im++)
	{
		const float im_f = (im - H_offset) * P;
		for (int re = 0; re < W; re++)
		{
			const float re_f = (re - W_offset) * P;
			MSET[H - 1 - im][re] = mandelbrot(re_f, im_f, I);
		}
	}
}

int main()
{
	const int H = 20000, W = 30000, I = 120;
	const int H_offset = 10000, W_offset = 20000;
	const float P = 0.0001;

	cout << "Allocating Mandelbrot Array\n";

	char** MSET = new char*[H];

	for (int i = 0; i < H; i++)
	{
		MSET[i] = new char[W];
	}

	cout << "Mapping Mandelbrot Booleans onto Mandelbrot Array\n";

	const int threadsize = thread::hardware_concurrency() / 2;
	thread* threadarr = new thread[threadsize];

	for (int i = 0; i < threadsize; i++)
	{
		const int start = (H * i) / threadsize;
		const int end = i == threadsize - 1 ? H : (H * (i + 1)) / threadsize;
		threadarr[i] = thread(mandelbrot_thread, start, end, H, W, MSET, H_offset, W_offset, I, P);
	}

	for (int i = 0; i < threadsize; i++)
	{
		threadarr[i].join();
	}

	delete[] threadarr;
	threadarr = nullptr;

	cout << "Forming PBM from Mandelbrot Array\n";
	
	ofstream ostr("mandelbrotset.pbm");

	ostr << "P1\n";
	ostr << W << " " << H << "\n";

	for (int i = 0; i < H; i++)
	{
		ostr.write(MSET[i], W);
	}

	ostr.close();

	cout << "Deallocating Mandelbrot Array\n";

	for (int i = 0; i < H; i++)
	{
		delete[] MSET[i];
		MSET[i] = nullptr;
	}

	delete[] MSET;
	MSET = nullptr;

	char exit;
	cout << "\nDone! Press any key then Enter to exit.\n";
	cin >> exit;

	return 0;
}
