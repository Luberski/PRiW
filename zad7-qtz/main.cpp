#include <stdio.h>
#include <iostream>
#include <omp.h>
#include <cstdio>

int main(int argc, char *argv[]) // run with: g++ -lgomp -fopenmp mopenmp.cpp -O3
{

    int THREADS;
    if(argc == 2) {
        THREADS = std::stoi(argv[1]);
    }
    else {
        std::cout << "usage: a.out <num_of_threads>" << std::endl;
        exit(1);
    }


    FILE * fp;
    FILE * fp1;
    FILE * fpOut;
    int iXmax;
    int iYmax;
    int MaxColorComponentValue;
    char p[3];

    fp = fopen ("pudzian.ppm","rb");
    fp1 = fopen ("pudzian1.ppm","rb");
    fscanf(fp, "%s\n%d\n%d\n%d\n", p, &iXmax, &iYmax, &MaxColorComponentValue);

    unsigned char* colorR = new unsigned char[iXmax*iYmax*3];
    unsigned char* colorL = new unsigned char[iXmax*iYmax*3];
    unsigned char* outColor1 = new unsigned char[iXmax*iYmax*3];
    unsigned char* outColor2 = new unsigned char[iXmax*iYmax*3];
    unsigned char* outColor3 = new unsigned char[iXmax*iYmax*3];
    unsigned char* outColor4 = new unsigned char[iXmax*iYmax*3];
    unsigned char* outColor5 = new unsigned char[iXmax*iYmax*3];

    unsigned char ch;
    int i = 0;   // grey średnia z pixeli r+g+b/3
    while (fscanf(fp, "%c", &ch) == 1) { 
        colorR[i] = ch;
        i++;
    }

    i = 0;
    while (fscanf(fp1, "%c", &ch) == 1) { 
        colorL[i] = ch;
        i++;
    }

    fclose(fp);
    fclose(fp1);

    i = 0;
    int color_meanR;
    int color_meanL;

    omp_set_num_threads(THREADS);
    double t_start = omp_get_wtime();
    #pragma omp parallel
        {
            #pragma omp for
            for (int i = 0; i < iYmax*iXmax*3; i+=3) { // Red on left, blue on right.
                outColor1[i] = colorL[i];
                outColor1[i+1] = 0;
                outColor1[i+2] = colorR[i+2];
            }

            #pragma omp for
            for (int i = 0; i < iYmax*iXmax*3; i+=3) { // Grey red/cyan
                color_meanR = (colorR[i] + colorR[i+1] + colorR[i+2])/3;
                color_meanL = (colorL[i] + colorL[i+1] + colorL[i+2])/3;
                outColor2[i] = color_meanL;
                outColor2[i+1] = color_meanR;
                outColor2[i+2] = color_meanR;
            }

            #pragma omp for
            for (int i = 0; i < iYmax*iXmax*3; i+=3) { // Colour
                outColor3[i] = colorL[i];
                outColor3[i+1] = (colorR[i+1]+colorL[i+1])/2;
                outColor3[i+2] = colorR[i+2];
            }

            #pragma omp for
            for (int i = 0; i < iYmax*iXmax*3; i+=3) { // Red/Cyan
                outColor4[i] = colorL[i];
                outColor4[i+1] = colorR[i+1];
                outColor4[i+2] = colorR[i+2];
            }

            #pragma omp for
            for (int i = 0; i < iYmax*iXmax*3; i+=3) { // Grey
                color_meanR = (colorR[i] + colorR[i+1] + colorR[i+2])/3;
                color_meanL = (colorL[i] + colorL[i+1] + colorL[i+2])/3;
                outColor5[i] = color_meanL;
                outColor5[i+1] = (color_meanL+color_meanR)/2;
                outColor5[i+2] = color_meanR;
            }

        }

    double t_end = omp_get_wtime();
    printf("Total time: %f s\n", (t_end - t_start));

    fpOut = fopen("out1.ppm", "wb");
    fprintf(fpOut, "P6\n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    printf("%d %d %d", iXmax, iYmax, MaxColorComponentValue);

    fwrite(outColor1, 1, 3 * iYmax * iXmax, fpOut);
    fclose(fpOut);

    fpOut = fopen("out2.ppm", "wb");
    fprintf(fpOut, "P6\n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    printf("%d %d %d", iXmax, iYmax, MaxColorComponentValue);

    fwrite(outColor2, 1, 3 * iYmax * iXmax, fpOut);
    fclose(fpOut);

    fpOut = fopen("out3.ppm", "wb");
    fprintf(fpOut, "P6\n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    printf("%d %d %d", iXmax, iYmax, MaxColorComponentValue);

    fwrite(outColor3, 1, 3 * iYmax * iXmax, fpOut);
    fclose(fpOut);

    fpOut = fopen("out4.ppm", "wb");
    fprintf(fpOut, "P6\n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    printf("%d %d %d", iXmax, iYmax, MaxColorComponentValue);

    fwrite(outColor4, 1, 3 * iYmax * iXmax, fpOut);
    fclose(fpOut);

    fpOut = fopen("out5.ppm", "wb");
    fprintf(fpOut, "P6\n%d\n%d\n%d\n", iXmax, iYmax, MaxColorComponentValue);
    printf("%d %d %d", iXmax, iYmax, MaxColorComponentValue);

    fwrite(outColor5, 1, 3 * iYmax * iXmax, fpOut);
    fclose(fpOut);

    delete[] colorL;
    delete[] colorR;
    delete[] outColor1;
    delete[] outColor2;
    delete[] outColor3;
    delete[] outColor4;
    delete[] outColor5;
}