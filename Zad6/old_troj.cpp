#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <chrono>

struct t_data{
    int x;
    int y;
    int l;
};

const int N = 4096;
static unsigned char color[N][N][3];
const int LEVEL = 8;

void *draw(void *t_arg)
{
    struct t_data* data;
    data = (struct t_data*) t_arg;
    
    int x, y, l;
    // std::cout << "x: " << " y: " << " l: " << l << std::endl;
    x = data->x;
    y = data->y;
    l = data->l;

    // std::cout << "x: " << x << " y: " << y << " l: " << l << std::endl;

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

    pthread_t threads[3];

    if (l < LEVEL)
    {
        l++;
        int size = N / pow(2, l);
        t_data tab2[3] = {
            {x, y+ size,l},
            {x+size, y+size, l},
            {x,y,l}
        };

        for(int t=0; t<3; t++)
        {
            pthread_create(&threads[t], NULL, draw, (void *)&tab2[t]);
        }

        for (int t = 0; t < 3; t++)
        {
            pthread_join(threads[t], NULL);
        }
    }

    pthread_exit(NULL);
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
    pthread_t threads;

    int tab[3] = { 0,
                   0,
                   0 };

    pthread_create(&threads, NULL, draw, (void *)&tab);
    pthread_join(threads, NULL);

    fwrite(color, 1, 3 * N * N, fp);
    fclose(fp);

    auto t2 = high_resolution_clock::now();

    duration<double, std::milli> ms_double = t2 - t1;
	std::cout << ms_double.count() << "ms";

    return 0;
}