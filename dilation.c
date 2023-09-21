#include <cpuid.h>
#include <ctype.h>
#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "image.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, uint8_t* new_image);
void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, char* pos);
void dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int reg_size);
void write_file(char* name, uint8_t* pixels, int height, int width, int maxValue);

int main(int argc, char* argv[]) {
  int option, size_mmx = 32, dimension;
  char *inputImage = NULL, *secuentialOutputImage = NULL, *parallelOutputImage = NULL;
  uint8_t* new_image_s;
  uint8_t* new_image_p;
  PGMImage* image = malloc(sizeof(PGMImage));
  while ((option = getopt(argc, argv, "i:s:p:")) != -1) {
    switch (option) {
      case 'i':
        inputImage = optarg;
        break;
      case 's': // generated secuential image
        secuentialOutputImage = optarg;
        break;
      case 'p': // generated parallel image
        parallelOutputImage = optarg;
        break;
      case '?':
        exit(0);
      default:
        abort();
    }
  }
  printf("inputImage: %s\nsecuentialOutputImage: %s\nparallelOutputImage: %s\n",
  inputImage, secuentialOutputImage, parallelOutputImage);

  openPGM(image, inputImage);

  dimension = (image->height) * (image->width);

  new_image_s = malloc(dimension * sizeof(uint8_t)); // Secuential image version
  new_image_p = malloc(dimension * sizeof(uint8_t)); // Parallel image version

  dilate_image(image->pixels, image->width, image->height, new_image_s, 1);
  dilate_image(image->pixels, image->width, image->height, new_image_p, size_mmx);
  
  write_file(secuentialOutputImage, new_image_s, image->height, image->width, image->maxValue);
  write_file(parallelOutputImage, new_image_p, image->height, image->width, image->maxValue);

  free(new_image_s);
  free(new_image_p);
}

void dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int reg_size) {
  int puntero = 0;
  clock_t start_s = clock();
  for (int i = 1; i < height - 1; i++) {
    for (int j = 1; j < width - 1; j = j + reg_size) {
      int pixel_pos = (width * i) + j;
      if (reg_size == 1) {
        dilation_secuential_pixel(input_image, width, pixel_pos, new_image);
      }
      else{
        char* pos = new_image + puntero;
        dilation_parallel_pixel(input_image, width, pixel_pos, pos);
        puntero += reg_size;
      }
    }
  }
  clock_t end_s = clock();
  double t_s = ((double)(end_s - start_s)) / CLOCKS_PER_SEC;
  if (reg_size == 1) {
    printf("T.Secuencial: %.2f[ms]\n", t_s * 100); // s * 100: ms
  }
  else{
    printf("T.Paralelo  : %.2f[ms]\n", t_s * 100); // s * 100: ms
  }
}

void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, uint8_t* new_image) {
  new_image[pixel_position] = MAX(
      MAX(MAX(input_image[pixel_position + 1], input_image[pixel_position - 1]),
      MAX(input_image[pixel_position + width], input_image[pixel_position - width])),input_image[pixel_position]);
  
}

void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, char* pos) {
  __m256i r1, r2, r3, r4, r5, max_px;
  char* p1 = input_image + pixel_position - width; // up
  char* p2 = input_image + pixel_position - 1; // left
  char* p3 = input_image + pixel_position; // center
  char* p4 = input_image + pixel_position + 1; // right
  char* p5 = input_image + pixel_position + width; // down
  r1 = _mm256_loadu_si256((__m256i*) p1);
  r2 = _mm256_loadu_si256((__m256i*) p2);
  r3 = _mm256_loadu_si256((__m256i*) p3);
  r4 = _mm256_loadu_si256((__m256i*) p4);
  r5 = _mm256_loadu_si256((__m256i*) p5);
  max_px = _mm256_max_epu8(_mm256_max_epu8(_mm256_max_epu8(r1, r2), _mm256_max_epu8(r3, r4)), r5);
  _mm256_storeu_si256((__m256i*)pos, max_px);
}

void write_file(char* archive_name, uint8_t* pixels, int height, int width, int maxValue) {
  FILE* pgmimg;
  pgmimg = fopen(archive_name, "wb");
  int dim = width * height;

  fprintf(pgmimg, "P5\n%d %d\n%d\n", height, width, maxValue);
  size_t elements_written = fwrite(pixels, sizeof(uint8_t), dim, pgmimg);

  if (elements_written == dim) {
    printf("Se escribieron todos los elementos correctamente.\n");
  } else {
    printf("Hubo un error al escribir los elementos.\n");
  }
  fclose(pgmimg);
}