#include <cpuid.h> /* __get_cpuid_max, __get_cpuid */
#include <ctype.h>
#include <mmintrin.h> /* MMX instrinsics __m64 integer type */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <stdlib.h>
#include <string.h> /* memcpy */
#include <sys/types.h>
#include <unistd.h>
#include <xmmintrin.h> /* SSE __m128 float */
#include <immintrin.h>

long toBin(short);

// Structure for storing the
// image data
typedef struct PGMImage {
  char pgmType[3];
  short **data;
  unsigned int width;
  unsigned int height;
  unsigned int maxValue;
} PGMImage;

// Function to ignore any comments
// in file
void ignoreComments(FILE* fp) {
  int ch;
  char line[100];

  // Ignore any blank lines
  while ((ch = fgetc(fp)) != EOF && isspace(ch))
    ;

  // Recursively ignore comments
  // in a PGM image commented lines
  // start with a '#'
  if (ch == '#') {
    fgets(line, sizeof(line), fp);
    ignoreComments(fp);
  } else
    fseek(fp, -1, SEEK_CUR);
}

// Function to open the input a PGM
// file and process it
bool openPGM(PGMImage* pgm, const char* filename) {
  // Open the image file in the
  // 'read binary' mode
  FILE* pgmfile = fopen(filename, "rb");

  // If file does not exist,
  // then return
  if (pgmfile == NULL) {
    printf("File does not exist\n");
    return false;
  }

  ignoreComments(pgmfile);
  fscanf(pgmfile, "%s", pgm->pgmType);

  // Check for correct PGM Binary
  // file type
  if (strcmp(pgm->pgmType, "P5")) {
    fprintf(stderr, "Wrong file type!\n");
    exit(EXIT_FAILURE);
  }

  ignoreComments(pgmfile);

  // Read the image dimensions
  fscanf(pgmfile, "%d %d", &(pgm->width), &(pgm->height));

  ignoreComments(pgmfile);

  // Read maximum gray value
  fscanf(pgmfile, "%d", &(pgm->maxValue));
  ignoreComments(pgmfile);

  // Allocating memory to store
  // img info in defined struct
  pgm->data = malloc(pgm->height * sizeof(short*));

  // Storing the pixel info in
  // the struct
  if (pgm->pgmType[1] == '5') {
    fgetc(pgmfile);

    for (int i = 0; i < pgm->height; i++) {
      pgm->data[i] = malloc(pgm->width * sizeof(short));
      // If memory allocation
      // is failed
      if (pgm->data[i] == NULL) {
        fprintf(stderr, "malloc failed\n");
        exit(1);
      }
      // Read the gray values and
      // write on allocated memory
      fread(pgm->data[i], sizeof(short), pgm->width, pgmfile);
    }
  }
  // Close the file
  fclose(pgmfile);
  return true;
}

// Function to print the file details
void printImageDetails(PGMImage* pgm, const char* filename) {
  FILE* pgmfile = fopen(filename, "rb");

  // Retrieving the file extension
  char* ext = strrchr(filename, '.');

  if (!ext)
    printf(
        "No extension found"
        "in file %s",
        filename);
  else
    printf(
        "File format"
        "    : %s\n",
        ext + 1);

  printf("PGM File type  : %s\n", pgm->pgmType);

  // Print type of PGM file, in ascii
  // and binary format
  if (!strcmp(pgm->pgmType, "P2"))
    printf(
        "PGM File Format:"
        "ASCII\n");
  else if (!strcmp(pgm->pgmType, "P5"))
    printf(
        "PGM File Format:"
        " Binary\n");

  printf("Width of img   : %d px\n", pgm->width);
  printf("Height of img  : %d px\n", pgm->height);
  printf("Max Gray value : %d\n", pgm->maxValue);

  // close file
  fclose(pgmfile);
}

int main(int argc, char* argv[]) {
  int option, imageWidth = 0;
  char *inputImage = NULL, *outputImage1 = NULL, *outputImage2 = NULL;
  char buffer[100];
  short int *salida;
  salida = malloc(510 * 510 * sizeof(short int*));
  int indice_r = 0, max_vl = 7, size_r = 8, is[8], js[8], indice = 0, maxe, maxed;
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
      default:  // En caso que no se haya ingresado alguna flag obligatoria, se
                // aborta la ejecucion
        abort();
    }
  }
  printf("inputImage: %s\n", inputImage);
  printf("outputImage1: %s\n", outputImage1);
  printf("outputImage2: %s\n", outputImage2);
  printf("imageWidth: %d\n", imageWidth);
  PGMImage* pgm = malloc(sizeof(PGMImage));
  FILE* pgmimg;
  pgmimg = fopen(outputImage2, "w");
  fprintf(pgmimg, "P5\n");
  fprintf(pgmimg, "%d %d\n", 512, 512);
  fprintf(pgmimg, "255\n");
  if (openPGM(pgm, inputImage)) {
    printImageDetails(pgm, inputImage);
  }
  __m128i r1, r2, r3, r4, r5, max_px;
  short int A[8] __attribute__((aligned(16))) = {-1,-1,-1,-1,-1,-1,-1,-1};
  short int B[8] __attribute__((aligned(16))) = {-1,-1,-1,-1,-1,-1,-1,-1};
  short int C[8] __attribute__((aligned(16))) = {-1,-1,-1,-1,-1,-1,-1,-1};
  short int D[8] __attribute__((aligned(16))) = {-1,-1,-1,-1,-1,-1,-1,-1};
  short int E[8] __attribute__((aligned(16))) = {-1,-1,-1,-1,-1,-1,-1,-1};
  short int max[8];
  
  for(int i = 1; i < pgm->height - 1; i++){
    for(int j = 1; j < pgm->width - 1; j++){
      A[indice_r] = *pgm->data[i-1, j];
      B[indice_r] = *pgm->data[i, j-1];
      C[indice_r] = *pgm->data[i, j];
      D[indice_r] = *pgm->data[i, j+1];
      E[indice_r] = *pgm->data[i+1, j];
      if(indice_r == max_vl){
        indice_r = 0;
        r1 = _mm_loadu_si128((__m128i*) A);
        r2 = _mm_loadu_si128((__m128i*) B);
        r3 = _mm_loadu_si128((__m128i*) C);
        r4 = _mm_loadu_si128((__m128i*) D);
        r5 = _mm_loadu_si128((__m128i*) E);
        memset(A, -1, sizeof(A));
        memset(B, -1, sizeof(B));
        memset(C, -1, sizeof(C));
        memset(D, -1, sizeof(D));
        memset(E, -1, sizeof(E));
        max_px = _mm_max_epi16(_mm_max_epi16(_mm_max_epi16(_mm_max_epi16(r1, r2), r3), r4), r5);
        _mm_store_si128((__m128i*)max, max_px);
        for(int h = 0; h < 8; h++){
          if(indice == 510 * 510){
            printf("AAAAAA");
            return 0;
          }
          salida[indice] = max[h];
          indice++;
        }
      }
      else{
        indice_r++;
      }
      maxed = j;
    }
    maxe = i;
  }
  
  printf("%d %d", maxe, maxed);
  fwrite(salida, sizeof(short int),510 * 510, pgmimg);
  fclose(pgmimg);
}
