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
#include "image.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void write_file(char* name,  uint8_t* pixels, int height, int width, int maxValue);

int main(int argc, char* argv[]) {
  int option, imageWidth = 0, indx = 0, size_r = 16, cont = 0, indy = 0;
  char *inputImage = NULL, *outputImage1 = NULL, *outputImage2 = NULL;
  uint8_t A[size_r], B[size_r], C[size_r], D[size_r], E[size_r], max[size_r];
  __m128i r1, r2, r3, r4, r5, max_px;
  PGMImage* pgm = malloc(sizeof(PGMImage));
  while ((option = getopt(argc, argv, "i:s:p:N:")) != -1) {
    switch (option) {
      case 'i':  // nombre del archivo de entrada
        inputImage = optarg;
        break;
      case 's':  // nombre del archivo de salida
        outputImage1 = optarg;
        break;
      case 'p':  // nombre del archivo de salida
        outputImage2 = optarg;
        break;
      case 'N':  // ancho de la imagen
        sscanf(optarg, "%d", &imageWidth);
        break;
      case '?':
        exit(0);
      default:  // Si no se ha ingresado alguna flag obligatoria, se aborta
        abort();
    }
  }
  printf("inputImage: %s\noutputImage1: %s\noutputImage2: %s\nimageWidth: %d\n",
         inputImage, outputImage1, outputImage2, imageWidth);
  openPGM(pgm, inputImage);
  int dim = (pgm->height - 2) * (pgm->width - 2);
  uint8_t* pixels = (uint8_t*)malloc(dim * sizeof(uint8_t));
  uint8_t* pixels2 = (uint8_t*)malloc(dim * sizeof(uint8_t));
  clock_t start = clock();
  for (int i = 1; i < pgm->height - 1; i++) {
    for (int j = 1; j < pgm->width - 1; j++) {
      A[indx] = pgm->data[i - 1][j];  // arriba
      B[indx] = pgm->data[i][j - 1];  // izquierda
      C[indx] = pgm->data[i][j];      // centro
      D[indx] = pgm->data[i][j + 1];  // derecha
      E[indx] = pgm->data[i + 1][j];  // abajo
      if (indx + 1 == size_r) {
        r1 = _mm_loadu_si128((__m128i*)A);
        r2 = _mm_loadu_si128((__m128i*)B);
        r3 = _mm_loadu_si128((__m128i*)C);
        r4 = _mm_loadu_si128((__m128i*)D);
        r5 = _mm_loadu_si128((__m128i*)E);
        max_px = _mm_max_epu8(_mm_max_epu8(_mm_max_epu8(r1, r2), _mm_max_epu8(r3, r4)), r5);
        _mm_store_si128((__m128i*)max, max_px);
        for (int h = 0; h < size_r; h++) {
          pixels[cont] = max[h];
          cont++;
        }
        indx = 0;
      } else {
        indx++;
      }
    }
  }
  clock_t end = clock();
  clock_t start1 = clock();
  for (int i = 1; i < pgm->height - 1; i++) {
    for (int j = 1; j < pgm->width - 1; j++) {
      pixels2[indy] = MAX(MAX(MAX(pgm->data[i][j + 1], pgm->data[i + 1][j]),MAX(pgm->data[i - 1][j], pgm->data[i][j - 1])),pgm->data[i][j]);
      indy++;
    }
  }
  clock_t end1 = clock();
  double paralelo = ((double) (end - start)) / CLOCKS_PER_SEC;
  double secuencial = ((double) (end1 - start1)) / CLOCKS_PER_SEC;
  printf("T.Paralelo: %f\nT.Secuencial: %f\n", paralelo, secuencial);
  write_file(outputImage2, pixels, pgm->height, pgm->width, pgm->maxValue);
  // write_file(outputImage2, pixels2, pgm->height, pgm->width, pgm->maxValue);
}

void write_file(char *archive_name, uint8_t* pixels, int height, int width, int maxValue){
  FILE* pgmimg;
  pgmimg = fopen(archive_name, "wb");
  int dim = (width - 2) * (height - 2);
  fprintf(pgmimg, "P5\n%d %d\n%d\n", height - 2, width - 2, maxValue);
  size_t elements_written = fwrite(pixels, sizeof(uint8_t), dim, pgmimg);
  if (elements_written == dim) {
    printf("Se escribieron todos los elementos correctamente.\n");
  } else {
    printf("Hubo un error al escribir los elementos.\n");
  }
  fclose(pgmimg);
}