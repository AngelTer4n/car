
#include <stdio.h>


int main(void) {
    int n;

    n = 262144;

    int x[n],y[n],i;
    float dot;
    float sum;


    for(i=0;i<n;i++) //Se calcula los coeficientes del x.
        x[i]=rand()%100;   //Generate number between 0 to 99
    //printf("\nElements of x: ");

    //for(i=0;i<n;i++)
   //{
   //  printf("%i, ",x[i]);
   //}

   for(i=0;i<n;i++) //Se calcula los coeficientes del y.
     y[i]=rand()%100;   //Generate number between 0 to 99
   //printf("\nElements of y: ");

   //for(i=0;i<n;i++)
   //{
    // printf("%i, ",y[i]);
   //}

    for (i = 0; i < n; i++)
        sum = sum + x[i]*y[i];
    printf("\nThe dot product is %f\n", sum);

   //dot = sum;

   //printf("\nThe dot product is %f\n", dot);

   return 0;
}

