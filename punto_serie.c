#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    int n;
    n = 10;
    int i;
    float x[n],y[n];
    float dot;
    float sum;
    
    float a = 5.0;

    srand((unsigned int)time(NULL));

   for(i=0;i<n;i++) //Se calcula los coeficientes del y.
     x[i]=((float)rand()/(float)(RAND_MAX)) * a;

    printf("\nElements of x: ");
    for(i=0;i<n;i++){
     printf("%f, ",x[i]);
     }

   for(i=0;i<n;i++)
     y[i]=((float)rand()/(float)(RAND_MAX)) * a;
 
   printf("\nElements of y: ");
    for(i=0;i<n;i++){
     printf("%f, ",y[i]);
     }

    for (i = 0; i < n; i++)
        sum = sum + x[i]*y[i];
    printf("\nThe dot product is %f\n", sum);


   return 0;
}

