#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef PI
/*  */
#else
#define PI	3.1415926535
#endif

struct twinddle_factor
{
	double real;
	double imag;
};

void tf_print(struct twinddle_factor *tf, int size);
int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b);

struct twinddle_factor *FFT_Calc(
    struct twinddle_factor *result, 
    struct twinddle_factor *input, 
    int size, 
    int inverse, 
    int *index)
{
  int i = 0;
	struct twinddle_factor *t = NULL;

  /* Return */
  if (size == 1) {

    tf_copy(&result[(*index)], &input[0]);
    /* tf_print(input, size);  */
    *(index) = *(index) + 1;
    return NULL;
  }

	double degree = 2 * PI/ size*inverse;
  t = (struct twinddle_factor*)malloc(sizeof(struct twinddle_factor) * size);

  for (i = 0; i < size / 2; i++) {
    t[i].real = input[i].real + input[i + size / 2].real;
		t[i].imag = input[i].imag + input[i + size / 2].imag;
  }

	for (i = 0; i < size / 2; i++) {
    t[i + size / 2].real = (input[i].real - input[i + size / 2].real)*cos(degree*i) + (input[i].imag - input[i + size / 2].imag)*sin(degree*i);
		t[i + size / 2].imag = (input[i + size / 2].real - input[i].real)*sin(degree*i) + (input[i].imag - input[i + size / 2].imag)*cos(degree*i);

  }

  FFT_Calc(result, &t[0], size/2, inverse, index);
  FFT_Calc(result, &t[size/2], size/2, inverse, index);
  
 
	return t;
}

int tf_copy(struct twinddle_factor *a, struct twinddle_factor *b)
{
  a->real = b->real;
  a->imag = b->imag;

  return 0;
}

struct twinddle_factor *tf_init(int *array, int size)
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
  int *index = NULL,i=0;

  /* Init inedx */
  index = (int*)malloc(sizeof(int));
  *index = 0;

  /* Init resutn array */
  result = tf_init(NULL, size);

  /* Run FFT main */
  FFT_Calc(result, input, size, inverse, index);
  
  for (i = 0; i < size; i++) {
    input[i].real /= size;
    input[i].imag /= size;
  }

  printf("Index : %d\n", *index);
  return result;
}



void tf_print(struct twinddle_factor *tf, int size)
{
  int i = 0;

  printf("==== PRINT ====\n");
  for (i = 0; i < size; i++) {
    printf("%d %p real : %lf / imag : %lf \n", i, &tf[i], tf[i].real, tf[i].imag);
  }
  printf("==== PRINT : end ====\n");
 
}

int bit_reversing(int i, int size)
{
  int *bit = NULL, j = 0, result = 0;
 
  if (size < 1)
    return 0;

  bit = (int*)malloc(sizeof(int) * size);

  for(j = 0; j < size; j++, i /= 2) {
    bit[size - j - 1] = i % 2;
  }
  
  for(j = 0; j < size; j++) {
    result += pow(2.0, j) * bit[j];
  }
  
  free(bit);
  return result;
}

/*
 * 위에 reversing 함수는 10진수의이진수를리버싱한후, 리버싱된 이진수를 다시 십진수로 바꿉니다.
 * 저것을 이용하여 아래 함수는 데이터배열이 들어왔을 때, 데이터들의 순서를 리버싱된 순서로 
 * 바꿔넣습니다.
 *
 * 아래 함수는 데이터들의 순서를 리버싱시키는 함수로 매개변수로 들어온 원본이 바뀝니다.
 * 예를 들어 데이터가 01234567로 저장되어있었다면 함수 실행 후 04261537로 데이터 순서가 리버싱됩니다.
 * 결과로 받은 구조체의 real들을 배열에 때려넣고 비트리버싱 시킨 다음, 찍으면 그게 주파수 변환된 아웃이에요.
 * */
void bit_reverse(int data[], int size)
{
  int count = 0, i = 0, j = 0, tmp = 0, reversed = 0;

  for(j = size; j > 1; j = j >> 1) {
    ++count;
  }

  for(i = 0; i < size / 2; i++) {
      reversed = bit_reversing(i, count);

      tmp = data[i];
      data[i] = data[reversed];
      data[reversed] = tmp;
      printf("swap : %d <=> %d \n", reversed, i);
  }
}

void tf_bit_reverse(struct twinddle_factor *tf, int size)
{
  int count = 0, i = 0, j = 0, reversed = 0;
  struct twinddle_factor tmp;

  for (j = size; j > 1; j = j >> 1) {
    ++count;
  }

  for (i = 0; i < size / 2; i++) {
    reversed = bit_reversing(i, count);

    tf_copy(&tmp, &tf[i]);
    tf_copy(&tf[i], &tf[reversed]);
    tf_copy(&tf[reversed], &tmp);
  }

}

int _main(int argc, const char *argv[])
{
  int size = 8,i=0;
  int array[8] = {0,1,2,3,4,5,6,7};
  
  struct twinddle_factor *tf = NULL;
  struct twinddle_factor *result = NULL;

  tf = tf_init(array, size);
  printf("Init..입력값 \n");
  tf_print(tf, size);

  printf("===== \n\n");
  tf = Main_FFT(tf, size, 1);
  printf("변환됨 Result \n");
  tf_print(tf, size);

  /* 리버싱 확인하기 */
  printf("reverse\n");
  tf_bit_reverse(tf,size);
  tf_print(tf, size);
  tf=Main_FFT(tf,size,-1);
  for(i=0;i<size;i++){tf[i].real/=size;tf[i].imag/=size;}
  tf_print(tf,size);
  tf_bit_reverse(tf,size);
  tf_print(tf,size);


  return 0;
}
