/*
 * =====================================================================================
 *
 *       Filename:  fft3.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015년 12월 07일 15시 28분 57초
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI  M_PI  /*  pi to machine precision, defined in math.h */
#define TWOPI (2.0*PI)

/*
 * 
 * Inputs:
 * data[] : array of complex* data points of size 2*N_Point+1.
 * data[0] is unused,
 * *n번째 복소수에 대해, for 0 <= n <= length(x)-1, is stored as:
 * data[2*n+1] = real(x(n))
 * data[2*n+2] = imag(x(n))
 *
 * if 입력받은 길이< N-포인트, 나머지는 0으로 채웁니다 
 * n_point : FFT order N_Point. This MUST be a power of 2 and >= length(x).
 * inverse_option: 
 * if set to 1,
 * if set to -1,
 *
 * 역변환은 
 * 1/N을 마지막 출력에 다 곱합니다
 * Outputs:
 *
 * data[] : 변환의 결과가 담기는 배열로 원본에 덮어쓰기 됩니다.
 * */

void FFT(double data[], int n_point, int inverse_option)
{ 
  int n, mmax, m, j, istep, i;

  double wtemp, wr, wpr, wpi, wi, theta;
  double tempr, tempi;

  n = n_point << 1;
  j = 1;
  for (i = 1; i < n; i += 2) {
    if (j > i) {
      tempr = data[j];     data[j] = data[i];     data[i] = tempr;
      tempr = data[j + 1]; data[j + 1] = data[i + 1]; data[i + 1] = tempr;
    }
    m = n >> 1;
    while (m >= 2 && j > m) 
    {
      j -= m;
      m >>= 1;
    }
    j += m;
  }
  mmax = 2;
  while (n > mmax) 
  {
    istep = 2 * mmax;
    theta = TWOPI / (inverse_option*mmax);
    wtemp = sin(0.5*theta);
    wpr = -2.0*wtemp*wtemp;
    wpi = sin(theta);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2)
    {
      for (i = m; i <= n; i += istep) 
      {
        j = i + mmax;
        tempr = wr*data[j] - wi*data[j + 1];
        tempi = wr*data[j + 1] + wi*data[j];
        data[j] = data[i] - tempr;
        data[j + 1] = data[i + 1] - tempi;
        data[i] += tempr;
        data[i + 1] += tempi;
      }
      wr = (wtemp = wr)*wpr - wi*wpi + wr;
      wi = wi*wpr + wtemp*wpi + wi;
    }
    mmax = istep;
  }
}


int main(int argc, char *argv[])
{
  int i;
  int input_count;
  int N_Point;
  double *x;
  double *X;

  /* 0~9생성 */
  input_count = 8;
  printf("input_count = %d\n", input_count);
  x = (double *)malloc(input_count * sizeof(double));

  x[0] = 0; 
  x[1] = 1; 
  x[2] = 2; 
  x[3] = 3; 
  x[4] = 4; 
  x[5] = 5; 
  x[6] = 6; 
  x[7] = 7;

  /* 데이터보다 한단계 더 큰 이진수 찾기 */
  N_Point = (int)pow(2.0, ceil(log((double)input_count) / log(2.0)));
  printf("N_Point = %d\n", N_Point);

  /* allocate memory for N_Point complex numbers (note the +1) */
  X = (double *)malloc((2 * N_Point + 1) * sizeof(double));

  /* 데이터를 집어넣어 줍니다 */
  for (i = 0; i<input_count; i++) {
    X[2 * i + 1] = x[i];  // 실수
    X[2 * i + 2] = 0.0;   // 허수
  }

  /* 나머지 빈 부분을 다 0으로 채움 */
  for (i = input_count; i<N_Point; i++) {
    X[2 * i + 1] = 0.0;
    X[2 * i + 2] = 0.0;
  }

  printf("입력된 데이터를 2제곱수 근처로 패딩합니다.:\n");
  for (i = 0; i<N_Point; i++)
    printf("x[%d] = (%.2f + j %.2f)\n", i, X[2 * i + 1], X[2 * i + 2]);

  /* FFT 계산 */
  FFT(X, N_Point, 1);
  /* 계산된 FFT찍기 */
  printf("\n_Point:\n");
  for (i = 0; i<N_Point; i++)
    printf("X[%d] = (%.2f + j %.2f)\n", i, X[2 * i + 1], X[2 * i + 2]);

  /* 역 FFT*/
  FFT(X, N_Point, -1);

  /*  normalize the IFFT */
  for (i = 0; i<N_Point; i++) {
    X[2 * i + 1] /= N_Point;
    X[2 * i + 2] /= N_Point;
  }

  //역변환 찍기
  printf("\n역변환 값입니다.:\n");
  for (i = 0; i<N_Point; i++)
    printf("x[%d] = (%.2f + j %.2f)\n", i, X[2 * i + 1], X[2 * i + 2]);

  return 0;
}
