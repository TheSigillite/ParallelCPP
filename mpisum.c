#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// size of array
#define n 100000000


// Temporary array for slave process
int a2[50000000];

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
    // np -> no. of processes
    // pid -> process id
    int *a = calloc(n, sizeof(int));
    MPI_Status status;

    // Creation of parallel processes
    MPI_Init(&argc, &argv);

    // find out process ID,
    // and how many processes were started
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

        // musi byc wiecej niz 1 proces
        if (np > 1) {
            // Rozdziel tabele reszcie
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

        // przelicz swoja czesc tabeli
        int sum = 0;
        for (i = 0; i < elements_per_process; i++){
            if(a[i]>4 && a[i]<8){
                if(a[i] % 2 == 0){
                    sum += a[i];
                }
            }
        }

        // zbierz sumy czesciowe
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
    // slave
    else {
        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        // zapisz swoja dzialke w podtablicy
        MPI_Recv(&a2, n_elements_recieved,
                 MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);

        // przelicz swoja dzialke
        int partial_sum = 0;
        for (int i = 0; i < n_elements_recieved; i++){
            if(a2[i]>4 && a2[i]<8){
                if(a2[i] % 2 == 0){
                    partial_sum += a2[i];
                }
            }
        }

        // wyslicz swoja sume do mastera
        MPI_Send(&partial_sum, 1, MPI_INT,
                 0, 0, MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    printf("Elapsed time : %f\n", t2-t1);

    MPI_Finalize();

    return 0;
}
