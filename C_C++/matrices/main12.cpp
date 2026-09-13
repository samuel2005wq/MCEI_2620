// ============================================================
//  sistema_10x10
// Codigo diseñado por: Carolina García García y Samuel Acuña Garzón
//  Solución de A*x = b (10x10) por tres métodos:
//     1) Eliminación de Gauss con pivoteo parcial
//     2) Factorización LU (PA = LU) con pivoteo parcial
//     3) Factorización QR (Householder)
//  y comparación de estabilidad (residuo/error) y eficiencia (tiempo)
//
//  Compilar:  g++ -O2 -std=c++17 sistema_10x10.cpp -o sistema_10x10
//  Ejecutar:  ./sistema_10x10
// ============================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <stdexcept>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

// ------------------------------------------------------------
// Utilidades básicas
// ------------------------------------------------------------
void printVector(const Vector &v, const std::string &name)
{
    // Imprime un título y cada componente del vector con seis decimales.
    std::cout << "--- " << name << " ---\n";
    for (size_t i = 0; i < v.size(); ++i)
        // i + 1 se usa para mostrar la numeración matemática x1, x2, ..., xn.
        std::cout << "x" << std::setw(2) << i + 1 << " = "
                  << std::setw(12) << std::fixed << std::setprecision(6) << v[i] << "\n";
    // Línea en blanco para separar este resultado del siguiente.
    std::cout << "\n";
}

Vector matVec(const Matrix &A, const Vector &x)
{
    // Multiplica una matriz cuadrada A por un vector x.
    size_t n = A.size();
    Vector r(n, 0.0);
    for (size_t i = 0; i < n; ++i)
        // Cada componente r[i] es el producto punto de la fila i por x.
        for (size_t j = 0; j < n; ++j)
            r[i] += A[i][j] * x[j];
    return r;
}

double normDiff(const Vector &a, const Vector &b)
{
    // Calcula la norma euclidiana de la diferencia entre dos vectores.
    double s = 0.0;
    // Se acumula la suma de los cuadrados de las diferencias componente a componente.
    for (size_t i = 0; i < a.size(); ++i)
        s += (a[i] - b[i]) * (a[i] - b[i]);
    return std::sqrt(s);
}

double vecNorm(const Vector &v)
{
    // Calcula la norma 2 del vector: sqrt(v_1^2 + ... + v_n^2).
    double s = 0.0;
    for (double vi : v)
        s += vi * vi;
    return std::sqrt(s);
}

// ------------------------------------------------------------
// 1) ELIMINACIÓN DE GAUSS CON PIVOTEO PARCIAL
// ------------------------------------------------------------
Vector gaussElimination(Matrix A, Vector b)
{
    // Se reciben copias porque la eliminación modifica A y b durante el proceso.
    int n = (int)b.size();
    for (int k = 0; k < n - 1; ++k)
    {
        // Se busca el mayor elemento de la columna k desde la fila actual hacia abajo.
        int p = k;
        double best = std::fabs(A[k][k]);
        for (int i = k + 1; i < n; ++i)
            if (std::fabs(A[i][k]) > best)
            {
                best = std::fabs(A[i][k]);
                p = i;
            }

        // Un pivote demasiado pequeño indica que la matriz es singular o casi singular.
        if (best < 1e-14)
            throw std::runtime_error("Matriz singular (Gauss)");

        // Se intercambia la fila actual con la fila del mejor pivote.
        if (p != k)
        {
            std::swap(A[p], A[k]);
            std::swap(b[p], b[k]);
        }
        // Se eliminan los elementos debajo del pivote para formar una matriz triangular superior.
        for (int i = k + 1; i < n; ++i)
        {
            // Factor utilizado para hacer cero el elemento A[i][k].
            double m = A[i][k] / A[k][k];
            for (int j = k; j < n; ++j)
                A[i][j] -= m * A[k][j];
            b[i] -= m * b[k];
        }
    }

    // Sustitución hacia atrás para resolver la matriz triangular superior.
    Vector x(n, 0.0);
    x[n - 1] = b[n - 1] / A[n - 1][n - 1];
    for (int i = n - 2; i >= 0; --i)
    {
        // Se resta la contribución de las incógnitas ya calculadas.
        double s = b[i];
        for (int j = i + 1; j < n; ++j)
            s -= A[i][j] * x[j];
        x[i] = s / A[i][i];
    }
    return x;
}

