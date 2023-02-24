#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define n 1000000


void rmg(int *arr){
    srand(123456789);
    for(int j = 0; j < n; j++){
        arr[j] = rand() % 100;
    }
}

int main()
{
    time_t t1, t2;
    int *a = calloc(n, sizeof(int));
    int partial_sum = 0;
    rmg(a);
    t1 = clock();
    for(int i = 0; i<n;i++){
        if(a[i]>4 && a[i]<8){
                if(a[i] % 2 == 0){
                    partial_sum += a[i];
                }
            }
    }
    t2 = clock();
    double exectime = (double)(t2-t1) / CLOCKS_PER_SEC;
    printf("Sum of array is : %d\n", partial_sum);
    printf("Elapsed time : %f\n", exectime);
    return 0;
}
