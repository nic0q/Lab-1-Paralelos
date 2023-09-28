#include "image.h"

/**
 * Opens a PGM (Portable Graymap) file and reads its contents into a PGMImage structure.
 *
 * @param pgm       Pointer to a PGMImage structure where the image data will be stored.
 * @param filename  The name of the PGM file to open.
 */
void openPGM(PGMImage* pgm, const char* filename) {
  FILE* pgmfile = fopen(filename, "rb");
  if (pgmfile == NULL) {
    printf("File does not exist\n");
    exit(-1);
  }
  if (fscanf(pgmfile, "%s\n%d %d\n%d", pgm->pgmType, &(pgm->width), &(pgm->height), &(pgm->maxValue)) != 4) {
    fprintf(stderr, "Error al leer el encabezado del archivo PGM.\n");
    exit(1);
  }
  pgm->pixels = (uint8_t*)malloc(pgm->height * pgm->width * sizeof(uint8_t));
  fgetc(pgmfile);
  if (fread(pgm->pixels, sizeof(uint8_t), pgm->width * pgm->height, pgmfile) != pgm->width * pgm->height) {
    fprintf(stderr, "Error al leer los píxeles del archivo PGM.\n");
    exit(1);
  }
  fclose(pgmfile);
}
