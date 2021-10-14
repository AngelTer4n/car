#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>

#define MAX_ORDER 512

typedef float LOCAL_MATRIX_T[MAX_ORDER][MAX_ORDER];

int main(int argc, char* argv[]) {
    int             my_rank;
    int             p;
    LOCAL_MATRIX_T  local_A; 
    LOCAL_MATRIX_T  local_B; //!
    LOCAL_MATRIX_T  global_B; 
    LOCAL_MATRIX_T  local_Q; //!
    LOCAL_MATRIX_T  local_C; //!
    int             n; //!
    int             local_n; //!

    srand((unsigned int)time(NULL)); //!

    void Read_matrix(char* prompt, LOCAL_MATRIX_T local_Q, int local_n, int n,
             int my_rank, int p);

    void Parallel_matrix_prod( LOCAL_MATRIX_T local_A, int n, LOCAL_MATRIX_T local_B, 
             LOCAL_MATRIX_T global_B, LOCAL_MATRIX_T local_C, int local_n);
    void Print_matrix(char* title, LOCAL_MATRIX_T local_Q, int local_n,
             int n, int my_rank, int p);
 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        n = 512;
        //printf("Enter the order of the matrix (n x n)\n");
        //scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_n = n/p;

    Read_matrix("Enter the matrix", local_A, local_n, n, my_rank, p);
    //Print_matrix("We read", local_A, local_n, n, my_rank, p);

    Read_matrix("Enter the matrix", local_B, local_n, n, my_rank, p); //!
    //Print_matrix("We read", local_B, local_n, n, my_rank, p); //!

    Parallel_matrix_prod(local_A, n, local_B, global_B, local_C, local_n); //!
    //Print_matrix("The product is", local_C, local_n, n, my_rank, p);

    MPI_Finalize();

}  /* main */


/**********************************************************************/
void Read_matrix(
         char*           prompt   /* in  */, 
         LOCAL_MATRIX_T  local_Q  /* out */, 
         int             local_n  /* in  */, 
         int             n        /* in  */,
         int             my_rank  /* in  */, 
         int             p        /* in  */) {

    int   i, j, q;
    float a = 5.0; //!
    LOCAL_MATRIX_T  temp;

    MPI_Status status; // Add
    MPI_Datatype column_mpi_t; // Add

    MPI_Type_vector(local_n, 1, n, MPI_FLOAT, &column_mpi_t); //Add and mod
    MPI_Type_commit(&column_mpi_t); // Add and mod

    /* Fill dummy entries in temp with zeroes */
    for (i = 0; i < p*local_n; i++)
        for (j = n; j < MAX_ORDER; j++)
            temp[i][j] = ((float)rand()/(float)(RAND_MAX)) * a;

    if (my_rank == 0) {
        //printf("%s\n", prompt);
        for (q = 1; q < p; q++) {
            for (i = 0; i < p*local_n; i++) {
                for (j = 0; j < n; j++) {
                    //scanf("%f",&temp[i][j]);
                    temp[i][j] = ((float)rand()/(float)(RAND_MAX)) * a; //!
                    //temp[i][j]=1.0;
                }
            }
            MPI_Send(&(temp[0][q]), 1, column_mpi_t, q, 0,
                MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&(temp[0][q]), 1, column_mpi_t, 0, 0,
            MPI_COMM_WORLD, &status); 
 //       for (i = 0; i < n; i++) // no 
 //           printf("%3.1f ", temp[i][2]); // no 
 //       printf("\n"); // no    
    }
    //MPI_Scatter(temp, local_n*MAX_ORDER, MPI_FLOAT, local_Q,
    //    local_n*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

}  /* Read_matrix */

/**********************************************************************/
/* All arrays are allocated in calling program */
/* Note that argument m is unused              */
void Parallel_matrix_prod(
         LOCAL_MATRIX_T  local_A     /* in  */,
         int             n           /* in  */,
         LOCAL_MATRIX_T  local_B     /* in  */, //!
         LOCAL_MATRIX_T  global_B    /* in  */, //!
         LOCAL_MATRIX_T  local_C     /* out */,
         int             local_n     /* in  */) {


    int i, j, k;

    MPI_Allgather(local_B, local_n*MAX_ORDER, MPI_FLOAT,
                   global_B, local_n*MAX_ORDER, MPI_FLOAT,
                   MPI_COMM_WORLD);
    //for (i = 0; i < local_m; i++) {
    //    local_y[i] = 0.0;
    //    for (j = 0; j < n; j++)
    //        local_y[i] = local_y[i] +
    //                     local_A[i][j]*global_x[j];
    //}

//    for (j = 0; j < n; j++){    
//        for (i = 0; i < local_n; i++){
//            local_C[i][j] = 0.0;
//            for (k = 0; k < n; k++){
//                local_C[i][j] = local_C[i][j] + local_A[i][k]*global_B[k][j];
            //printf("i = %d, k = %d, a_ik = %f\n", i, k, local_A[0][0]);
            //printf("k = %d, j = %d, b_kj = %f\n", k, j, local_B[0][0]);
            //printf("i = %d, j = %d, c_ij = %f\n", i, j, local_C[i][j]);
//            }
//        }
//    }


    for (j = 0; j < n; j++){
        for (k = 0; k < n; k++){   
            for (i = 0; i < local_n; i++){

                local_C[i][j] = 0.0;

                local_C[i][j] = local_C[i][j] + local_A[i][k]*global_B[k][j];
            }
        }
    }

}  /* Parallel_matrix_vector_prod */


/**********************************************************************/
void Print_matrix(
         char*           title      /* in */, 
         LOCAL_MATRIX_T  local_Q    /* in */, 
         int             local_n    /* in */, 
         int             n          /* in */,
         int             my_rank    /* in */,
         int             p          /* in */) {

    int   i, j;
    float temp[MAX_ORDER][MAX_ORDER];

    MPI_Gather(local_Q, local_n*MAX_ORDER, MPI_FLOAT, temp, 
         local_n*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("%s\n", title);
        for (i = 0; i < p*local_n; i++) {
            for (j = 0; j < n; j++)
                printf("%4.1f ", temp[i][j]);
            printf("\n");
        }
    } 
}  /* Print_matrix */

/**********************************************************************/
