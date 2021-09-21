#include <stdio.h>
#include <stdlib.h>

int main(void)
{
   int sz;
   sz = 10;
   //printf("Enter the size of array::");
   //scanf("%d",&sz);
   int vecA[sz],vecB[sz],i;


   //Se calcula los coeficientes del vecA.

   for(i=0;i<sz;i++)
     vecA[i]=rand()%100;   //Generate number between 0 to 99

   printf("\nElements of vecA: ");

   for(i=0;i<sz;i++)
   {
     printf("%i, ",vecA[i]);
   }

   ///////////////////////////////////////////

   //Se calcula los coeficientes del vecB.

   for(i=0;i<sz;i++)
     vecB[i]=rand()%100;   //Generate number between 0 to 99

   printf("\nElements of vecB: ");

   for(i=0;i<sz;i++)
   {
     printf("%i, ",vecB[i]);
   }


   return 0;
}
