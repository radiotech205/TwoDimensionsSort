#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;

void TwoDSorting(float* xy, int xy_size, float eps, int *rows, int* rows_size);
void PrintXY(float* xy, int xy_size);
void PrintRows(int* rows, int rows_size);
void PrintXYWithRows(float* xy, int xy_size, int* rows, int rows_size);

int main(int argc, char** argv)
{
    char* file_name = NULL;
    if(argc < 2)    file_name = strdup("../corners.bin");
    else file_name = argv[1];

    float *xy_l, *xy_r;
    int xy_l_size, xy_r_size;

    FILE* file = fopen(file_name, "rb");

    fread(&xy_l_size, sizeof(int), 1, file);
    xy_l = (float*)malloc(2*xy_l_size*sizeof(float));
    for(int i = 0; i < xy_l_size; i++) {
        fread(&xy_l[2*i+0], sizeof(float), 1, file);
        fread(&xy_l[2*i+1], sizeof(float), 1, file);
    }

    fread(&xy_r_size, sizeof(int), 1, file);
    xy_r = (float*)malloc(2*xy_r_size*sizeof(float));
    for(int i = 0; i < xy_r_size; i++) {
        fread(&xy_r[2*i+0], sizeof(float), 1, file);
        fread(&xy_r[2*i+1], sizeof(float), 1, file);
    }

    fclose(file);



    float eps_row = 2.0f;
    int* rows_l = (int*)malloc(2*sizeof(int)*xy_l_size);
    int rows_l_size = 0;

    TwoDSorting(xy_l, xy_l_size, eps_row, rows_l, &rows_l_size);

    printf("xy_l:\n");
    PrintXY(xy_l, xy_l_size);

    printf("rows_l:\n");
    PrintRows(rows_l, rows_l_size);

    printf("xy_l_size = %d, rows_l_size = %d\n", xy_l_size, rows_l_size);


    PrintXYWithRows(xy_l, xy_l_size, rows_l, rows_l_size);

    if(argc < 2)    free(file_name);
    free(xy_l);
    free(xy_r);
    free(rows_l);

    cout << "Hello World!" << endl;
    return 0;
}

int CallbackSortY(const void* xy_a, const void* xy_b) {
    float* p_y_a = (float*)xy_a;
    float* p_y_b = (float*)xy_b;

    float y_a = p_y_a[1];
    float y_b = p_y_b[1];

    if(fabs(y_a - y_b) < 1e-6)
        return 0;
    else if(y_a < y_b)
        return -1;
    else
        return 1;
}

int CallbackSortX(const void* xy_a, const void* xy_b) {
    float* p_x_a = (float*)xy_a;
    float* p_x_b = (float*)xy_b;

    float x_a = p_x_a[0];
    float x_b = p_x_b[0];

    if(fabs(x_a - x_b) < 1e-6)
        return 0;
    else if(x_a < x_b)
        return -1;
    else
        return 1;
}

void TwoDSorting(float* xy, int xy_size, float eps, int* rows, int* rows_size) {
    qsort(xy, xy_size, 2*sizeof(float), CallbackSortY);

    (*rows_size) = 0;
    int mem_cnt = 0;

    float last, now;
    for(int i = 1; i < xy_size; i++) {
        last = xy[2*(i-1)+1];
        now = xy[2*i+1];

        if(fabs(last - now) < eps) {
            mem_cnt++;
        }
        else {
            int index_prev = rows[2*(*rows_size-1) + 0];
            int size_prev = rows[2*(*rows_size-1) + 1];
            int flag_cont = 0;
            for(int j = index_prev; j < (index_prev+size_prev); j++) {
                if(fabs(xy[2*j+1] - now) < eps)
                    flag_cont = 1;
            }
            if(flag_cont)   continue;

            rows[2*(*rows_size) + 0] = i;
            rows[2*(*rows_size) + 1] = mem_cnt+1;
            mem_cnt = 0;
            (*rows_size)++;
        }
    }
    if(mem_cnt) {
        rows[2*(*rows_size) + 0] = rows[2*(*rows_size-1) + 0] + rows[2*(*rows_size-1) + 1];
        rows[2*(*rows_size) + 1] = xy_size - rows[2*(*rows_size) + 0];
        (*rows_size)++;
    }


    for(int i = 0; i < *rows_size; i++) {
        int index = rows[2*i + 0];
        int size = rows[2*i + 1];
        qsort(&xy[2*index], size, 2*sizeof(float), CallbackSortX);
    }
}

void PrintXY(float* xy, int xy_size) {
    for(int i = 0; i < xy_size; i++)
        printf("(%f,%f) ", xy[2*i+0], xy[2*i+1]);
    printf("\n");
}

void PrintRows(int* rows, int rows_size) {
    for(int i = 0; i < rows_size; i++)
        printf("(%d, %d) ", rows[2*i+0], rows[2*i+1]);
    printf("\n");
}

void PrintXYWithRows(float* xy, int xy_size, int* rows, int rows_size) {
    for(int i = 0; i < rows_size; i++) {
        int index = rows[2*i + 0];
        int size = rows[2*i + 1];
        PrintXY(&xy[2*index], size);
        printf("\n");
    }
    printf("\n");
}
