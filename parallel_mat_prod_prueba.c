
#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <time.h>

#define MAX_ORDER 100

typedef float LOCAL_MATRIX_T[MAX_ORDER][MAX_ORDER];

int main(int argc, char* argv[]) {
    int             my_rank;
    int             p;
    LOCAL_MATRIX_T  local_V;
    LOCAL_MATRIX_T  local_W;
    LOCAL_MATRIX_T  local_X;
    LOCAL_MATRIX_T  local_Y;
    LOCAL_MATRIX_T  local_Z;   
    LOCAL_MATRIX_T  local_A; 
    LOCAL_MATRIX_T  local_B; //!
    LOCAL_MATRIX_T  local_Q; //!
    LOCAL_MATRIX_T  local_C; //!
    LOCAL_MATRIX_T  local_D; //!
    LOCAL_MATRIX_T  local_E; //!
    LOCAL_MATRIX_T  local_F; //!
    LOCAL_MATRIX_T  local_G; //!
    LOCAL_MATRIX_T  local_H; //!
    LOCAL_MATRIX_T  local_AE; //!
    LOCAL_MATRIX_T  local_BG; //!
    LOCAL_MATRIX_T  local_AF; //!
    LOCAL_MATRIX_T  local_BH; //!
    LOCAL_MATRIX_T  local_CE; //!
    LOCAL_MATRIX_T  local_DG; //!
    LOCAL_MATRIX_T  local_CF; //!
    LOCAL_MATRIX_T  local_DH; //!
    LOCAL_MATRIX_T  local_R; //!
    LOCAL_MATRIX_T  local_S; //!
    LOCAL_MATRIX_T  local_T; //!
    LOCAL_MATRIX_T  local_U; //!
    int             n; //!
    int             local_n; //!

    srand((unsigned int)time(NULL)); //!

    void Read_matrix(LOCAL_MATRIX_T local_Q, int local_n, int n,
             int my_rank, int p);

    void Parallel_matrix_prod( LOCAL_MATRIX_T local_V, LOCAL_MATRIX_T local_W,
        LOCAL_MATRIX_T local_X, LOCAL_MATRIX_T local_Y, 
             LOCAL_MATRIX_T local_Z, int n, int local_n);
    void Print_matrix(char* title, LOCAL_MATRIX_T local_Q, int local_n,
             int n, int my_rank, int p);
 

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        //n = 2;
        printf("Enter the order of the matrix (n x n)\n");
        scanf("%d", &n);
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    local_n = n/p;

    Read_matrix(local_A, local_n, n, my_rank, p);
    Read_matrix(local_B, local_n, n, my_rank, p); //!
    Read_matrix(local_C, local_n, n, my_rank, p); //!
    Read_matrix(local_D, local_n, n, my_rank, p); //!
    Read_matrix(local_E, local_n, n, my_rank, p); //!
    Read_matrix(local_F, local_n, n, my_rank, p); //!
    Read_matrix(local_G, local_n, n, my_rank, p); //!
    Read_matrix(local_H, local_n, n, my_rank, p); //!


    Parallel_matrix_prod(local_A, local_E, local_B, local_G, local_R, n, local_n); //!
    Print_matrix("The product R is", local_R, local_n, n, my_rank, p);

    Parallel_matrix_prod(local_A, local_F, local_B, local_H, local_S, n, local_n); //!
    Print_matrix("The product S is", local_S, local_n, n, my_rank, p);

    Parallel_matrix_prod(local_C, local_E, local_D, local_G, local_T, n, local_n); //!
    Print_matrix("The product T is", local_T, local_n, n, my_rank, p);

    Parallel_matrix_prod(local_C, local_F, local_D, local_H, local_U, n, local_n); //!
    Print_matrix("The product U is", local_U, local_n, n, my_rank, p);


    //Print_matrix("The product R is", local_AF, local_n, n, my_rank, p);

    MPI_Finalize();

}  /* main */


/**********************************************************************/
void Read_matrix(
         //char*           prompt   /* in  */, 
         LOCAL_MATRIX_T  local_Q  /* out */, 
         int             local_n  /* in  */, 
         int             n        /* in  */,
         int             my_rank  /* in  */, 
         int             p        /* in  */) {

    int             i, j;
    float a = 5.0; //!
    LOCAL_MATRIX_T  temp;

    /* Fill dummy entries in temp with zeroes */
    for (i = 0; i < p*local_n; i++)
        for (j = n; j < MAX_ORDER; j++)
            temp[i][j] = 0.0;

    if (my_rank == 0) {
        //printf("%s\n", prompt);
        for (i = 0; i < p*local_n; i++) 
            for (j = 0; j < n; j++)
                //scanf("%f",&temp[i][j]);
                temp[i][j] = ((float)rand()/(float)(RAND_MAX)) * a; //!
                //temp[i][j]=1.0;
    }
    MPI_Scatter(temp, local_n*MAX_ORDER, MPI_FLOAT, local_Q,
        local_n*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);

}  /* Read_matrix */

/**********************************************************************/
/* All arrays are allocated in calling program */
/* Note that argument m is unused              */
void Parallel_matrix_prod(
         LOCAL_MATRIX_T  local_V     /* in  */,
         LOCAL_MATRIX_T  local_W     /* in  */, //!
         LOCAL_MATRIX_T  local_X     /* in  */,
         LOCAL_MATRIX_T  local_Y     /* in  */, //!
         LOCAL_MATRIX_T  local_Z     /* out */,
         int             n           /* in  */,
         int             local_n     /* in  */) {


    int i, j, k;

    MPI_Allgather(local_Z, local_n, MPI_FLOAT,
                   local_Z, local_n, MPI_FLOAT,
                   MPI_COMM_WORLD);
    //for (i = 0; i < local_m; i++) {
    //    local_y[i] = 0.0;
    //    for (j = 0; j < n; j++)
    //        local_y[i] = local_y[i] +
    //                     local_A[i][j]*global_x[j];
    //}

    for (i = 0; i < local_n; i++){    
        for (j = 0; j < n; j++) {
            local_Z[i][j] = 0.0;
            for (k = 0; k < n; k++)
                local_Z[i][j] = local_Z[i][j] + local_X[i][k]*local_Y[k][j] + local_V[i][k]*local_W[k][j];
        }
    }

 //   MPI_Scatter(local_Z, local_n*MAX_ORDER, MPI_FLOAT, local_Z,
 //       local_n*MAX_ORDER, MPI_FLOAT, 0, MPI_COMM_WORLD);
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