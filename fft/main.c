#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define PI	3.1415926535

/*
 * Twinddle factor strcut
 */
struct twinddle_factor
{
	double real;
	double imag;
};

void tf_print(struct twinddle_factor *tf, int size);
int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b);

/*
 * Calculating FFT
 * @result : FFT result array
 * @input : input array
 * @size : size of input array
 * @inverse :
 * @index : h_(index)
 */
struct twinddle_factor *FFT_Calc(
    struct twinddle_factor *result, 
    struct twinddle_factor *input, 
    int size, 
    int inverse, 
    int *index)
{
  int i = 0;
	struct twinddle_factor *t = NULL;

  /* Return recursive function */
  if (size == 1) {
    tf_copy(&result[(*index)], &input[0]);
    *(index) = *(index) + 1;
    return NULL;
  }

	double degree = 2 * PI/ size * inverse;
  t = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  for (i = 0; i < size / 2; i++) {
    t[i].real = input[i].real + input[i + size / 2].real;
		t[i].imag = input[i].imag + input[i + size / 2].imag;
  }

	for (i = 0; i < size / 2; i++) {
    t[i + size / 2].real = (input[i].real - input[i + size / 2].real)*cos(degree*i) + (input[i].imag - input[i + size / 2].imag)*sin(degree*i);
		t[i + size / 2].imag = (input[i + size / 2].real - input[i].real)*sin(degree*i) + (input[i].imag - input[i + size / 2].imag)*cos(degree*i);
	}

  printf("Loop 1\n");
  tf_print(t, size);

  /* Recursive */
  FFT_Calc(result, &t[0], size/2, inverse, index);
  FFT_Calc(result, &t[size/2], size/2, inverse, index);
  
	return t;
}

/*
 * Copy twinddle_factor
 */
int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b)
{
  a->real = b->real;
  a->imag = b->imag;

  return 0;
}

/*
 * Init twinddle_factor array.
 * @array : double array
 * @size : size of array
 */
struct twinddle_factor *tf_init(double *array, int size)
{
  int i = 0;
  struct twinddle_factor *tf = NULL;

  if (size <= 0)
    return NULL;

  tf = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  if (array == NULL)
    goto insert_0;
  else
    goto insert_array;

insert_array:
  for (i = 0; i < size; i++) {
    tf[i].real = array[i];
    tf[i].imag = 0;
  }
  return tf;

insert_0:
  for (i = 0; i < size; i++) {
    tf[i].real = 0;
    tf[i].imag = 0;
  }
  return tf;
}

struct twinddle_factor *Main_FFT(struct twinddle_factor *input, const int size, int inverse)
{
  struct twinddle_factor *result = NULL;
  int *index = NULL;

  /* Init inedx */
  index = (int*)malloc(sizeof(int));
  *index = 0;

  /* Init resutn array */
  result = tf_init(NULL, size);

  /* Run FFT main */
  FFT_Calc(result, input, size, inverse, index);

  printf("Index : %d\n", *index);
  return result;
}

void tf_print(struct twinddle_factor *tf, int size)
{
  int i = 0;

  printf("==== PRINT : Start====\n");
  for (i = 0; i < size; i++) {
    printf("%p real : %lf / imag : %lf \n", &tf[i], tf[i].real, tf[i].imag);
  }
  printf("==== PRINT : End ====\n");
 
}

int main(int argc, const char *argv[])
{
  int size = 8;
  double array[] = {0, 1, 2, 3, 4, 5, 6, 7};
  /* int array[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}; */
  struct twinddle_factor *tf = NULL;
  struct twinddle_factor *result = NULL;

  int i=0;
  double tmp[8]={0,};
  
  
  tf = tf_init(array, size);
  /* tf = tf_init(tmp, size); */

  tf_print(tf, size);

  printf("===== \n\n");
  tf = Main_FFT(tf, 8, 1);
  tf_print(tf, size);
  
  return 0;

}
