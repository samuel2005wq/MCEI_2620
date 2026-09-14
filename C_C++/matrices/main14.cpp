// ============================================================
//  punto4_inversa_qr_svd.cpp
//
//  PARTE 4: QR y SVD para inversa de matrices
//     - Inversa por método directo (Gauss-Jordan)
//     - Inversa vía factorización QR (Householder)
//     - Inversa vía SVD (algoritmo de Jacobi de un lado, "one-sided
//       Jacobi"), implementada desde cero sin librerías externas
//     - Comparación de resultados y estabilidad numérica
//
//  Compilar:  g++ -O2 -std=c++17 punto4_inversa_qr_svd.cpp -o punto4
//  Ejecutar:  ./punto4
// ============================================================
#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <stdexcept>
#include <algorithm>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

// ------------------------------------------------------------
// Utilidades básicas
// ------------------------------------------------------------
Matrix identity(int n) {
    Matrix I(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i) I[i][i] = 1.0;
    return I;
}

Matrix transpose(const Matrix& A) {
    int m = (int)A.size(), n = (int)A[0].size();
    Matrix T(n, Vector(m));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            T[j][i] = A[i][j];
    return T;
}

Matrix matMul(const Matrix& A, const Matrix& B) {
    int m = (int)A.size(), k = (int)A[0].size(), n = (int)B[0].size();
    Matrix C(m, Vector(n, 0.0));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j) {
            double s = 0.0;
            for (int t = 0; t < k; ++t) s += A[i][t] * B[t][j];
            C[i][j] = s;
        }
    return C;
}

Matrix matSub(const Matrix& A, const Matrix& B) {
    int m = (int)A.size(), n = (int)A[0].size();
    Matrix C(m, Vector(n));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            C[i][j] = A[i][j] - B[i][j];
    return C;
}

double frobeniusNorm(const Matrix& A) {
    double s = 0.0;
    for (const auto& row : A)
        for (double v : row) s += v * v;
    return std::sqrt(s);
}

void printMatrixCompact(const Matrix& A, const std::string& name, int maxRows = 3) {
    std::cout << "--- " << name << " (primeras " << maxRows << " filas) ---\n";
    std::cout << std::fixed << std::setprecision(4);
    for (int i = 0; i < std::min((int)A.size(), maxRows); ++i) {
        for (double v : A[i]) std::cout << std::setw(9) << v << " ";
        std::cout << "\n";
    }
    std::cout << "...\n\n";
}

// ------------------------------------------------------------
// 1) INVERSA POR MÉTODO DIRECTO (Gauss-Jordan con pivoteo parcial)
// ------------------------------------------------------------
Matrix gaussJordanInverse(Matrix A) {
    int n = (int)A.size();
    Matrix I = identity(n);

    for (int k = 0; k < n; ++k) {
        int p = k;
        double best = std::fabs(A[k][k]);
        for (int i = k + 1; i < n; ++i)
            if (std::fabs(A[i][k]) > best) { best = std::fabs(A[i][k]); p = i; }
        if (best < 1e-14) throw std::runtime_error("Matriz singular (no invertible)");
        if (p != k) { std::swap(A[p], A[k]); std::swap(I[p], I[k]); }

        double piv = A[k][k];
        for (int j = 0; j < n; ++j) { A[k][j] /= piv; I[k][j] /= piv; }

        for (int i = 0; i < n; ++i) {
            if (i == k) continue;
            double f = A[i][k];
            for (int j = 0; j < n; ++j) {
                A[i][j] -= f * A[k][j];
                I[i][j] -= f * I[k][j];
            }
        }
    }
    return I;
}

// ------------------------------------------------------------
// 2) FACTORIZACIÓN QR (Householder) E INVERSA VÍA QR
// ------------------------------------------------------------
void qrHouseholder(Matrix A, Matrix& Q, Matrix& R) {
    int n = (int)A.size();
    R = A;
    Q = identity(n);

    for (int k = 0; k < n; ++k) {
        int m = n - k;
        Vector x(m);
        for (int i = 0; i < m; ++i) x[i] = R[k + i][k];

        double normx = 0.0;
        for (double xi : x) normx += xi * xi;
        normx = std::sqrt(normx);
        if (normx < 1e-14) continue;

        double s = (x[0] < 0) ? -1.0 : 1.0;
        double alpha = -s * normx;

        Vector v = x;
        v[0] -= alpha;
        double normv = 0.0;
        for (double vi : v) normv += vi * vi;
        normv = std::sqrt(normv);
        if (normv < 1e-14) continue;
        for (double& vi : v) vi /= normv;

        for (int j = 0; j < n; ++j) {
            double dot = 0.0;
            for (int i = 0; i < m; ++i) dot += v[i] * R[k + i][j];
            for (int i = 0; i < m; ++i) R[k + i][j] -= 2.0 * v[i] * dot;
        }
        for (int i = 0; i < n; ++i) {
            double dot = 0.0;
            for (int j = 0; j < m; ++j) dot += Q[i][k + j] * v[j];
            for (int j = 0; j < m; ++j) Q[i][k + j] -= 2.0 * dot * v[j];
        }
    }
}

