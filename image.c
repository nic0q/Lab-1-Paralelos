
#include "image.h"

void openPGM(PGMImage* pgm, const char* filename) {
  FILE* pgmfile = fopen(filename, "rb");
  if (pgmfile == NULL) {
    printf("File does not exist\n");
    exit(-1);
  }
  fscanf(pgmfile, "%s\n%d %d\n%d", pgm->pgmType,&(pgm->width), &(pgm->height),&(pgm->maxValue));
  pgm->pixels = (uint8_t*)malloc(pgm->height * pgm->width * sizeof(uint8_t));
  fgetc(pgmfile);
  fread(pgm->pixels, sizeof(uint8_t), pgm->width * pgm->height, pgmfile);
  fclose(pgmfile);
}