#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <chrono>

#define T 4

using namespace std;

const int iXmax = 10000;
const int iYmax = 10000;
const double CxMin = -2.5;
const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;
const int IterationMax = 200;
const double EscapeRadius = 2;
unsigned char color[iYmax][iXmax][3];


void * Multiply(void * threadid) {
    long tid;
    int i, j, k;
    tid = (long) threadid;

    int lb = tid * iXmax / T;
    int ub = lb + iXmax / T - 1;

    int iX, iY;
    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
    int Iteration;
    double PixelWidth = (CxMax - CxMin) / iXmax;
    double PixelHeight = (CyMax - CyMin) / iYmax;
    double ER2 = EscapeRadius * EscapeRadius;

    cout << "Worker: no." << tid << "<" << lb << "-" << ub << endl;
    // Multiplying matrix a and b and storing in array c.
    // for(i = lb; i <= ub; ++i) {
    //     for(j = 0; j < N; ++j) {
    //         for(k = 0; k < N; ++k)
    //         {
    //             c[i][j] += a[i][k] * b[k][j];
    //         }
    //     }
    // }

    for (iY = lb; iY < ub; iY++) {
        Cy = CyMin + iY * PixelHeight;
        if (fabs(Cy) < PixelHeight / 2) Cy = 0.0; /* Main antenna */
        for (iX = 0; iX < iXmax; iX++) {
            Cx = CxMin + iX * PixelWidth;
            /* initial value of orbit = critical point Z= 0 */
            Zx = 0.0;
            Zy = 0.0;
            Zx2 = Zx * Zx;
            Zy2 = Zy * Zy;
            /* */
            for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++) {
                Zy = 2 * Zx * Zy + Cy;
                Zx = Zx2 - Zy2 + Cx;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
            };
            /* compute  pixel color (24 bit = 3 bytes) */
            if (Iteration == IterationMax) {
                /*  interior of Mandelbrot set = black */
                color[iY][iX][0] = 0;
                color[iY][iX][1] = 0;
                color[iY][iX][2] = 0;
            } else {
                /* exterior of Mandelbrot set = white */
                color[iY][iX][0] = 255-(tid*30); /* Red*/
                color[iY][iX][1] = (tid*30); /* Green */
                color[iY][iX][2] = 255-(tid*30); /* Blue */
            };
            /*write color to the file*/

        }
    }

    pthread_exit(NULL);
}

int main() {
    using chrono::high_resolution_clock;
	using chrono::duration_cast;
	using chrono::duration;
	using chrono::milliseconds;


    auto t1 = high_resolution_clock::now();

    pthread_t threads[T];

    const int MaxColorComponentValue = 255;
    FILE * fp;
    char * filename = "new1.ppm";
    char * comment = "# "; /* comment should start with # */

    /* Z=Zx+Zy*i  ;   Z0 = 0 */
    
    /*  */
    
    /*create new file,give it a name and open it in binary mode  */
    fp = fopen(filename, "wb"); /* b -  binary mode */
    /*write ASCII header to the file*/
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);
    /* compute and write image data bytes to the file*/

    for(int t=0; t<T; t++) {
      pthread_create(&threads[t], NULL, Multiply, (void *)t);
    }

    for(int t=0; t<T; t++) {
      pthread_join(threads[t], NULL);
    }

    auto t2 = high_resolution_clock::now();
    duration<double, milli> ms_double = t2 - t1;
	cout << ms_double.count() << "s";

    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);

    

    

    return 0;
}