// Resuelve R*x = y por sustitución hacia atrás (R triangular superior)
Vector backSub(const Matrix& R, const Vector& y) {
    int n = (int)y.size();
    Vector x(n, 0.0);
    for (int i = n - 1; i >= 0; --i) {
        double s = y[i];
        for (int j = i + 1; j < n; ++j) s -= R[i][j] * x[j];
        x[i] = s / R[i][i];
    }
    return x;
}

// Inversa vía QR: A^-1 = R^-1 * Q^T, resolviendo columna por columna
Matrix inverseViaQR(const Matrix& A) {
    int n = (int)A.size();
    Matrix Q, R;
    qrHouseholder(A, Q, R);
    Matrix Qt = transpose(Q);
    Matrix Ainv(n, Vector(n, 0.0));
    for (int col = 0; col < n; ++col) {
        Vector y(n);
        for (int i = 0; i < n; ++i) y[i] = Qt[i][col];
        Vector x = backSub(R, y);
        for (int i = 0; i < n; ++i) Ainv[i][col] = x[i];
    }
    return Ainv;
}

// ------------------------------------------------------------
// 3) SVD (Jacobi de un lado, "one-sided Jacobi") E INVERSA VÍA SVD
//    A = U * S * V^T   =>   A^-1 = V * S^-1 * U^T
// ------------------------------------------------------------
void svdOneSidedJacobi(Matrix A, Matrix& U, Vector& sigma, Matrix& V,
                        int maxSweeps = 60, double tol = 1e-13) {
    int n = (int)A.size();
    U = A;                 // se irá ortogonalizando por columnas
    V = identity(n);        // acumula las rotaciones

    for (int sweep = 0; sweep < maxSweeps; ++sweep) {
        double offDiag = 0.0;
        for (int p = 0; p < n - 1; ++p) {
            for (int q = p + 1; q < n; ++q) {
                double alpha = 0.0, beta = 0.0, gamma = 0.0;
                for (int i = 0; i < n; ++i) {
                    alpha += U[i][p] * U[i][p];
                    beta  += U[i][q] * U[i][q];
                    gamma += U[i][p] * U[i][q];
                }
                offDiag += gamma * gamma;
                if (std::fabs(gamma) < tol * std::sqrt(alpha * beta + 1e-300)) continue;

                double zeta = (beta - alpha) / (2.0 * gamma);
                double t = (zeta >= 0 ? 1.0 : -1.0) / (std::fabs(zeta) + std::sqrt(1.0 + zeta * zeta));
                double c = 1.0 / std::sqrt(1.0 + t * t);
                double s = c * t;

                for (int i = 0; i < n; ++i) {
                    double up = U[i][p], uq = U[i][q];
                    U[i][p] = c * up - s * uq;
                    U[i][q] = s * up + c * uq;

                    double vp = V[i][p], vq = V[i][q];
                    V[i][p] = c * vp - s * vq;
                    V[i][q] = s * vp + c * vq;
                }
            }
        }
        if (offDiag < tol * tol) break;   // convergió
    }

    // Valores singulares = norma de cada columna de U; luego se normalizan
    sigma.assign(n, 0.0);
    for (int j = 0; j < n; ++j) {
        double s = 0.0;
        for (int i = 0; i < n; ++i) s += U[i][j] * U[i][j];
        sigma[j] = std::sqrt(s);
        if (sigma[j] > 1e-300)
            for (int i = 0; i < n; ++i) U[i][j] /= sigma[j];
    }

    // Ordenar de mayor a menor sigma (y reordenar columnas de U y V)
    std::vector<int> idx(n);
    for (int i = 0; i < n; ++i) idx[i] = i;
    std::sort(idx.begin(), idx.end(), [&](int a, int b) { return sigma[a] > sigma[b]; });

    Matrix U2(n, Vector(n)), V2(n, Vector(n));
    Vector sigma2(n);
    for (int j = 0; j < n; ++j) {
        sigma2[j] = sigma[idx[j]];
        for (int i = 0; i < n; ++i) {
            U2[i][j] = U[i][idx[j]];
            V2[i][j] = V[i][idx[j]];
        }
    }
    U = U2; V = V2; sigma = sigma2;
}

Matrix inverseViaSVD(const Matrix& A) {
    int n = (int)A.size();
    Matrix U, V;
    Vector sigma;
    svdOneSidedJacobi(A, U, sigma, V);

    // Sinv = diag(1/sigma)
    Matrix Sinv(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i) Sinv[i][i] = 1.0 / sigma[i];

    // A^-1 = V * Sinv * U^T
    Matrix Ut = transpose(U);
    return matMul(matMul(V, Sinv), Ut);
}

