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
float DistCalc(float* xy_a, float* xy_b);
void RowDistCalc(float* xy, int* rows, int num_row, float* xy_dist, int* xy_dist_size);
float ProbabitilyMatcher(float* xy_a_dist, int xy_a_dist_size, float* xy_b_dist, int xy_b_dist_size, float eps_dist);
void PrintFloatX(float* x, int x_size);
void PrintIntX(int* x, int x_size);
int FindIntAtArr(int* arr, int arr_size, int value);

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
    int* rows_r = (int*)malloc(2*sizeof(int)*xy_r_size);
    int rows_l_size = 0;
    int rows_r_size = 0;

    TwoDSorting(xy_l, xy_l_size, eps_row, rows_l, &rows_l_size);
    TwoDSorting(xy_r, xy_r_size, eps_row, rows_r, &rows_r_size);

    printf("rows_l:\n");
    PrintRows(rows_l, rows_l_size);

    printf("rows_r:\n");
    PrintRows(rows_r, rows_r_size);

    PrintXYWithRows(xy_l, xy_l_size, rows_l, rows_l_size);
    PrintXYWithRows(xy_r, xy_r_size, rows_r, rows_r_size);


    //PrintXYWithRows(xy_l, xy_l_size, rows_l, rows_l_size);
    /******************************************************************/
    float eps_dist = 1.5f;
    float* xy_l_dist = (float*)malloc(xy_l_size * sizeof(float));
    float* xy_r_dist = (float*)malloc(xy_r_size * sizeof(float));
    int xy_l_dist_size = 0;
    int xy_r_dist_size = 0;

    int* index_l = (int*)malloc(xy_l_size*sizeof(int));
    int* index_r = (int*)malloc(xy_r_size*sizeof(int));

    int index_l_size = 0;
    int index_r_size = 0;


    for(int i = 0; i < rows_l_size; i++) {
        for(int j = i; j < rows_r_size; j++) {
            if((rows_l[2*i+1] == 1) && (rows_r[2*j+1] == 1)) {
                index_l[index_l_size] = i;
                index_r[index_r_size] = j;
                index_l_size++;
                index_r_size++;
                break;
            }
        }
    }

//    PrintIntX(index_l, index_l_size);
//    PrintIntX(index_r, index_r_size);

    for(int i = 0; i < rows_l_size; i++) {
        for(int j = 0; j < rows_r_size; j++) {
            int result_l = FindIntAtArr(index_l, index_l_size, i);
            int result_r = FindIntAtArr(index_r, index_r_size, j);
            if(result_l && result_r)
                continue;

            RowDistCalc(xy_l, rows_l, i, xy_l_dist, &xy_l_dist_size);
            RowDistCalc(xy_r, rows_r, j, xy_r_dist, &xy_r_dist_size);

            if((xy_l_dist_size == 0) || (xy_r_dist_size == 0))
                continue;

            float prob = ProbabitilyMatcher(xy_l_dist, xy_l_dist_size, xy_r_dist, xy_r_dist_size, eps_dist);
            if(prob > 0.666) {
                printf("(%d,%d): %f\n", i, j, prob);
                int num_memb = rows_l[2*i+1] < rows_r[2*j+1] ? rows_l[2*i+1] : rows_r[2*j+1];

                for(int k = rows_l[2*i+0]; k < (rows_l[2*i+0] + num_memb); k++) {
                    result_l = FindIntAtArr(index_l, index_l_size, k);
                    if(!result_l) {
                        index_l[index_l_size] = k;
                        index_l_size++;
                    }
                }

                for(int k = rows_r[2*j+0]; k < (rows_r[2*j+0] + num_memb); k++) {
                    result_r = FindIntAtArr(index_r, index_r_size, k);
                    if(!result_r) {
                        index_r[index_r_size] = k;
                        index_r_size++;
                    }
                }
                if(index_l_size > index_r_size)
                    index_l_size = index_r_size;
                else if(index_l_size < index_r_size)
                    index_r_size = index_l_size;
            }
        }
    }

    PrintIntX(index_l, index_l_size);
    PrintIntX(index_r, index_r_size);

    for(int i = 0; i < index_l_size; i++) {
        int i_l = index_l[i];
        int i_r = index_r[i];

        printf("(%f, %f) -> (%f, %f)\n", xy_l[2*i_l+0], xy_l[2*i_l+1], xy_r[2*i_r+0], xy_r[2*i_r+1]);
    }

    if(argc < 2)    free(file_name);
    free(xy_l);
    free(xy_r);
    free(rows_l);
    free(rows_r);
    free(xy_l_dist);
    free(xy_r_dist);
    free(index_l);
    free(index_r);

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
        printf("row %d\t", i);
        PrintXY(&xy[2*index], size);
        printf("\n");
    }
    printf("\n");
}

float DistCalc(float* xy_a, float* xy_b) {
    return sqrt(pow(xy_b[0] - xy_a[0], 2.0) + pow(xy_b[1] - xy_a[1], 2.0));
}

void RowDistCalc(float* xy, int* rows, int num_row, float* xy_dist, int* xy_dist_size) {
    int index = rows[2*num_row + 0];
    int size = rows[2*num_row + 1];
    *xy_dist_size = 0;

    for(int i = index; i < (index + size - 1); i += 1) {
        xy_dist[*xy_dist_size] = DistCalc(&xy[2*i+0], &xy[2*(i+1)]);
        (*xy_dist_size)++;
    }
}

float ProbabitilyMatcher(float* xy_a_dist, int xy_a_dist_size, float* xy_b_dist, int xy_b_dist_size, float eps_dist) {
    int match = 0;
    int match_cnt = (xy_a_dist_size > xy_b_dist_size) ? xy_a_dist_size : xy_b_dist_size;

    for(int i = 0; i < xy_a_dist_size; i++) {
        for(int j = i; j < xy_b_dist_size; j++) {
            if(fabs(xy_a_dist[i] - xy_b_dist[j]) < eps_dist)
                match++;
        }
    }
    if(match)
        return (float)match/(float)match_cnt;
    else
        return 0.0f;
}

void PrintFloatX(float* x, int x_size) {
    for(int i = 0; i < x_size; i++)
        printf("%f ", x[i]);
    printf("\n");
}

void PrintIntX(int* x, int x_size) {
    for(int i = 0; i < x_size; i++)
        printf("%d ", x[i]);
    printf("\n");
}

int FindIntAtArr(int* arr, int arr_size, int value) {
    for(int i = 0; i < arr_size; i++)
        if(arr[i] == value)
            return 1;
    return 0;
}
