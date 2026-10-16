#include <stdio.h>
#include <math.h>
#include <omp.h>

double f(double x)
{
    return sin(x);
}

double trapecio_parallel(double a, double b, int n)
{
    double h = (b - a) / n;
    double sum = 0.5 * (f(a) + f(b));

    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        sum += f(x);
    }

    return h * sum;
}

int main()
{
    double a = 0.0, b = M_PI;
    int n = 1000000;

    double t0 = omp_get_wtime();
    double resultado = trapecio_parallel(a, b, n);
    double t1 = omp_get_wtime();

    printf("Resultado (paralelo): %.10f\n", resultado);
    printf("Tiempo: %.5f segundos\n", t1 - t0);

    return 0;
}
