#include <x86intrin.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* exit */

int main(int argc, char* argv[]) {
  int size_r = 10;
  uint8_t data[16];
  uint8_t max[16];
  for (uint8_t i = 0; i < size_r; i++) {
  max[i] = 0; // O cualquier otro valor de inicialización que desees.
}
  for (uint8_t i = 0; i < size_r; i++) {
    data[i] = i;
  }
  __m256i r1 = _mm256_loadu_si256((__m256i*) data);
  _mm256_store_si256((__m256i*)max, r1);
  for (int h = 0; h < size_r; h++) {
    printf("%d ",max[h]);
  }
}