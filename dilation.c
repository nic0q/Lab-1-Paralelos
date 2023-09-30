#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> // uint8_8: unsigned char
#include <time.h> // clock
#include <immintrin.h> // mmx256
#include <unistd.h> // getopt
#include <getopt.h>

#include "image.h"
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, uint8_t* new_image, int puntero);
void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, char* pos);
void dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int reg_size);
void write_file(char* name, uint8_t* pixels, int height, int width, int maxValue);
void cut_2_rightpx(uint8_t* img, int width, int height, int n_pixels);

int main(int argc, char* argv[]) {
  int option, size_mmx = 32, dimension, adjusted_height, adjusted_width;
  char *inputImage = NULL, *secuentialOutputImage = NULL, *parallelOutputImage = NULL;
  uint8_t* new_image_s;
  uint8_t* new_image_p;
  PGMImage* image = malloc(sizeof(PGMImage));
  while ((option = getopt(argc, argv, "i:s:p:")) != -1) {
    switch (option) {
      case 'i':
        inputImage = optarg;
        break;
      case 's':
        secuentialOutputImage = optarg;
        break;
      case 'p':
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

  adjusted_width = image->height - 2;
  adjusted_height = image->width - 2;

  dimension = image->height * image->width;

  new_image_s = malloc(dimension * sizeof(uint8_t)); // Secuential image version
  new_image_p = malloc(dimension * sizeof(uint8_t)); // Parallel image version
  
  dilate_image(image->pixels, image->width, image->height, new_image_s, 1);
  dilate_image(image->pixels, image->width, image->height, new_image_p, size_mmx);
  
  // cut two right pixels side to make secuential and parallel equal images
  cut_2_rightpx(new_image_p, image->width, image->height, 2);

  write_file(secuentialOutputImage, new_image_s, adjusted_height, adjusted_width, image->maxValue);
  write_file(parallelOutputImage, new_image_p, adjusted_height, adjusted_width, image->maxValue);

  free(new_image_s);
  free(new_image_p);
}

/**
 * Applies a dilation operation to an image.
 *
 * @param input_image   Pointer to the input image.
 * @param width         The width of the image.
 * @param height        The height of the image.
 * @param new_image     Pointer to the output image where the dilation result will be stored.
 * @param reg_size      Neighborhood region size for dilation.
 *                     If reg_size is 1, sequential dilation is performed.
 *                     Otherwise, parallel dilation is performed with the specified region size.
 */
void dilate_image(uint8_t* input_image, int width, int height, uint8_t* new_image, int reg_size) {
  int puntero = 0;
  clock_t start_s = clock();

  for (int i = 1; i < height - 1; i++) {
    int row = width * i;
    for (int j = 1; j < width - 1; j = j + reg_size) {
      int pixel_pos = row + j;
      if (reg_size == 1) {
        dilation_secuential_pixel(input_image, width, pixel_pos, new_image, puntero);
      } else {
        char* pos = new_image + puntero;
        dilation_parallel_pixel(input_image, width, pixel_pos, pos);
      }
      puntero += reg_size;
    }
  }

  clock_t end_s = clock();
  double t_s = ((double)(end_s - start_s)) / CLOCKS_PER_SEC;

  if (reg_size == 1) {
    printf("Sequential Time: %.2f[ms]\n", t_s * 100);
  } else {
    printf("Parallel Time: %.2f[ms]\n", t_s * 100);
  }
}

/**
 * Applies a sequential dilation operation to a pixel in an image.
 *
 * @param input_image      Pointer to the input image.
 * @param width            The width of the image.
 * @param pixel_position   The position of the current pixel in the image.
 * @param new_image        Pointer to the output image where the dilation result will be stored.
 */
void dilation_secuential_pixel(uint8_t* input_image, int width, int pixel_position, uint8_t* new_image, int puntero) {
  uint8_t max_value = MAX(
      MAX(MAX(input_image[pixel_position + 1], input_image[pixel_position - 1]),
      MAX(input_image[pixel_position + width], input_image[pixel_position - width])), input_image[pixel_position]);
  new_image[puntero] = max_value;
}

/**
 * Applies a parallel dilation operation to a pixel in an image using SIMD (AVX2) instructions.
 *
 * @param input_image      Pointer to the input image.
 * @param width            The width of the image.
 * @param pixel_position   The position of the current pixel in the image.
 * @param pos              Pointer to the location in the output image where the dilation result will be stored.
 */
void dilation_parallel_pixel(uint8_t* input_image, int width, int pixel_position, char* pos) {
  __m256i r1, r2, r3, r4, r5, max_px;

  char* p1 = input_image + pixel_position - width; // Up
  char* p2 = input_image + pixel_position - 1; // Left
  char* p3 = input_image + pixel_position; // Center
  char* p4 = input_image + pixel_position + 1; // Right
  char* p5 = input_image + pixel_position + width; // Down

  r1 = _mm256_loadu_si256((__m256i*) p1);
  r2 = _mm256_loadu_si256((__m256i*) p2);
  r3 = _mm256_loadu_si256((__m256i*) p3);
  r4 = _mm256_loadu_si256((__m256i*) p4);
  r5 = _mm256_loadu_si256((__m256i*) p5);

  max_px = _mm256_max_epu8(_mm256_max_epu8(_mm256_max_epu8(r1, r2), _mm256_max_epu8(r3, r4)), r5);

  _mm256_storeu_si256((__m256i*) pos, max_px);
}

/**
 * Writes an image in PGM (Portable Graymap) format to a binary file.
 *
 * @param archive_name   The name of the file where the image will be written.
 * @param pixels         Pointer to the image's pixels.
 * @param height         The height of the image.
 * @param width          The width of the image.
 * @param maxValue       The maximum pixel value in the image.
 */
void write_file(char* archive_name, uint8_t* pixels, int height, int width, int maxValue) {
  FILE* pgmimg;
  pgmimg = fopen(archive_name, "wb");
  int dim = width * height;

  fprintf(pgmimg, "P5\n%d %d\n%d\n", width, height, maxValue);

  size_t elements_written = fwrite(pixels, sizeof(uint8_t), dim, pgmimg);

  if (elements_written == dim) {
    printf("All elements were written successfully.\n");
  } else {
    printf("There was an error while writing the elements.\n");
  }

  fclose(pgmimg);
}

/**
 * Writes an image in PGM (Portable Graymap) format to a binary file.
 *
 * @param img            Pointer to the image's pixels.
 * @param width          The width of the image.
 * @param height         The height of the image.
 * @param n_pixels       Number of pixels to cut from rigth
 */
void cut_2_rightpx(uint8_t* img, int width, int height, int n_pixels) {
  int new_width = width - n_pixels;
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < new_width; j++) {
      img[i * new_width + j] = img[i * height + j];
    }
  }
}
