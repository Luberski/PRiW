#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <chrono>
#include <omp.h>
#include <cmath>

using namespace std;

const int size = 1024 * 2;
char pixels[size][size][3];

class Lapunov
{
public:
	char Sequence[16];
	const int noColors = 255;
	float MinR, MaxR, MinX, MaxX;
	char NoSeq = 16;
	float LapMin = -2;
	int sample;

	int DecToBin(int Dec)
	{
		int pow;
		char rest;
		pow = 65536;
		rest = 0;

		while (rest == 0 && pow > 0)
		{
			rest = (char)floor(Dec / pow);
			if (rest == 0)
				pow = floor(pow / 2);
		}
		while (pow > 1)
		{
			Dec = Dec - pow * rest;
			pow = floor(pow / 2);
			rest = (char)floor(Dec / pow);
			NoSeq++;
			cout << NoSeq;
			Sequence[NoSeq] = rest;
		}
		cout << '\n';
		return 0;
	}
	/*void DecToBin(int Dec){
		  int i;
		  for(i=0; i<16; i++){
		     Sequence[i] = Dec >> i & 1;

		  }
		}*/

	void Draw(unsigned int Seed, int NoIter, int RozX, int RozY, int start, int koniec, float RXMin, float RXMax, float RYMin, float RYMax, int val, int s)
	{
		float rx, ry, deltaX, deltaY, tmpLap = 0;
		int k, w;
		char tmp;

		for (k = 0; k < 16; k++)
			Sequence[k] = 0;

		sample = s;
		NoSeq = 0;
		Sequence[0] = 1;
		DecToBin(val);
		LapMin = -2;
		MinR = RXMin;
		MaxR = RXMax;
		MinX = RYMin;
		MaxX = RYMax;
		deltaX = (MaxR - MinR) / RozX;
		deltaY = (MaxX - MinX) / RozY;
		rx = MinR;
		int ry_org = MaxX - (start - 1) * deltaY;
		ry = ry_org;
		double z;
		
#pragma omp parallel for private(w, k)
		for (w = 0; w < RozY; w++)
		{
			for (k = 0; k < RozX; k++)
			{

				rx = MinR + k * deltaX;
				ry = ry_org - w * deltaY;
				tmpLap = ValLap(Seed, NoIter, rx, ry);

				//std::cout << (int)tmpLap;
				if (tmpLap <= 0)
				{

					z = noColors * tmpLap / LapMin;
					tmp = (int)(z) % noColors;
					pixels[k][w][0] = tmp;
					pixels[k][w][1] = tmp;
					pixels[k][w][2] = tmp;
				}
				else
				{
					pixels[k][w][0] = 0;
					pixels[k][w][1] = 0;
					pixels[k][w][2] = 0;
				}
			}
		}
	}

	float ValLap(unsigned int Seed, int NoIter, float rx, float ry)
	{

		float x, sumlap, elem, ValLap;
		int i, poz, NoElem;
		float R;

		// x = rand();
		x = rand_r(&Seed);
		// cout << x << endl;
		sumlap = 0;
		NoElem = 0;
		poz = 0;

		for (i = 1; i <= NoIter; i++)
		{
			if (Sequence[poz] == 0)
				R = ry;
			else
				R = rx;
			poz++;
			if (poz > NoSeq)
				poz = 0;
			x = fun(x, R);
			elem = (float)abs(dfun(x, R));
			if (elem > 1000)
			{
				ValLap = 10;
				break;
			}
			if (elem != 0)
			{
				sumlap = sumlap + (float)log2(elem);
				NoElem++;
			}
		}
		if (NoElem > 0)
			ValLap = sumlap / NoElem;
		else
			ValLap = 0;
		return ValLap;
	}

	float fun(float x, float r)
	{
		float y = 0;
		switch (sample)
		{
		case (0):
			y = r * sin(x) + r;
			break;
		case (1):
			y = r * cos(x) + r;
			break;
		case (2):
			y = r * cos(x) * (1 - sin(x));
			break;
		}
		return y;
	}

	float dfun(float x, float r)
	{
		float y = 0;
		switch (sample)
		{
		case (0):
			y = r * cos(x);
			break;
		case (1):
			y = -r * sin(x);
			break;
		case (2):
			y = r * (1 - sin(x)) - 2 * cos(x) * cos(x);
			break;
		}
		return y;
	}
};

int main(int argc, char *argv[])
{

	if (argc != 2)
	{
		fprintf(stderr, "%s <number of threads>\n", argv[0], argv[1]);
		return -1;
	}

	int NUM_THREADS = atoi(argv[1]);
	omp_set_num_threads(NUM_THREADS);
	FILE *fp;
	int i, j;
	char *filename = "malysz.ppm";
	char *comment = "# "; /* comment should start with # */
	const int MaxColorComponentValue = 255;
	int a;
	Lapunov lp;
	fp = fopen(filename, "wb"); /* b -  binary mode */
	/*write ASCII header to the file*/

	fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, size, size, MaxColorComponentValue);

	unsigned int seed;
	srand(seed);
	auto start = omp_get_wtime();
	lp.Draw(seed, 100, size, size, 0, size, -3, 9, -5, 2, 2477, 1);

	std::cout << "Draw time: " << (omp_get_wtime() - start) << "[s]" << std::endl;

	fwrite(pixels, 1, 3 * size * size, fp);

	fclose(fp);

	return 0;
}