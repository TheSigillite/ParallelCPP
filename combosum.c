#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>

// size of array
#define n 1000000
#define thds 1

// Temporary array for slave process
int a2[500000];

void rmg(int *arr){
    srand(123456789);
    for(int j = 0; j < n; j++){
        arr[j] = rand() % 100;
    }
}

int main(int argc, char* argv[])
{
    double t1, t2;
    int pid, np,
        elements_per_process,
        n_elements_recieved;

    int *a = calloc(n, sizeof(int));
    MPI_Status status;


    MPI_Init(&argc, &argv);


    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    t1 = MPI_Wtime();
    // master
    if (pid == 0) {
        int index, i;
        int *ptr = a;
        rmg(ptr);
        t1 = MPI_Wtime();
        elements_per_process = n / np;


        if (np > 1) {

            for (i = 1; i < np - 1; i++) {
                index = i * elements_per_process;

                MPI_Send(&elements_per_process,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
                MPI_Send(&a[index],
                         elements_per_process,
                         MPI_INT, i, 0,
                         MPI_COMM_WORLD);
            }


            index = i * elements_per_process;
            int elements_left = n - index;

            MPI_Send(&elements_left,
                     1, MPI_INT,
                     i, 0,
                     MPI_COMM_WORLD);
            MPI_Send(&a[index],
                     elements_left,
                     MPI_INT, i, 0,
                     MPI_COMM_WORLD);
        }


        int sum = 0;
        #pragma omp parallel for reduction(+:sum) num_threads(thds)
        for (i = 0; i < elements_per_process; i++){
            if(a[i]>4 && a[i]<8){
                if(a[i] % 2 == 0){
                    sum += a[i];
                }
            }
        }

        int tmp;
        for (i = 1; i < np; i++) {
            MPI_Recv(&tmp, 1, MPI_INT,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;

            sum += tmp;
        }

        printf("Sum of array is : %d\n", sum);
    }
    // slaves
    else {
        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        MPI_Recv(&a2, n_elements_recieved,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        int partial_sum = 0;
        #pragma omp parallel for reduction(+:partial_sum) num_threads(thds)
        for (int i = 0; i < n_elements_recieved; i++){
            if(a2[i]>4 && a2[i]<8){
                if(a2[i] % 2 == 0){
                    partial_sum += a2[i];
                }
            }
        }

        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    printf("Elapsed time : %f\n", t2-t1);
    MPI_Finalize();

    return 0;
}