// ------------------------------------------------------------
// 2) FACTORIZACIÓN LU (PA = LU) CON PIVOTEO PARCIAL
// ------------------------------------------------------------
void luDecompose(Matrix A, Matrix &L, Matrix &U, std::vector<int> &perm)
{
    // Descompone la matriz en PA = LU, donde P está representada por perm.
    int n = (int)A.size();
    // L comienza como matriz cero y U como una copia de A.
    L.assign(n, Vector(n, 0.0));
    U = A;
    perm.resize(n);
    // La diagonal de L es 1 y perm comienza representando la identidad.
    for (int i = 0; i < n; ++i)
    {
        perm[i] = i;
        L[i][i] = 1.0;
    }

    for (int k = 0; k < n - 1; ++k)
    {
        // Selección del pivote parcial en la columna k de U.
        int p = k;
        double best = std::fabs(U[k][k]);
        for (int i = k + 1; i < n; ++i)
            if (std::fabs(U[i][k]) > best)
            {
                best = std::fabs(U[i][k]);
                p = i;
            }

        if (p != k)
        {
            // Se intercambian filas de U y el registro de permutaciones.
            std::swap(U[p], U[k]);
            std::swap(perm[p], perm[k]);
            // Los multiplicadores calculados anteriormente también deben intercambiarse en L.
            for (int j = 0; j < k; ++j)
                std::swap(L[p][j], L[k][j]);
        }
        // Se calcula la columna k de L y se actualiza U eliminando debajo del pivote.
        for (int i = k + 1; i < n; ++i)
        {
            L[i][k] = U[i][k] / U[k][k];
            for (int j = k; j < n; ++j)
                U[i][j] -= L[i][k] * U[k][j];
        }
    }
}

Vector forwardSub(const Matrix &L, const Vector &b)
{
    // Resuelve L*y = b mediante sustitución hacia adelante.
    int n = (int)b.size();
    Vector y(n, 0.0);
    for (int i = 0; i < n; ++i)
    {
        // Las componentes y[0] ... y[i-1] ya fueron calculadas.
        double s = b[i];
        for (int j = 0; j < i; ++j)
            s -= L[i][j] * y[j];
        y[i] = s / L[i][i];
    }
    return y;
}

Vector backSub(const Matrix &U, const Vector &y)
{
    // Resuelve U*x = y mediante sustitución hacia atrás.
    int n = (int)y.size();
    Vector x(n, 0.0);
    for (int i = n - 1; i >= 0; --i)
    {
        // Las componentes x[i+1] ... x[n-1] ya fueron calculadas.
        double s = y[i];
        for (int j = i + 1; j < n; ++j)
            s -= U[i][j] * x[j];
        x[i] = s / U[i][i];
    }
    return x;
}

Vector solveLU(const Matrix &A, const Vector &b, Matrix &L, Matrix &U)
{
    // Resuelve PA = LU aplicando la permutación también al vector b.
    std::vector<int> perm;
    luDecompose(A, L, U, perm);
    int n = (int)b.size();
    Vector pb(n);
    // pb es el vector b reordenado según los intercambios de filas.
    for (int i = 0; i < n; ++i)
        pb[i] = b[perm[i]];
    // Primero se resuelve L*y = P*b y luego U*x = y.
    Vector y = forwardSub(L, pb);
    return backSub(U, y);
}

// ------------------------------------------------------------
// 3) FACTORIZACIÓN QR (Householder)
// ------------------------------------------------------------
void qrHouseholder(Matrix A, Matrix &Q, Matrix &R)
{
    // Construye A = Q*R mediante transformaciones ortogonales de Householder.
    int n = (int)A.size();
    // R comienza siendo A y Q comienza siendo la matriz identidad.
    R = A;
    Q.assign(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i)
        Q[i][i] = 1.0;

    for (int k = 0; k < n; ++k)
    {
        // En cada paso se anulan los elementos debajo de R[k][k].
        int m = n - k; // tamaño del subvector
        Vector x(m);
        // x contiene la parte activa de la columna k.
        for (int i = 0; i < m; ++i)
            x[i] = R[k + i][k];

        double normx = vecNorm(x);
        // Si el subvector ya es prácticamente cero, no hace falta transformar.
        if (normx < 1e-14)
            continue;

        // Se escoge el signo para evitar pérdida de precisión al construir el reflector.
        double s = (x[0] < 0) ? -1.0 : 1.0;
        double alpha = -s * normx;

        // El vector v define la reflexión de Householder.
        Vector v = x;
        v[0] -= alpha;
        double normv = vecNorm(v);
        if (normv < 1e-14)
            continue;
        // Se normaliza v para que la reflexión sea H = I - 2*v*v^T.
        for (double &vi : v)
            vi /= normv;

        // R(k:n,:) = R(k:n,:) - 2*v*(v' * R(k:n,:))
        for (int j = 0; j < n; ++j)
        {
            double dot = 0.0;
            for (int i = 0; i < m; ++i)
                dot += v[i] * R[k + i][j];
            for (int i = 0; i < m; ++i)
                R[k + i][j] -= 2.0 * v[i] * dot;
        }
        // Q(:,k:n) = Q(:,k:n) - 2*(Q(:,k:n)*v)*v'
        for (int i = 0; i < n; ++i)
        {
            double dot = 0.0;
            for (int j = 0; j < m; ++j)
                dot += Q[i][k + j] * v[j];
            for (int j = 0; j < m; ++j)
                Q[i][k + j] -= 2.0 * dot * v[j];
        }
    }
}

