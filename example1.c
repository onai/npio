/* Simple example to print the shape of an array in npy format. */

#include <stdio.h>
#include "npio.h"

int main()
{
  int i;
  npio_Array array;

  npio_init_array(&array);
  npio_load("test1.npy", &array);

  printf("shape: ");
  for (i = 0; i < array.dim; ++i)
    printf("%ld ", array.shape[i]);
  printf("\n");

  npio_free_array(&array);
  return 0;
}