// ------------------------------------------------------------
// Programa principal
// ------------------------------------------------------------
int main() {
    Matrix A = {
        {2,1,0,3,2,1,0,2,1,4},
        {1,3,2,0,1,4,2,1,0,2},
        {0,2,4,1,3,0,1,2,4,1},
        {3,0,1,5,2,1,3,0,2,1},
        {2,1,3,2,6,2,1,4,0,3},
        {1,4,0,1,2,5,2,1,3,0},
        {0,2,1,3,1,2,4,0,2,1},
        {2,1,2,0,4,1,0,5,3,2},
        {1,0,4,2,0,3,2,3,6,1},
        {4,2,1,1,3,0,1,2,1,5}
    };
    int n = (int)A.size();

    std::cout << "=====================================================\n";
    std::cout << " PARTE 4: QR y SVD para inversa de matrices\n";
    std::cout << "=====================================================\n\n";

    // ---- 1) Directo ----
    auto t0 = std::chrono::high_resolution_clock::now();
    Matrix Ainv_direct = gaussJordanInverse(A);
    auto t1 = std::chrono::high_resolution_clock::now();
    double t_direct = std::chrono::duration<double>(t1 - t0).count();
    printMatrixCompact(Ainv_direct, "Inversa por metodo directo (Gauss-Jordan)");

    // ---- 2) QR ----
    t0 = std::chrono::high_resolution_clock::now();
    Matrix Ainv_qr = inverseViaQR(A);
    t1 = std::chrono::high_resolution_clock::now();
    double t_qr = std::chrono::duration<double>(t1 - t0).count();
    printMatrixCompact(Ainv_qr, "Inversa via factorizacion QR");

    // ---- 3) SVD ----
    t0 = std::chrono::high_resolution_clock::now();
    Matrix Ainv_svd = inverseViaSVD(A);
    t1 = std::chrono::high_resolution_clock::now();
    double t_svd = std::chrono::duration<double>(t1 - t0).count();
    printMatrixCompact(Ainv_svd, "Inversa via SVD (Jacobi de un lado)");

    // ---- Valores singulares y condicion ----
    Matrix U, V; Vector sigma;
    svdOneSidedJacobi(A, U, sigma, V);
    std::cout << "Valores singulares de A: ";
    for (double s : sigma) std::cout << std::fixed << std::setprecision(4) << s << "  ";
    std::cout << "\ncond(A) = sigma_max/sigma_min = "
               << std::scientific << std::setprecision(6) << sigma.front() / sigma.back() << "\n\n";

    // ---- Comparacion ----
    Matrix I = identity(n);
    double res_direct = frobeniusNorm(matSub(matMul(A, Ainv_direct), I));
    double res_qr      = frobeniusNorm(matSub(matMul(A, Ainv_qr), I));
    double res_svd      = frobeniusNorm(matSub(matMul(A, Ainv_svd), I));

    double diff_qr  = frobeniusNorm(matSub(Ainv_qr, Ainv_direct));
    double diff_svd = frobeniusNorm(matSub(Ainv_svd, Ainv_direct));

    std::cout << std::scientific << std::setprecision(3);
    std::cout << "===================================================================\n";
    std::cout << " Metodo    | Tiempo (s)   | ||A*Ainv - I||  | ||Ainv - Ainv_directo||\n";
    std::cout << "-------------------------------------------------------------------\n";
    std::cout << " Directo   | " << std::fixed << std::setprecision(6) << t_direct
               << "   | " << std::scientific << res_direct << "  |     ---\n";
    std::cout << " QR        | " << std::fixed << std::setprecision(6) << t_qr
               << "   | " << std::scientific << res_qr << "  |  " << diff_qr << "\n";
    std::cout << " SVD       | " << std::fixed << std::setprecision(6) << t_svd
               << "   | " << std::scientific << res_svd << "  |  " << diff_svd << "\n";
    std::cout << "===================================================================\n\n";

    std::cout <<
        "Analisis:\n"
        " - Los tres metodos dan practicamente la misma inversa para esta\n"
        "   matriz A (bien condicionada).\n"
        " - Estabilidad, de mas a menos robusto ante mal condicionamiento:\n"
        "     1) SVD:    la mas estable; expone los valores singulares y\n"
        "        detecta cercania a la singularidad (sigma_min chico).\n"
        "     2) QR:     estable por usar reflexiones ortogonales.\n"
        "     3) Directo (Gauss-Jordan): mas rapido y barato, pero el que\n"
        "        mas amplifica errores si A esta mal condicionada.\n"
        " - Costo computacional: Directo ~ O(n^3), QR ~ 2x ese costo, y\n"
        "   la SVD iterativa (Jacobi) es la mas cara de las tres, por eso\n"
        "   en la practica solo se usa cuando se sospecha mal\n"
        "   condicionamiento o casi singularidad.\n";

    return 0;
}
