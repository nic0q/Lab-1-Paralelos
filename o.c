#include <cpuid.h> /* __get_cpuid_max, __get_cpuid */
#include <ctype.h>
#include <immintrin.h>
#include <mmintrin.h> /* MMX instrinsics __m64 integer type */
#include <smmintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <tmmintrin.h>
#include <unistd.h>
#include <xmmintrin.h> /* SSE __m128 float */

int main(int argc, char* argv[]) {
  int size_r = 8;
  __m128i r1, r2, r3, r4, r5, max_px;
  uint8_t A[8] = {1,2,3,4,5,6,  7, 8};
  uint8_t B[8] = {2,3,4,5,5,100,70,81};
  uint8_t max[size_r];
  r1 = _mm_loadu_si128((__m128i*)A);
  r2 = _mm_loadu_si128((__m128i*)B);
  max_px = _mm_max_epi8(r1, r2);
  _mm_store_si128((__m128i*)max, max_px);
  for (int h = 0; h < size_r; h++) {
    printf("%d tamaño:%d a",max[h], sizeof(A));
  }
}