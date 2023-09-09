#include <pmmintrin.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  __m128 A1, A2, A, B, C, B1, B2, D;
  float a[4] __attribute__((aligned(16))) = {1.0, 2.0, 3.0, 4.0};
  float b[4] __attribute__((aligned(16))) = {0.1, 0.2, 0.3, 0.4};
  C = _mm_shuffle_ps(A, B, _MM_SHUFFLE(1, 3, 3, 0));
  printf("%d", A[2]);
}