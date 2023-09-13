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
#include <xmmintrin.h> /* SSE __m256 float */

#include "image.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, int index, uint8_t* new_image);
void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, int index, uint8_t* new_image);
double dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int gr);
void write_file(char* name, uint8_t* pixels, int height, int width, int maxValue);

int main(int argc, char* argv[]) {
  int option, imageWidth = 0, size_mmx = 32, index_parallel = 0, dimension, index_secuential = 0, its = 1;
  char *inputImage = NULL, *secuentialOutputImage = NULL, *parallelOutputImage = NULL;
  uint8_t* new_image_s;
  uint8_t* new_image_p;
  PGMImage* image = malloc(sizeof(PGMImage));
  while ((option = getopt(argc, argv, "i:s:p:N:t:")) != -1) {
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
      case 't':  // numero de iteraciones
        sscanf(optarg, "%d", &its);
        break;
      case '?':
        exit(0);
      default:  // Si no se ha ingresado alguna flag obligatoria, se aborta
        abort();
    }
  }
  printf("inputImage: %s\nsecuentialOutputImage: %s\nparallelOutputImage: %s\nimageWidth: %d\n",
  inputImage, secuentialOutputImage, parallelOutputImage, imageWidth);
  openPGM(image, inputImage);

  dimension = (image->height) * (image->width);
  printf("Numero de iteraciones: %d\n", its);
  for (int h = 0; h < its; h++) {
    printf("It[%d]\n", h + 1);
    new_image_s = (uint8_t*)malloc(dimension * sizeof(uint8_t));
    new_image_p = (uint8_t*)malloc(dimension * sizeof(uint8_t));
    
    dilate_image(image->pixels, image->width, image->height, new_image_s, 1);
    dilate_image(image->pixels, image->width, image->height,new_image_p, size_mmx);
  }
  write_file(secuentialOutputImage, new_image_s, image->height - 2, image->width - 2, image->maxValue);
  write_file(parallelOutputImage, new_image_p, image->height, image->width, image->maxValue);
  free(new_image_s);
  free(new_image_p);
}

void write_file(char* archive_name, uint8_t* pixels, int height, int width, int maxValue) {
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

void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, int index, uint8_t* new_image) {
  new_image[index] = MAX(
      MAX(MAX(input_image[pixel_position + 1], input_image[pixel_position - 1]), MAX(input_image[pixel_position + width], input_image[pixel_position - width])),input_image[pixel_position]);
}

void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, int index, uint8_t* new_image) {
  __m256i r1, r2, r3, r4, r5, max_px;
  char* p1 = input_image + pixel_position - width;
  char* p2 = input_image + pixel_position - 1;
  char* p3 = input_image + pixel_position;
  char* p4 = input_image + pixel_position + 1;
  char* p5 = input_image + pixel_position + width;
  r1 = _mm256_loadu_si256((__m256i*)p1);
  r2 = _mm256_loadu_si256((__m256i*)p2);
  r3 = _mm256_loadu_si256((__m256i*)p3);
  r4 = _mm256_loadu_si256((__m256i*)p4);
  r5 = _mm256_loadu_si256((__m256i*)p5);
  max_px = _mm256_max_epu8(_mm256_max_epu8(_mm256_max_epu8(r1, r2), _mm256_max_epu8(r3, r4)), r5);
  _mm256_store_si256((__m256i*)(new_image + index), max_px);
}

double dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int gr) {
  int index = 0;
  clock_t start_s = clock();
  for (int i = 1; i < height - 1; i++) {
    int row = width * i;
    for (int j = 1; j < width - 1; j = j + gr) {
      int pixel_pos = row + j;
      if (gr == 1) {
        dilation_secuential_pixel(input_image, width, pixel_pos, index, new_image);
      }
      else{
        dilation_parallel_pixel(input_image, width, pixel_pos, index, new_image);
      }
      index += gr;
    }
  }
  clock_t end_s = clock();
  double t_s = ((double)(end_s - start_s)) / CLOCKS_PER_SEC;
  if (gr == 1) {
    printf("T.Secuencial: %.10f[s]\n", t_s);
  }
  else{
    printf("T.Paralelo  : %.10f[s]\n", t_s);
  }
  return t_s;
}