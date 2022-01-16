#include <stdio.h>
#include <iostream>
#include <math.h>
#include <omp.h>

using namespace std;

const int iXmax     = 2000;
const int iYmax     = 2000;
const double CxMin  = -2.5;
const double CxMax  = 1.5;
const double CyMin  = -2.0;
const double CyMax  = 2.0;
const int IterationMax      = 200;
const double EscapeRadius   = 2;
const int MaxColorComponentValue = 255;
unsigned char color[iYmax][iXmax][3];

void set_color(int id, int iY, int iX, bool is_exterior) {
    if(id > 6) id = id-6;

    switch(id) {
        case 0:
            color[iY][iX][0] = 255;
            color[iY][iX][1] = 255;
            color[iY][iX][2] = 0 + (is_exterior*100);
            break;

        case 1:
            color[iY][iX][0] = 0 + (is_exterior*100);
            color[iY][iX][1] = 0 + (is_exterior*100);
            color[iY][iX][2] = 255;
            break;

        case 2:
            color[iY][iX][0] = 0 + (is_exterior*100);
            color[iY][iX][1] = 255;
            color[iY][iX][2] = 0 + (is_exterior*100);
            break;

        case 3:
            color[iY][iX][0] = 255;
            color[iY][iX][1] = 0 + (is_exterior*100);
            color[iY][iX][2] = 0 + (is_exterior*100);
            break;

        case 4:
            color[iY][iX][0] = 108;
            color[iY][iX][1] = 0 + (is_exterior*100);
            color[iY][iX][2] = 158 + (is_exterior*100);
            break;
        
        case 5:
            color[iY][iX][0] = 145;
            color[iY][iX][1] = 0 + (is_exterior*100);
            color[iY][iX][2] = 104 + (is_exterior*100);
            break;
        }
}

int main(int argc, char *argv[]) // run with: g++ -lgomp -fopenmp mopenmp.cpp -O3
{

    int THREADS;
    if(argc == 2) {
        THREADS = stoi(argv[1]);
    }
    else {
        std::cout << "usage: a.out <num_of_threads>" << std::endl;
        exit(1);
    }
    

    int iX, iY;
    double Cx, Cy;
    double Zx, Zy;
    double Zx2, Zy2;
    long long int Iteration;
    double PixelWidth = (CxMax - CxMin) / iXmax;
    double PixelHeight = (CyMax - CyMin) / iYmax;
    double ER2 = EscapeRadius * EscapeRadius;
    int id;

    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    fp = fopen(filename, "wb");
    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, iXmax, iYmax, MaxColorComponentValue);
    long long int iter[THREADS] = {0};
    omp_set_num_threads(THREADS);

    double t_start = omp_get_wtime();

#pragma omp parallel private(id)
    {
        float start, end;
        id = omp_get_thread_num();
        start = omp_get_wtime();

#pragma omp for //schedule(static, 10) nowait
        for (iY = 0; iY < iYmax; iY++)
        {
            Cy = CyMin + iY * PixelHeight;
            if (fabs(Cy) < PixelHeight / 2)
                Cy = 0.0; /* Main antenna */
            for (iX = 0; iX < iXmax; iX++)
            {
                Cx = CxMin + iX * PixelWidth;
                
                Zx = 0.0;
                Zy = 0.0;
                Zx2 = Zx * Zx;
                Zy2 = Zy * Zy;
                
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
                    set_color(id, iY, iX, 0);   
                }
                else
                {
                    set_color(id, iY, iX, 1);
                };
                /*write color to the file*/
            }
        }

        end = omp_get_wtime();
        printf("Thread id: %d Work took %f s\n", id, (end - start));
    }

    double t_end = omp_get_wtime();
    printf("Total time: %f s\n", (t_end - t_start));

    for(long long int i = 0; i < THREADS; i++) {
        std::cout << "Thread " << i << " " << iter[i] << " iterations" << std::endl;
    }
    
    fwrite(color, 1, 3 * iYmax * iXmax, fp);
    fclose(fp);

    

    return 0;
}