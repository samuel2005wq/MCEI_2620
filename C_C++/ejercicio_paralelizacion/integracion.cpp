// ------------------------------------------------------------
// Metodo del Trapecio y Simpson 1/3 para f(x) = sin(x)
// Intervalo: [0, pi]
// ------------------------------------------------------------
#include <iostream>
#include <cmath>
#include <chrono>

// Definir M_PI si no está disponible
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Función a integrar
double f(double x)
{
    return std::sin(x);
}

// Antiderivada analitica de sin(x) es -cos(x) para la referencia exacta
double F(double x)
{
    return -std::cos(x);
}

// 1. Regla del Trapecio Compuesta (requiere n par o impar, idealmente n par para comparar)
double trapecio_compuesto(double a, double b, int n)
{
    double h = (b - a) / n;
    double s = 0.5 * (f(a) + f(b));
    for (int i = 1; i < n; i++)
    {
        s += f(a + i * h);
    }
    return h * s;
}

// 2. Regla de Simpson 1/3 Compuesta (requiere n par)
double simpson_compuesto(double a, double b, int n)
{
    if (n % 2 != 0)
    {
        n++; // Asegurar que n sea par
    }
    double h = (b - a) / n;
    double s = f(a) + f(b);

    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        if (i % 2 == 0)
        {
            s += 2.0 * f(x);
        }
        else
        {
            s += 4.0 * f(x);
        }
    }
    return (h / 3.0) * s;
}

int main()
{
    const double a = 0.0;
    const double b = M_PI;

    // Valor de referencia analítico: Integral de sin(x) de 0 a pi es [-cos(pi)] - [-cos(0)] = 1 - (-1) = 2.0
    const double I_ref = F(b) - F(a);

    std::cout << "=== METODOS DE INTEGRACION NUMERICA: f(x) = sin(x) ===" << std::endl;
    std::cout << "Intervalo: [" << a << ", pi]" << std::endl;
    std::cout << "Valor de referencia analitico: " << I_ref << "\n\n";

    // Valores de subintervalos a evaluar
    const int n_vals[] = {100, 1000, 10000, 100000, 1000000};
    const int num_pruebas = 5;
    const int R = 10000; // Repeticiones para medición de tiempo robusta

    std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
    std::cout << "    n  |       Metodo       |      Resultado     |    Error Relativo  |   Tiempo (s)     " << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------" << std::endl;

    for (int k = 0; k < num_pruebas; k++)
    {
        int n = n_vals[k];

        // --- TRAPECIO ---
        auto t0_trap = std::chrono::steady_clock::now();
        double I_trap = 0.0;
        for (int r = 0; r < R; r++)
        {
            I_trap = trapecio_compuesto(a, b, n);
        }
        auto t1_trap = std::chrono::steady_clock::now();
        double t_trap = std::chrono::duration<double>(t1_trap - t0_trap).count() / R;
        double err_trap = std::fabs(I_trap - I_ref);

        printf("%6d | Trapecio         | %18.15f | %14.4e | %14.4e\n", n, I_trap, err_trap, t_trap);

        // --- SIMPSON 1/3 ---
        auto t0_simp = std::chrono::steady_clock::now();
        double I_simp = 0.0;
        for (int r = 0; r < R; r++)
        {
            I_simp = simpson_compuesto(a, b, n);
        }
        auto t1_simp = std::chrono::steady_clock::now();
        double t_simp = std::chrono::duration<double>(t1_simp - t0_simp).count() / R;
        double err_simp = std::fabs(I_simp - I_ref);

        printf("%6d | Simpson 1/3      | %18.15f | %14.4e | %14.4e\n", n, I_simp, err_simp, t_simp);
        std::cout << "-----------------------------------------------------------------------------------------" << std::endl;
    }

    return 0;
}