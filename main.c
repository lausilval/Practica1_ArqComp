/*
 *      AUTORES: Enrique Viqueira y Laura Silva
 *      PRACTICA 1
 *      FECHA: 22/03/23
 *
 * */

#include <stdio.h>
#include <pmmintrin.h>
#include <time.h>

// Constantes
#define CLS 64      // tamaño en bytes de la linea cache

// Programa Principal
int main() {
    // Variables
    double *A;      // vector de nºs aleatorios en el rango [0,1]
    int *ind;       // vector que hace referencia a los elementos del tipo A[ind[i]], con ind[i] = 0, D, 2D, 3D...
    int R = 4;      // numero de elementos del vector
    int D = 2;      // numero por el que vamos a multiplicar los R numeros del array
    double S[10];   // vector donde se almacenaran los resultados de cada repetición
    double suma;    // variable auxiliar para almacenar las reducciones

    /// 1. Reservas de memoria
    ind = (int*)malloc(R * sizeof(int));
    A = (double*) _mm_malloc(R * D * sizeof(double), CLS);

    /// 2. Inicializamos los elementos del vector ind
    for(int i = 0; i < R ; i++)
    {
        ind[i] = i * D;
    }
    /// 3. Inicializamos los elementos del vector A con elementos aleatorios
    srand(time(NULL));  // Instrucción que inicializa el generador de números aleatorios
    for(int i = 0; i < R*D ; i++)
    {
        A[ind[i]] = (double)rand()/RAND_MAX;
        printf(" %0.2lf\n", A[i]);
    }
    printf("\n\n");
    /// 4. Calculamos las reducciones y las guardamos en el array
    for(int i = 0; i < 10; i++)
    {
        suma = 0;
        for(int k = 0; k < R; k++)
        {
            suma = suma + A[ind[k]];    // sumar estos R elementos almacenando el valor en una variable
        }
        S[i] = suma;
    }
    printf("Resultado:\n");
    for(int i = 0; i < 10; i++)
    {
        printf("%0.2lf\n", S[i]);
    }





    return 0;
}