Vector solveQR(const Matrix &A, const Vector &b, Matrix &Q, Matrix &R)
{
    // Factoriza A y resuelve Q*R*x = b.
    qrHouseholder(A, Q, R);
    int n = (int)b.size();
    // y = Q^T * b
    Vector y(n, 0.0);
    // Se calcula y y después se resuelve el sistema triangular R*x = y.
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            y[i] += Q[j][i] * b[j]; // Q^T
    return backSub(R, y);
}

// ------------------------------------------------------------
// Programa principal
// ------------------------------------------------------------
int main()
{
    // Matriz cuadrada A de 10x10 del sistema lineal.
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

    // <<<<<< Vector b de ejemplo (cámbialo si tu ejercicio trae uno) >>>>>>
    // Vector de términos independientes del sistema A*x = b.
    Vector b = {12, 18, 15, 20, 25, 14, 16, 22, 19, 17};

    std::cout << "=====================================================\n";
    std::cout << " Sistema 10x10 -- Comparación Gauss / LU / QR\n";
    std::cout << "=====================================================\n\n";

    // ---- 1) Gauss ----
    // Se mide únicamente el tiempo de cálculo de la solución por Gauss.
    auto t0 = std::chrono::high_resolution_clock::now();
    Vector x_gauss = gaussElimination(A, b);
    auto t1 = std::chrono::high_resolution_clock::now();
    double t_gauss = std::chrono::duration<double>(t1 - t0).count();
    printVector(x_gauss, "Solución por Eliminación de Gauss");

    // ---- 2) LU ----
    // L y U recibirán los factores de la descomposición PA = LU.
    Matrix L, U;
    t0 = std::chrono::high_resolution_clock::now();
    Vector x_lu = solveLU(A, b, L, U);
    t1 = std::chrono::high_resolution_clock::now();
    double t_lu = std::chrono::duration<double>(t1 - t0).count();
    printVector(x_lu, "Solución por Factorización LU");

    // ---- 3) QR ----
    // Q y R recibirán los factores de la descomposición A = QR.
    Matrix Q, R;
    t0 = std::chrono::high_resolution_clock::now();
    Vector x_qr = solveQR(A, b, Q, R);
    t1 = std::chrono::high_resolution_clock::now();
    double t_qr = std::chrono::duration<double>(t1 - t0).count();
    printVector(x_qr, "Solución por Factorización QR (Householder)");

    // ---- Comparación ----
    // Se calcula A*x para cada solución y se compara con b.
    Vector r_gauss_vec = matVec(A, x_gauss);
    Vector r_lu_vec = matVec(A, x_lu);
    Vector r_qr_vec = matVec(A, x_qr);

    double res_gauss = normDiff(r_gauss_vec, b);
    double res_lu = normDiff(r_lu_vec, b);
    double res_qr = normDiff(r_qr_vec, b);

    // El residuo ||A*x - b|| mide qué tan bien satisface cada solución el sistema original.
    std::cout << "=====================================================\n";
    std::cout << " Método   | Tiempo (s)   | ||A*x - b||\n";
    std::cout << "-----------------------------------------------------\n";
    std::cout << std::scientific << std::setprecision(3);
    std::cout << " Gauss    | " << std::fixed << std::setprecision(8) << t_gauss
              << "  | " << std::scientific << res_gauss << "\n";
    std::cout << " LU       | " << std::fixed << std::setprecision(8) << t_lu
              << "  | " << std::scientific << res_lu << "\n";
    std::cout << " QR       | " << std::fixed << std::setprecision(8) << t_qr
              << "  | " << std::scientific << res_qr << "\n";
    std::cout << "=====================================================\n\n";

    std::cout << "Comentario:\n"
                 " - Gauss y LU hacen la misma eliminacion (~2n^3/3 flops); LU\n"
                 "   conviene cuando se resuelven varios b con la misma A, ya\n"
                 "   que la factorizacion se reutiliza.\n"
                 " - QR (Householder) cuesta aprox el doble (~4n^3/3 flops) pero\n"
                 "   es mas estable numericamente al usar transformaciones\n"
                 "   ortogonales, por lo que amplifica menos los errores de\n"
                 "   redondeo; es preferible en sistemas mal condicionados o en\n"
                 "   minimos cuadrados.\n"
                 " - Para esta matriz A (bien condicionada) los tres metodos dan\n"
                 "   practicamente el mismo resultado y residuos del orden de\n"
                 "   la precision de la maquina.\n";

    return 0;
}
