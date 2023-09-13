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
  int size_r = 30;
  __m128i r1, r2, r3, r4, r5, max_px;
  uint8_t A[30] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30};
  uint8_t* pixels_p = (uint8_t*)malloc(30 * sizeof(uint8_t));
  for (int i = 0; i < 30; i++){
    pixels_p[i] = i + 1;
  }
  uint8_t max[size_r];
  // char* specific_position = pixels_p + 10;
  // r1 = _mm_loadu_si128((__m128i*) specific_position);
  // // r2 = _mm_loadu_si128((__m128i*)B);
  // // max_px = _mm_max_epi8(r1, r2);
  // _mm_store_si128((__m128i*)max, r1);
  // for (int h = 0; h < size_r; h++) {
  //   printf("%d ",max[h]);
  // }
  // for (int i = 1; i < 3; i+2) {
  //   for (int j = 1; j < 3; j+2) {
  //     char* p1 = A + (10 * (i - 1) + j);
  //     char* p2 = A + (10 * i + (j - 1));
  //     char* p3 = A + (10 * i + j);
  //     char* p4 = A + (10 * i + (j + 1));
  //     char* p5 = A + (10 * (i + 1) + j);
  //     r1 = _mm_loadu_si128((__m128i*)p1);
  //     r2 = _mm_loadu_si128((__m128i*)p2);
  //     r3 = _mm_loadu_si128((__m128i*)p3);
  //     r4 = _mm_loadu_si128((__m128i*)p4);
  //     r5 = _mm_loadu_si128((__m128i*)p5);
  //     _mm_store_si128((__m128i*)max, r1);
  //     for (int h = 0; h < size_r; h++) {
  //       printf("%d ",max[h]);
  //     }      
  //   }
  // }
  // int der = i * 10 + j
  // LARGO FILAS -> 10, si la posicion + LAR
  // LARGO FILA (10) -> POSICION(2) + LARGO REGISTRO()
  char* p1 = A + (10 * 1 + 1 + 8);
  r1 = _mm_loadu_si128((__m128i*)p1);
  _mm_store_si128((__m128i*)max, r1);
  for (int h = 0; h < size_r; h++) {
    printf("%d ",max[h]);
  }
}