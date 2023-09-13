#include <stdint.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include "image.h"

void openPGM(PGMImage* pgm, const char* filename) {
  FILE* pgmfile = fopen(filename, "rb");
  FILE* pgmfile2 = fopen(filename, "rb");
  if (pgmfile == NULL) {
    printf("File does not exist\n");
    exit(-1);
  }
  fscanf(pgmfile, "%s", pgm->pgmType);
  fscanf(pgmfile, "%d %d", &(pgm->width), &(pgm->height));
  fscanf(pgmfile, "%d", &(pgm->maxValue));
  pgm->data = (uint8_t**)malloc(pgm->height * sizeof(uint8_t*));
  pgm->pixs = (uint8_t*)malloc(pgm->height * pgm->width * sizeof(uint8_t));
  fgetc(pgmfile);
  
  for (int i = 0; i < pgm->height; i++) {
    pgm->data[i] = (uint8_t*)malloc(pgm->width * sizeof(uint8_t));
    fread(pgm->data[i], sizeof(uint8_t), pgm->width, pgmfile2);
  }
  fread(pgm->pixs, sizeof(uint8_t), pgm->width * pgm->height, pgmfile);
  fclose(pgmfile);
}