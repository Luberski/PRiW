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
const int MaxColorComponentValue = 255;
unsigned char color[iYmax][iXmax][3];

int main()
{
    int iX, iY;
    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2;
    int Iteration;
    double PixelWidth = (CxMax - CxMin) / iXmax;
    double PixelHeight = (CyMax - CyMin) / iYmax;
    double ER2 = EscapeRadius * EscapeRadius;
    int id;

    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);
    int iter[4] = {0};
    omp_set_num_threads(T);
#pragma omp parallel private(id)
    {
        float start, end;
        start = omp_get_wtime();
        id = omp_get_thread_num();

#pragma omp for schedule(static, 10) nowait
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
                iter[id] += Iteration;
                /* compute  pixel color (24 bit = 3 bytes) */
                if (Iteration == IterationMax)
                {
                    /*  interior of Mandelbrot set = black */
                    switch(id) {
                        case 0:
                            color[iY][iX][0] = 180;
                            color[iY][iX][1] = 0;
                            color[iY][iX][2] = 255;
                            break;

                        case 1:
                            color[iY][iX][0] = 0;
                            color[iY][iX][1] = 50;
                            color[iY][iX][2] = 255;
                            break;

                        case 2:
                            color[iY][iX][0] = 0;
                            color[iY][iX][1] = 255;
                            color[iY][iX][2] = 70;
                            break;

                        case 3:
                            color[iY][iX][0] = 255;
                            color[iY][iX][1] = 0;
                            color[iY][iX][2] = 0;
                            break;
                    }
                    
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
        end = omp_get_wtime();
        printf("Work took %d ms\n", int((end - start) * 1000));
    }

    for(int i = 0; i < 4; i++) {
        std::cout << iter[i] << std::endl;
    }

    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);

    return 0;
}