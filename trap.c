/* trap.c -- Parallel Trapezoidal Rule (cuarta versión). Funciones Bcast.
 *
 * Input: Se introduce los valores de integración [a,b] y el número de 
          trapezoides.

 * Output:  Estimate of the integral from a to b of f(x)
 *    using the trapezoidal rule and n trapezoids.
 *
 * Algorithm:
 *    1.  Each process calculates "its" interval of
 *        integration.
 *    2.  Each process estimates the integral of f(x)
 *        over its interval using the trapezoidal rule.
 *    3a. Each process != 0 sends its integral to 0.
 *    3b. Process 0 sums the calculations received from
 *        the individual processes and prints the result.
 *
 * Notes:  
 *    1.  f(x), a, b, and n are all hardwired.
 *    2.  The number of processes (p) should evenly divide
 *        the number of trapezoids (n = 1024)
 *
 * See Chap. 4, pp. 56 & ff. in PPMPI.
 */
#include <stdio.h>

/* We'll be using MPI routines, definitions, etc. */
#include "mpi.h"

int main(int argc, char** argv) {
    int         my_rank;   /* My process rank           */
    int         p;         /* The number of processes   */
    float       a;         /* Límite izquierdo          */
    float       b;         /* Límite derecho            */
    int         n;         /* Number of trapezoids      */
    float       h;         /* Trapezoid base length     */
    float       local_a;   /* Left endpoint my process  */
    float       local_b;   /* Right endpoint my process */
    int         local_n;   /* Number of trapezoids for  */
                           /* my calculation            */
    float       integral;  /* Integral over my interval */
    float       total;     /* Total integral            */
    int         source;    /* Process sending integral  */
    int         dest = 0;  /* All messages go to 0      */
    int         tag = 0;
    MPI_Status  status;

    void Get_data2(float* a_ptr, float* b_ptr, int* n_ptr, int my_rank); /*la función Get_data que reparta en input a los demás nodos*/
    float Trap(float local_a, float local_b, int local_n,
              float h);    /* Calculate local integral  */

    /* Let the system do what it needs to start up MPI */
    MPI_Init(&argc, &argv);

    /* Get my process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out how many processes are being used */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    Get_data2(&a, &b, &n, my_rank);/*Aquí leemos los datos desde la terminal*/

    h = (b-a)/n;    /* h is the same for all processes */
    local_n = n/p;  /* So is the number of trapezoids */

    /* Length of each process' interval of
     * integration = local_n*h.  So my interval
     * starts at: */
    local_a = a + my_rank*local_n*h;
    local_b = local_a + local_n*h;
    integral = Trap(local_a, local_b, local_n, h);

    /* Add up the integrals calculated by each process */
    if (my_rank == 0) {
        total = integral;
        for (source = 1; source < p; source++) {
            MPI_Recv(&integral, 1, MPI_FLOAT, source, tag, 
                MPI_COMM_WORLD, &status);
            total = total + integral;
        }
    } else {   
        MPI_Send(&integral, 1, MPI_FLOAT, dest, 
            tag, MPI_COMM_WORLD);
    }

    /* Print the result */
    if (my_rank == 0) {
        printf("With n = %d trapezoids, our estimate\n", 
            n);
        printf("of the integral from %f to %f = %f\n", 
            a, b, total); 
    }

    /* Shut down MPI */
    MPI_Finalize();
} /*  main  */


/********************************************************************/
/* Ceiling of log_2(x) is just the number of times
 * times x-1 can be divided by 2 until the quotient
 * is 0.  Dividing by 2 is the same as right shift.
 */
int Ceiling_log2(int  x  /* in */) {
    /* Use unsigned so that right shift will fill
     * leftmost bit with 0
     */
    unsigned temp = (unsigned) x - 1;
    int result = 0;

    while (temp != 0) {
         temp = temp >> 1;
         result = result + 1 ;
    }
    return result;
} /* Ceiling_log2 */


/********************************************************************/
int I_receive(
        int   stage       /* in  */,
        int   my_rank     /* in  */, 
        int*  source_ptr  /* out */) {
    int   power_2_stage;

    /* 2^stage = 1 << stage */
    power_2_stage = 1 << stage;
    if ((power_2_stage <= my_rank) && 
            (my_rank < 2*power_2_stage)){
        *source_ptr = my_rank - power_2_stage;
        return 1;
    } else return 0;
} /* I_receive */

/********************************************************************/
int I_send(
        int   stage     /* in  */,
        int   my_rank   /* in  */,
        int   p         /* in  */, 
        int*  dest_ptr  /* out */) {
    int power_2_stage;

    /* 2^stage = 1 << stage */
    power_2_stage = 1 << stage;  
    if (my_rank < power_2_stage){
        *dest_ptr = my_rank + power_2_stage;
        if (*dest_ptr >= p) return 0;
        else return 1;
    } else return 0;
} /* I_send */            

/********************************************************************/
void Send(
        float  a     /* in */,
        float  b     /* in */, 
        int    n     /* in */, 
        int    dest  /* in */) {

    MPI_Send(&a, 1, MPI_FLOAT, dest, 0, MPI_COMM_WORLD);
    MPI_Send(&b, 1, MPI_FLOAT, dest, 1, MPI_COMM_WORLD);
    MPI_Send(&n, 1, MPI_INT, dest, 2, MPI_COMM_WORLD);
} /* Send */


/********************************************************************/
void Receive(
        float*  a_ptr  /* out */, 
        float*  b_ptr  /* out */,
        int*    n_ptr  /* out */,
        int     source /* in  */) {

    MPI_Status status;

    MPI_Recv(a_ptr, 1, MPI_FLOAT, source, 0, 
        MPI_COMM_WORLD, &status);
    MPI_Recv(b_ptr, 1, MPI_FLOAT, source, 1, 
        MPI_COMM_WORLD, &status);
    MPI_Recv(n_ptr, 1, MPI_INT, source, 2, 
        MPI_COMM_WORLD, &status);
} /* Receive */


/********************************************************************/
/* Función Get_data2*/

void Get_data2(
         float*  a_ptr    /* out */, 
         float*  b_ptr    /* out */, 
         int*    n_ptr    /* out */,
         int     my_rank  /* in  */) {

    if (my_rank == 0) {
        printf("Enter a, b, and n\n");
        scanf("%f %f %d", a_ptr, b_ptr, n_ptr);
    }
    MPI_Bcast(a_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD); /* Funciones Bcast*/ 
    MPI_Bcast(b_ptr, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(n_ptr, 1, MPI_INT, 0, MPI_COMM_WORLD);
} /* Get_data2 */


/********************************************************************/
float Trap(
          float  local_a   /* in */, 
          float  local_b   /* in */, 
          int    local_n   /* in */, 
          float  h         /* in */) { 

    float integral;   /* Store result in integral  */ 
    float x; 
    int i; 

    float f(float x); /* function we're integrating */

    integral = (f(local_a) + f(local_b))/2.0; 
    x = local_a; 
    for (i = 1; i <= local_n-1; i++) { 
        x = x + h; 
        integral = integral + f(x); 
    } 
    integral = integral*h; 
    return integral;
} /*  Trap  */


/********************************************************************/
float f(float x) { 
    float return_val; 
    /* Calculate f(x). */
    /* Store calculation in return_val. */ 
    return_val = x*x;
    return return_val; 
} /* f */

