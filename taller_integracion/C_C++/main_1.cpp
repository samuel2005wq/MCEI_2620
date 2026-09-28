// ejercicio1_gsl.cpp
// ------------------------------------------------------------
// Ejercicio 1 - Parte C (C/C++ con GSL)
// f(x) = e^{-0.4x}(1 + 0.5 sin(3x)),  0 <= x <= 8
//
// Compilar:
//   g++ ejercicio1_gsl.cpp -o ejercicio1_gsl -lgsl -lgslcblas -lm
// Ejecutar:
//   ./ejercicio1_gsl
// ------------------------------------------------------------
#include <cstdio>
#include <cmath>
#include <chrono>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>

// Contador de evaluaciones de f (se pasa por params)
struct Contador
{
    long evals;
};

// Funcion a integrar
double f(double x, void *params)
{
    if (params)
        ((Contador *)params)->evals++;
    return std::exp(-0.4 * x) * (1.0 + 0.5 * std::sin(3.0 * x));
}

// Antiderivada analitica (referencia exacta)
double F(double x)
{
    return -2.5 * std::exp(-0.4 * x) + (0.5 / 9.16) * std::exp(-0.4 * x) * (-0.4 * std::sin(3.0 * x) - 3.0 * std::cos(3.0 * x));
}

// Regla del trapecio compuesta (implementacion explicita)
double trapecio(double a, double b, int n)
{
    double h = (b - a) / n;
    double s = 0.5 * (f(a, NULL) + f(b, NULL));
    for (int i = 1; i < n; i++)
        s += f(a + i * h, NULL);
    return h * s;
}

int main()
{
    const double a = 0.0, b = 8.0;
    const double I_ref = F(b) - F(a);

    // ============ 1. Cuadratura adaptativa GSL (QAGS) ============
    const size_t limit = 1000;   // max. de subintervalos
    const double epsabs = 1e-10; // tolerancia absoluta
    const double epsrel = 1e-10; // tolerancia relativa

    gsl_set_error_handler_off(); // manejamos el codigo de retorno nosotros
    gsl_integration_workspace *w = gsl_integration_workspace_alloc(limit);

    Contador cont = {0};
    gsl_function G;
    G.function = &f;
    G.params = &cont;

    double resultado = 0.0, error_est = 0.0;
    int status = gsl_integration_qags(&G, a, b, epsabs, epsrel, limit, w, &resultado, &error_est);
    long evals = cont.evals;
    size_t subint = w->size; // subintervalos usados por el algoritmo

    // Tiempo promedio de QAGS (funcion sin contador para no distorsionar)
    gsl_function G2;
    G2.function = &f;
    G2.params = NULL;
    const int R = 20000;
    double tmp, tmp_err;
    auto t0 = std::chrono::steady_clock::now();
    for (int r = 0; r < R; r++)
        gsl_integration_qags(&G2, a, b, epsabs, epsrel, limit, w, &tmp, &tmp_err);
    auto t1 = std::chrono::steady_clock::now();
    double t_qags = std::chrono::duration<double>(t1 - t0).count() / R;

    printf("Referencia analitica : % .15f\n\n", I_ref);
    printf("=== GSL: gsl_integration_qags ===\n");
    printf("Estado (0 = OK)      : %d (%s)\n", status, gsl_strerror(status));
    printf("Resultado            : % .15f\n", resultado);
    printf("Error estimado       : % .3e\n", error_est);
    printf("Error real           : % .3e\n", std::fabs(resultado - I_ref));
    printf("Tolerancia absoluta  : % .1e\n", epsabs);
    printf("Tolerancia relativa  : % .1e\n", epsrel);
    printf("Subintervalos usados : %zu\n", subint);
    printf("Evaluaciones de f    : %ld\n", evals);
    printf("Tiempo promedio      : % .3e s\n\n", t_qags);

    // ============ 2. Trapecio explicito ============
    const int n_vals[] = {10, 20, 50, 100, 500, 1000};
    const int R2 = 20000;

    printf("=== Trapecio compuesto (implementacion explicita) ===\n");
    printf("%6s %22s %14s %14s\n", "n", "Integral aproximada", "Error", "Tiempo (s)");
    for (int k = 0; k < 6; k++)
    {
        int n = n_vals[k];
        double I = trapecio(a, b, n);

        volatile double sink = 0.0; // evita que el compilador elimine el bucle
        auto s0 = std::chrono::steady_clock::now();
        for (int r = 0; r < R2; r++)
            sink = trapecio(a, b, n);
        auto s1 = std::chrono::steady_clock::now();
        double t = std::chrono::duration<double>(s1 - s0).count() / R2;
        (void)sink;

        printf("%6d %22.12f %14.4e %14.4e\n", n, I, std::fabs(I - I_ref), t);
    }

    gsl_integration_workspace_free(w);
    return 0;
}