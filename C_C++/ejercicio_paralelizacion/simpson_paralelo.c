#include <stdio.h>
#include <math.h>
#include <omp.h>

double f(double x)
{
    return sin(x);
}

double simpson_parallel(double a, double b, int n)
{
    if (n % 2 != 0) {
        n++;
    }

    double h = (b - a) / n;
    double sum = f(a) + f(b);
    double suma_par = 0.0, suma_impar = 0.0;

    #pragma omp parallel for reduction(+:suma_par, suma_impar)
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        if (i % 2 == 0) {
            suma_par += f(x);
        } else {
            suma_impar += f(x);
        }
    }

    return (h / 3.0) * (sum + 2 * suma_par + 4 * suma_impar);
}

int main()
{
    double a = 0.0, b = M_PI;
    int n = 1000000;

    double t0 = omp_get_wtime();
    double result = simpson_parallel(a, b, n);
    double t1 = omp_get_wtime();

    printf("Resultado (paralelo): %.10f\n", result);
    printf("Tiempo: %.5f segundos\n", t1 - t0);

    return 0;
}
