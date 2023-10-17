## Lab 1 SIMD - Paralelismo con SSE
#### Contexto
(Para entender el problema lea el enunciado *lab1.pdf*)

#### Resultados
### Image
![image](https://github.com/nic0q/SSE-MMX-registers-c/assets/91075814/cae2ee05-3b5a-407f-b164-a713e6b6ab46)

### Dilation
![image](https://github.com/nic0q/SSE-MMX-registers-c/assets/91075814/54d7f676-51bf-4467-853c-056f54d40e16)

#### Estrategia de paralelización
Utilicé las funciones intrínsecas de intel para registros mmx de **256bits** tales como: **_mm256_store_si256**, **_mm256_max_epu8**, **_mm256_loadu_si256**.
Nota: No fue posible implementar esta solución con registros de 512bits dado que mi pc no soporta la familia AVX-512.

Dado que las funciones de **store** y **load** reciben un puntero a una dirección de memoria es posible simplemente sumar un (contador o puntero) para seguir almacenando en la posición deseada, dado que estamos almacenando la imagen en un arreglo unidimensional para que esta estrategia funcione, la posición del pixel está dada por **Arreglo[Width * i + j]**

#### Especificaciones
Programa creado en Windows 10, con compilador gcc versión: **12.1.0**

------------
#### Para compilar usar:

		$ make

#### Para ejecutar:		

	$ .\dilation.exe -i [string.pgm] -s [string.pgm] -p [string.pgm] -N [entero]

Donde:
- -i: Imagen .pgm de entrada
-  -s: Nombre imagen salida secuencial
- -p: Nombre imagen salida paralela
- -N: Ancho de la imagen

##### Por ejemplo:
	.\dilation.exe -i bike.pgm -s imagen_salida_secuencial.pgm -p imagen_salida_paralela.pgm -N 256
#### Comandos de ejemplo para las imágenes bike.pgm y lines.pgm

		$ make test_bike
		$ make test_lines

 #### Se puede probar que el resultado es el mismo haciendo

		$ diff imagen_salida_paralela.pgm imagen_salida_secuencial.pgm
  
Y si no arroja nada las imágenes son idénticas, es decir que se obtiene el mismo resultado.

#### Links relevantes

##### Para ver la imagen utilicé
https://bytes.usc.edu/~saty/tools/PGMViewer/viewer.html

##### Documentación de Intel Intrinsics
https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html#ig_expand=28,11,4327,4141&techs=MMX,SSE_ALL,AVX_ALL&text=_mm256
