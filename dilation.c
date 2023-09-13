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
  int option, imageWidth = 0, size_r = 16, pos = 0, indy = 0;
  char *inputImage = NULL, *secuentialOutputImage = NULL, *parallelOutputImage = NULL;
  uint8_t A[size_r], B[size_r], C[size_r], D[size_r], E[size_r], max[size_r];
  __m128i r1, r2, r3, r4, r5, max_px;
  PGMImage* pgm = malloc(sizeof(PGMImage));
  while ((option = getopt(argc, argv, "i:s:p:N:")) != -1) {
    switch (option) {
      case 'i':  // nombre del archivo de entrada
        inputImage = optarg;
        break;
      case 's':  // nombre del archivo de salida
        secuentialOutputImage = optarg;
        break;
      case 'p':  // nombre del archivo de salida
        parallelOutputImage = optarg;
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
  printf("inputImage: %s\nsecuentialOutputImage: %s\nparallelOutputImage: %s\nimageWidth: %d\n",
         inputImage, secuentialOutputImage, parallelOutputImage, imageWidth);
  openPGM(pgm, inputImage);

  int adjustedWidth = (pgm->width / size_r) * size_r;
  int dim = (pgm->height) * (adjustedWidth);
  int its = 10;
  uint8_t* pixels_s;
  uint8_t* pixels_p;
  printf("Numero de iteraciones: %d\n", 10);
  for(int h = 0; h < its; h++){
    printf("It[%d]\n", h);
    pixels_s = (uint8_t*)malloc(dim * sizeof(uint8_t));
    pixels_p = (uint8_t*)malloc(dim * sizeof(uint8_t));
    int pos = 0, indy = 0;
    clock_t start_s = clock();
    for (int i = 1; i < pgm->height - 1; i++) {
      for (int j = 1; j < pgm->width - 1; j++) {
        pixels_s[indy] = MAX(MAX(MAX(pgm->data[i][j + 1], pgm->data[i + 1][j]),MAX(pgm->data[i - 1][j], pgm->data[i][j - 1])),pgm->data[i][j]);
        indy++;
      }
    }
    clock_t end_s = clock();

    clock_t start_p = clock();
    for (int i = 1; i < pgm->height - 1; i++) {
      for (int j = 1; j < adjustedWidth; j += size_r) {
        char* p1 = pgm->pixs + (pgm->width * (i - 1) + j);
        char* p2 = pgm->pixs + (pgm->width * i + (j - 1));
        char* p3 = pgm->pixs + (pgm->width * i + j);
        char* p4 = pgm->pixs + (pgm->width * i + (j + 1));
        char* p5 = pgm->pixs + (pgm->width * (i + 1) + j);
        r1 = _mm_loadu_si128((__m128i*)p1);
        r2 = _mm_loadu_si128((__m128i*)p2);
        r3 = _mm_loadu_si128((__m128i*)p3);
        r4 = _mm_loadu_si128((__m128i*)p4);
        r5 = _mm_loadu_si128((__m128i*)p5);
        max_px = _mm_max_epu8(_mm_max_epu8(_mm_max_epu8(r1, r2), _mm_max_epu8(r3, r4)), r5);
        _mm_store_si128((__m128i*)(pixels_p + pos), max_px);
        pos += size_r;
      }
    }
    clock_t end_p = clock();
    double t_s = ((double) (end_s - start_s)) / CLOCKS_PER_SEC;
    double t_p = ((double) (end_p - start_p)) / CLOCKS_PER_SEC;
    printf("T.Secuencial: %.50f[s]\nT.Paralelo:   %.50f[s]\n", t_s, t_p);
  }
  write_file(secuentialOutputImage, pixels_s, pgm->height - 2, pgm->width - 2, pgm->maxValue);
  write_file(parallelOutputImage, pixels_p, pgm->height, pgm->width, pgm->maxValue);
  free(pixels_s);
  free(pixels_p);
}

void write_file(char *archive_name, uint8_t* pixels, int height, int width, int maxValue){
  FILE* pgmimg;
  pgmimg = fopen(archive_name, "wb");
  int dim = (width) * (height);
  fprintf(pgmimg, "P5\n%d %d\n%d\n", height, width, maxValue);
  size_t elements_written = fwrite(pixels, sizeof(uint8_t), dim, pgmimg);
  if (elements_written == dim) {
    printf("Se escribieron todos los elementos correctamente.\n");
  } else {
    printf("Hubo un error al escribir los elementos.\n");
  }
  fclose(pgmimg);
}