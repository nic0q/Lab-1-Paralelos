FLAGS = -O2# optimize the code

all: dilation

dilation: dilation.c image.o
	gcc $(FLAGS) image.o dilation.c -msse4 -o dilation

image.o: image.c image.h
	gcc $(FLAGS) -c image.c

clean:
	rm -f *.o *.exe

test:
	.\dilation.exe -i bike.pgm -s imagen_salida1.pgm -p imagen_salida2.pgm -N 3

test2:
	.\dilation.exe -i lines.pgm -s imagen_salida1.pgm -p imagen_salida3.pgm -N 3
