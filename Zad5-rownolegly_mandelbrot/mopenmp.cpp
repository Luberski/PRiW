#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <chrono>
#include <omp.h>

#define T 6

using namespace std;

const int iXmax = 5000;
const int iYmax = 5000;
const double CxMin = -2.5;
const double CxMax = 1.5;
const double CyMin = -2.0;
const double CyMax = 2.0;
const int IterationMax = 200;
const double EscapeRadius = 2;
unsigned char color[iYmax][iXmax][3];

void *Multiply(void *threadid)
{
    long tid;
    int i, j, k;
    tid = (long)threadid;

    int lb = tid * iXmax / T;
    int ub = lb + iXmax / T - 1;

    cout << "Worker: no." << tid << "<" << lb << "-" << ub << endl;

    pthread_exit(NULL);
}

int main()
{
    int id;

    const int MaxColorComponentValue = 255;
    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb"); /* b -  binary mode */
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);

    //

    //
omp_set_num_threads(T);
#pragma omp parallel private(id)
    {
        id = omp_get_thread_num();
        int iX, iY;
        double Cx, Cy;
        double Zx, Zy;
        double Zx2, Zy2; /* Zx2=Zx*Zx;  Zy2=Zy*Zy  */
        int Iteration;
        double PixelWidth = (CxMax - CxMin) / iXmax;
        double PixelHeight = (CyMax - CyMin) / iYmax;
        double ER2 = EscapeRadius * EscapeRadius;

#pragma omp for
            for (iY = 0; iY < iYmax; iY++)
            {
                Cy = CyMin + iY * PixelHeight;
                if (fabs(Cy) < PixelHeight / 2)
                    Cy = 0.0; /* Main antenna */
                for (iX = 0; iX < iXmax; iX++)
                {
                    Cx = CxMin + iX * PixelWidth;
                    /* initial value of orbit = critical point Z= 0 */
                    Zx = 0.0;
                    Zy = 0.0;
                    Zx2 = Zx * Zx;
                    Zy2 = Zy * Zy;
                    /* */
                    for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
                    {
                        Zy = 2 * Zx * Zy + Cy;
                        Zx = Zx2 - Zy2 + Cx;
                        Zx2 = Zx * Zx;
                        Zy2 = Zy * Zy;
                    };
                    /* compute  pixel color (24 bit = 3 bytes) */
                    if (Iteration == IterationMax)
                    {
                        /*  interior of Mandelbrot set = black */
                        color[iY][iX][0] = 0;
                        color[iY][iX][1] = 0;
                        color[iY][iX][2] = 0;
                    }
                    else
                    {
                        /* exterior of Mandelbrot set = white */
                        color[iY][iX][0] = 255; /* Red*/
                        color[iY][iX][1] = 255; /* Green */
                        color[iY][iX][2] = 255; /* Blue */
                    };
                    /*write color to the file*/
                }
            }
        }

    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);

    return 0;
}