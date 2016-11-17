#include "npio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>


/* 100-long 64-bit signed array */
void test1()
{
  int err;
  size_t i;
  int64_t *data, total;
  npio_Array array;

  npio_init_array(&array);
  err = npio_load("test1.npy", &array);
  if (err)
  {
    fprintf(stderr, "npio_load: %s\n", strerror(err));
    exit(1);
  }

  printf("data type: ");
  if (array.floating_point)
    printf("%d-bit float\n", array.bit_width);
  else if (array.is_signed)
    printf("%d-bit signed integer\n", array.bit_width);
  else
    printf("%d-bit unsigned integer\n", array.bit_width);

  printf("shape: ");
  for (i = 0; i < array.dim; ++i)
    printf("%ld ", array.shape[i]);
  printf("\n");

  assert(array.bit_width == 64 && !array.floating_point && array.is_signed);

  data = (int64_t*) array.data;
  total = 0;
  for (i = 0; i < array.size; ++i)
    total += data[i];

  assert(total == 4950);

  npio_free_array(&array);
  printf("test1 passed\n");
}


/* 10000-long 32-bit floats in 100x10x10 array */
void test2()
{
  int err;
  size_t i;
  float *data, total;
  npio_Array array;

  npio_init_array(&array);
  err = npio_load("test2.npy", &array);
  if (err)
  {
    fprintf(stderr, "npio_load: %s\n", strerror(err));
    exit(1);
  }

  printf("data type: ");
  if (array.floating_point)
    printf("%d-bit float\n", array.bit_width);
  else if (array.is_signed)
    printf("%d-bit signed integer\n", array.bit_width);
  else
    printf("%d-bit unsigned integer\n", array.bit_width);

  printf("shape: ");
  for (i = 0; i < array.dim; ++i)
    printf("%ld ", array.shape[i]);
  printf("\n");

  assert(array.bit_width == 32 && array.floating_point);

  data = (float*) array.data;
  total = 0;
  for (i = 0; i < array.size; ++i)
    total += data[i];
  assert(fabs(total / 5005.37f - 1.0f) < 1e-6f);

  npio_free_array(&array);
  printf("test2 passed\n");
}


void test3()
{
  int i, err;
  float *data, total;
  npio_Array array;

  npio_init_array(&array);
  err = npio_load_header("test2.npy", &array);
  if (err)
  {
    fprintf(stderr, "npio_load_header: %s\n", strerror(err));
    exit(1);
  }

  err = npio_load_data(&array);
  if (err)
  {
    fprintf(stderr, "npio_load_data: %s\n", strerror(err));
    exit(1);
  }

  data = (float*) array.data;
  total = 0;
  for (i = 0; i < array.size; ++i)
    total += data[i];
  assert(fabs(total / 5005.37f - 1.0f) < 1e-6f);

  npio_free_array(&array);
  printf("test3 passed\n");
}


void test4()
{
  int i, err;
  int64_t *data;
  float total;
  FILE *f;
  void *p;
  size_t sz;
  npio_Array array;

  npio_init_array(&array);
  f = fopen("test1.npy", "r");
  if (!f)
  {
    fprintf(stderr, "could not open test1.npy\n");
    exit(1);
  }

  fseek(f, 0, SEEK_END);
  sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  p = malloc(sz);

  err = npio_load_mem(p, sz, &array);
  if (err)
  {
    fprintf(stderr, "npio_load_mem: %s\n", strerror(err));
    exit(1);
  }

  fclose(f);

  data = (int64_t*) array.data;
  total = 0;
  for (i = 0; i < array.size; ++i)
    total += data[i];

  assert(total == 4950);
  
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
