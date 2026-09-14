// ============================================================
//  punto3_condicionamiento.cpp
//
//  PARTE 3: Condicionamiento numérico y estabilidad
//     - Número de condición de A (norma infinito, vía inversa
//       calculada con Gauss-Jordan)
//     - Solución con A original vs. A perturbada
//     - Análisis del error y discusión de estabilidad
//
//  Nota: aquí se usa la norma INFINITO (max suma de fila) para el
//  número de condición, porque calcular la norma-2 exacta requiere
//  los valores singulares (SVD), que se implementa aparte en el
//  punto 4. cond_inf(A) = ||A||_inf * ||A^-1||_inf es una alternativa
//  estándar, más barata de calcular, y muestra el mismo fenómeno
//  cualitativo de amplificación del error.
//
//  Compilar:  g++ -O2 -std=c++17 punto3_condicionamiento.cpp -o punto3
//  Ejecutar:  ./punto3
// ============================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <stdexcept>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

// ------------------------------------------------------------
// Utilidades
// ------------------------------------------------------------
void printVector(const Vector &v, const std::string &name)
{
    std::cout << "--- " << name << " ---\n";
    for (size_t i = 0; i < v.size(); ++i)
        std::cout << "x" << std::setw(2) << i + 1 << " = "
                  << std::setw(12) << std::fixed << std::setprecision(6) << v[i] << "\n";
    std::cout << "\n";
}

double matrixNormInf(const Matrix &A)
{
    double best = 0.0;
    for (const auto &row : A)
    {
        double s = 0.0;
        for (double v : row)
            s += std::fabs(v);
        best = std::max(best, s);
    }
    return best;
}

double vectorNormInf(const Vector &v)
{
    double best = 0.0;
    for (double vi : v)
        best = std::max(best, std::fabs(vi));
    return best;
}

