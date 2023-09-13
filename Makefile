FLAGS = -O2# optimize the code

all: dilation

dilation: dilation.c image.o
	gcc $(FLAGS) image.o dilation.c -msse4 -o dilation

image.o: image.c image.h
	gcc $(FLAGS) -c image.c

clean:
	rm -f *.o *.exe imagen_salida1.pgm imagen_salida2.pgm imagen_salida3.pgm imagen_salida4.pgm

test_bike:
	.\dilation.exe -i bike.pgm -s imagen_salida_secuencial.pgm -p imagen_salida_paralela.pgm -N 3 -t 10

test_lines:
	.\dilation.exe -i lines.pgm -s imagen_salida_secuencial2.pgm -p imagen_salida_paralela2.pgm -N 3 -t 10
