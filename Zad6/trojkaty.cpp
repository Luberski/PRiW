#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <omp.h>
#include <chrono>

struct t_data{
    int x;
    int y;
    int l;
};

const bool task_if = true;

const int N = 4096;
static unsigned char color[N][N][3];
const int LEVEL = 8;

void draw(t_data& t_arg)
{
    int x, y, l;
    x = t_arg.x;
    y = t_arg.y;
    l = t_arg.l;

    int size = (int)N / std::pow(2, l);
    

    for (int iY = 0; iY < size; iY++)
    {
        color[(size + y) - 1][iY + x][0] = 255;
        color[(size + y) - 1][iY + x][1] = 255;
        color[(size + y) - 1][iY + x][2] = 255;

        color[iY + y][iY + x][0] = 255;
        color[iY + y][iY + x][1] = 255;
        color[iY + y][iY + x][2] = 255;
    }

    for (int iX = 0; iX < size; iX++)
    {
            color[iX + y][x][0] = 255;
            color[iX + y][x][1] = 255;
            color[iX + y][x][2] = 255;
    }

    if (l < LEVEL)
    {
        l++;
        int size = N / pow(2, l);
        t_data tab2[3] = {
            {x, y+ size,l},
            {x+size, y+size, l},
            {x,y,l}
        };

        t_data tab_th0 = tab2[0];
        t_data tab_th1 = tab2[1];
        t_data tab_th2 = tab2[2];

        #pragma omp task firstprivate(tab_th0) if(task_if)
        draw(tab_th0);

        #pragma omp task firstprivate(tab_th1) if(task_if)
        draw(tab_th1);

        #pragma omp task firstprivate(tab_th2) if(task_if)
        draw(tab_th2);
    }

    #pragma omp taskwait
}

int main()
{
    using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();

    int size;

    //-------------------------

    const int MaxColorComponentValue = 255;
    FILE *fp;
    char *filename = "new1.ppm";
    char *comment = "# ";
    
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            color[i][j][0] = 0;
            color[i][j][1] = 0;
            color[i][j][2] = 0;
        }
    }

    fp = fopen(filename, "wb");

    //-------------------------

    fprintf(fp, "P6\n %s\n %d\n %d\n %d\n", comment, N, N, MaxColorComponentValue);

    t_data tab = {  0,
                    0,
                    0 };

    omp_set_dynamic(0);
    omp_set_num_threads(4);

    #pragma omp parallel shared(color, N, LEVEL) private(tab)
    {
        #pragma omp single
        draw(tab);
    }

    fwrite(color, 1, 3 * N * N, fp);
    fclose(fp);

    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << ms_double.count() << "ms";

    return 0;
}