Matrix matAdd(const Matrix &A, const Matrix &B)
{
    int n = (int)A.size();
    Matrix C(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            C[i][j] = A[i][j] + B[i][j];
    return C;
}

Vector vecSub(const Vector &a, const Vector &b)
{
    Vector r(a.size());
    for (size_t i = 0; i < a.size(); ++i)
        r[i] = a[i] - b[i];
    return r;
}

// ------------------------------------------------------------
// Eliminación de Gauss con pivoteo parcial (resuelve A*x=b)
// ------------------------------------------------------------
Vector gaussSolve(Matrix A, Vector b)
{
    int n = (int)b.size();
    for (int k = 0; k < n - 1; ++k)
    {
        int p = k;
        double best = std::fabs(A[k][k]);
        for (int i = k + 1; i < n; ++i)
            if (std::fabs(A[i][k]) > best)
            {
                best = std::fabs(A[i][k]);
                p = i;
            }
        if (best < 1e-14)
            throw std::runtime_error("Matriz singular");
        if (p != k)
        {
            std::swap(A[p], A[k]);
            std::swap(b[p], b[k]);
        }
        for (int i = k + 1; i < n; ++i)
        {
            double m = A[i][k] / A[k][k];
            for (int j = k; j < n; ++j)
                A[i][j] -= m * A[k][j];
            b[i] -= m * b[k];
        }
    }
    Vector x(n, 0.0);
    x[n - 1] = b[n - 1] / A[n - 1][n - 1];
    for (int i = n - 2; i >= 0; --i)
    {
        double s = b[i];
        for (int j = i + 1; j < n; ++j)
            s -= A[i][j] * x[j];
        x[i] = s / A[i][i];
    }
    return x;
}

// ------------------------------------------------------------
// Inversa por Gauss-Jordan con pivoteo parcial
// ------------------------------------------------------------
Matrix gaussJordanInverse(Matrix A)
{
    int n = (int)A.size();
    Matrix I(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i)
        I[i][i] = 1.0;

    for (int k = 0; k < n; ++k)
    {
        int p = k;
        double best = std::fabs(A[k][k]);
        for (int i = k + 1; i < n; ++i)
            if (std::fabs(A[i][k]) > best)
            {
                best = std::fabs(A[i][k]);
                p = i;
            }
        if (best < 1e-14)
            throw std::runtime_error("Matriz singular (no invertible)");
        if (p != k)
        {
            std::swap(A[p], A[k]);
            std::swap(I[p], I[k]);
        }

        double piv = A[k][k];
        for (int j = 0; j < n; ++j)
        {
            A[k][j] /= piv;
            I[k][j] /= piv;
        }

        for (int i = 0; i < n; ++i)
        {
            if (i == k)
                continue;
            double f = A[i][k];
            for (int j = 0; j < n; ++j)
            {
                A[i][j] -= f * A[k][j];
                I[i][j] -= f * I[k][j];
            }
        }
    }
    return I;
}

// ------------------------------------------------------------
// Programa principal
// ------------------------------------------------------------
int main()
{
    Matrix A = {
        {2, 1, 0, 3, 2, 1, 0, 2, 1, 4},
        {1, 3, 2, 0, 1, 4, 2, 1, 0, 2},
        {0, 2, 4, 1, 3, 0, 1, 2, 4, 1},
        {3, 0, 1, 5, 2, 1, 3, 0, 2, 1},
        {2, 1, 3, 2, 6, 2, 1, 4, 0, 3},
        {1, 4, 0, 1, 2, 5, 2, 1, 3, 0},
        {0, 2, 1, 3, 1, 2, 4, 0, 2, 1},
        {2, 1, 2, 0, 4, 1, 0, 5, 3, 2},
        {1, 0, 4, 2, 0, 3, 2, 3, 6, 1},
        {4, 2, 1, 1, 3, 0, 1, 2, 1, 5}};
    Vector b = {12, 18, 15, 20, 25, 14, 16, 22, 19, 17};
    int n = (int)b.size();

    std::cout << "=====================================================\n";
    std::cout << " PARTE 3: Condicionamiento numerico y estabilidad\n";
    std::cout << "=====================================================\n\n";

    // ---- Número de condición (norma infinito) ----
    Matrix Ainv = gaussJordanInverse(A);
    double condInf = matrixNormInf(A) * matrixNormInf(Ainv);
    std::cout << "Numero de condicion de A (norma infinito): "
              << std::scientific << std::setprecision(6) << condInf << "\n\n";

    // ---- Perturbación pequeña y estructurada ----
    Matrix deltaA(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            deltaA[i][j] = 1e-4 * (((i + j) % 2 == 0) ? 1.0 : -1.0);

    Matrix A_pert = matAdd(A, deltaA);

    Vector x_orig = gaussSolve(A, b);
    Vector x_pert = gaussSolve(A_pert, b);

    printVector(x_orig, "Solucion con A original");
    printVector(x_pert, "Solucion con A perturbada (A + deltaA), deltaA ~ 1e-4");

    double err_A_rel = matrixNormInf(deltaA) / matrixNormInf(A);
    double err_x_rel = vectorNormInf(vecSub(x_pert, x_orig)) / vectorNormInf(x_orig);
    double amplif = err_x_rel / err_A_rel;

    std::cout << std::scientific << std::setprecision(6);
    std::cout << "||deltaA||_inf / ||A||_inf         = " << err_A_rel << "\n";
    std::cout << "||x_pert-x_orig||_inf/||x_orig||_inf = " << err_x_rel << "\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << "Factor de amplificacion observado  = " << amplif << "\n";
    std::cout << "cond_inf(A) (cota teorica superior) = " << condInf << "\n\n";

    std::cout << "Analisis:\n"
                 " - Teoria: ||dx||/||x|| <= cond(A) * ||dA||/||A||; cond(A) es\n"
                 "   una COTA del factor de amplificacion, no el valor exacto.\n"
                 " - El factor observado ("
              << amplif << ") deberia ser menor o\n"
                           "   igual a cond_inf(A) ("
              << condInf << ").\n"
                            " - Si A estuviera muy mal condicionada, la misma perturbacion\n"
                            "   de 1e-4 podria producir errores relativos mucho mayores en x.\n"
                            " - Gauss con pivoteo parcial es estable para matrices bien\n"
                            "   condicionadas como esta; el problema de precision nace de la\n"
                            "   sensibilidad del SISTEMA (cond(A)), no solo del metodo usado\n"
                            "   para resolverlo.\n";

    return 0;
}
