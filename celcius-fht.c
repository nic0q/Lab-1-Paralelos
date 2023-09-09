#include <cpuid.h>    /* __get_cpuid_max, __get_cpuid */
#include <mmintrin.h> /* MMX instrinsics __m64 integer type */
#include <stdio.h>
#include <stdlib.h>    /* exit */
#include <string.h>    /* memcpy */
#include <xmmintrin.h> /* SSE __m128 float */

void test_sse1(void) {
  __m128 celsiusvector, fahrenheitvector, partialvector, coeffvector;
  const __m128 thirtytwovector = {32.0, 32.0, 32.0, 32.0};
  float celsiusarray[]
      __attribute__((aligned(16))) = {-100.0, -80.0, -40.0, 0.0};
  float fahrenheitarray[4] __attribute__((aligned(16)));
  int i;
  coeffvector = _mm_set1_ps(9.0 / 5.0);
  celsiusvector = _mm_load_ps(celsiusarray);
  partialvector = _mm_mul_ps(celsiusvector, coeffvector);
  fahrenheitvector = _mm_add_ps(partialvector, thirtytwovector);
  _mm_store_ps(fahrenheitarray, fahrenheitvector);
  for (i = 0; i < 4; i++)
    printf("%f celsius es %f fahrenheit\n", celsiusarray[i],
           fahrenheitarray[i]);
}

int main(int argc, char* argv[]) {
  unsigned int eax, ebx, ecx, edx;
  unsigned int extensions, sig;
  int result, sse1_available;
  result = __get_cpuid(2, &eax, &ebx, &ecx, &edx);
  sse1_available = (bit_SSE & edx);
  // Por algún motivo sale 0 pero el programa funciona
  if (0 == sse1_available) {
    fprintf(stderr, "Error: SSE1 features not available\n");
    // exit(-1);
  } else
    fprintf(stderr, "SSE1 features ARE available\n");
  test_sse1();
}