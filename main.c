/*
 *      AUTORES: Enrique Viqueira y Laura Silva
 *      PRACTICA 1
 *      FECHA: 22/03/23
 *
 * */

#include <stdio.h>
#include <pmmintrin.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

/// Constantes

#define S1 (64 * 8)     // S1 --> nº de lineas cache que caben en la cache L1 de datos
#define S2 (1024 * 4)   // S2 --> nº de lineas cache que caben en la cache L2
#define CLS 64          // tamaño en bytes de la linea cache

/// Funciones para calcular los ciclos de reloj
void start_counter();
double get_counter();
double mhz();

/* Initialize the cycle counter */
static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;


// Programa Principal
int main(int argc, char * argv[]) {
    /// Variables
    double *A;          // vector de nºs aleatorios en el rango [1,2)
    int *ind;           // vector que hace referencia a los elementos del tipo A[ind[i]], con ind[i] = 0, D, 2D, 3D...
    int R;              // numero de elementos del vector A a los que se accede
    int D;              // numero por el que vamos a multiplicar los R numeros del array
    // numero de linas cache que se leeran en el acceso al vector A
    int L[] = {0.5*S1,1.5*S1, 0.5*S2, 0.75*S2, 2*S2, 4*S2, 8*S2};
    double S[10];       // vector donde se almacenaran los resultados de cada repetición
    double suma;        // variable auxiliar para almacenar las reducciones

    double ck;          // tiempo de las 10 reducciones
    double ck_reloj;    // tiempo de los ciclos de reloj

    /// Comprobación de que se pasan el número de parametros correcto
    // El programa debe aceptar 2 parámetros como argumentos de entrada, correspondientes con los valores de D y L
    if(argc != 3)
    {
        perror("Número de argumentos introducido incorrecto.\n");
        printf("Formato correcto: %s <valor_D> <valor_L>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // inicializamos los parámetros con los argumentos de entrada que se nos proporcionan
    D = atoi(argv[1]);
    // valor para R
    if(D <= CLS)
    {
        R = ceil(L[atoi(argv[2])] * CLS / D);
    }
    else
    {
        R = L[atoi(argv[2])];
    }

    /// 1. Reservas de memoria
    ind = (int*)malloc(R * sizeof(int));
    if(ind == NULL) // comprobar que se está reservando memoria correctamente
    {
        perror("Hay un error en el malloc de ind");
        exit(EXIT_FAILURE);
    }
    A = (double*) _mm_malloc(R * D * sizeof(double), CLS);
    if(A == NULL)// comprobar que se está reservando memoria correctamente
    {
        perror("Hay un error en el _mm_malloc de A");
        exit(EXIT_FAILURE);
    }

    /// 2. Inicializamos los elementos del vector ind
    for(int i = 0; i < R ; i++)
    {
        ind[i] = i * D;
    }

    /// 3. Inicializamos los elementos del vector A con elementos aleatorios
    srand(time(NULL));  // Instrucción que inicializa el generador de números aleatorios
    for(int i = 0; i < R*D ; i++)
    {
        A[ind[i]] = ((double)rand()/RAND_MAX + 1) * pow(-1, rand() %2);
        printf(" %0.2lf\n", A[i]);
    }
    printf("\n\n");

    /// 4. Calculamos las reducciones y las guardamos en el array
    start_counter();    // inicio de la medición del tiempo
    for(int i = 0; i < 10; i++)
    {
        suma = 0;
        for(int k = 0; k < R; k++)
        {
            suma = suma + A[ind[k]];    // sumar estos R elementos almacenando el valor en una variable
        }
        S[i] = suma;
    }
    ck = get_counter(); // paramos la medicion

    /// 5. Analizamos los resultados obtenidos
    printf("El tiempo de 10 reducciones en ciclos de reloj = %1.10lf\n", ck);
    printf("Resultado:\n");
    for(int i = 0; i < 10; i++)
    {
        printf("\tS[%d] = %0.2lf\n", i, S[i]);
    }



    return 0;
}

/// FUNCIONES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Set *hi and *lo to the high and low order bits of the cycle counter.
Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
            : "=r" (*hi), "=r" (*lo) /* and move results to */
            : /* No input */ /* the two outputs */
            : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double) hi * (1 << 30) * 4 + lo;
    if (result < 0) {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6*sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}