FLAGS = -Ofast# optimize the code
MMX_FLAGS = -msse4 -mavx2

all: dilation

dilation: dilation.c image.o
	gcc $(FLAGS) $(MMX_FLAGS) -fopenmp image.o dilation.c  -o dilation

image.o: image.c image.h
	gcc $(FLAGS) -c image.c

clean:
	rm -f *.o *.exe imagen_salida1.pgm imagen_salida_secuencial.pgm imagen_salida_paralela.pgm imagen_salida_secuencial2.pgm imagen_salida_paralela2.pgm dilation

test_bike:
	./dilation -i bike.pgm -s imagen_salida_secuencial.pgm -p imagen_salida_paralela.pgm

test_lines:
	./dilation -i lines.pgm -s imagen_salida_secuencial2.pgm -p imagen_salida_paralela2.pgm
