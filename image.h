#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct PGMImage {
  char pgmType[3];
  uint8_t* pixels;
  unsigned int width;
  unsigned int height;
  unsigned int maxValue;
} PGMImage;

void openPGM(PGMImage* pgm, const char* filename);

#endif // IMAGE_H