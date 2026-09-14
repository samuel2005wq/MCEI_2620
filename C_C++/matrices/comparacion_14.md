# Análisis del punto 1.4 del taller

## Desempeño y Precisión (C++ vs. Octave)

| Método | Plataforma | Tiempo de Ejecución (s) | Error de Identidad ($\|A A^{-1} - I\|$) | Diferencia vs. Directo |
| :--- | :--- | :--- | :--- | :--- |
| **Directo (Gauss-Jordan/LU)** | C++ | 0.000029 | $3.684 \times 10^{-15}$ | --- |
| | Octave | 0.000678 | $3.371 \times 10^{-15}$ | --- |
| **Factorización QR (Householder)** | C++ | 0.000041 | $4.849 \times 10^{-15}$ | $2.603 \times 10^{-15}$ |
| | Octave | 0.009636 | $4.671 \times 10^{-15}$ | $1.801 \times 10^{-15}$ |
| **SVD (Jacobi / LAPACK)** | C++ | 0.000688 | $6.260 \times 10^{-14}$ | $5.626 \times 10^{-14}$ |
| | Octave | 0.000482 | $6.967 \times 10^{-15}$ | $4.067 \times 10^{-15}$ |

## Análisis de Estabilidad y Costo Computacional

*   **Precisión de Máquina Alcanzada:** Todos los métodos logran un error residual en el orden de $10^{-14}$ a $10^{-15}$. Esto confirma que la matriz de prueba está muy bien condicionada ($\text{cond}(A) \approx 51.71$) y valida la exactitud matemática de tu implementación manual del algoritmo de Jacobi en C++.
*   **Complejidad vs. Estabilidad:** 
    *   **Directo (Gauss-Jordan):** Es el más rápido (como se evidencia en los $\sim 29 \mu s$ de C++), pero es el más vulnerable a la inestabilidad si la matriz estuviera mal condicionada.
    *   **QR (Householder):** Las reflexiones ortogonales evitan la amplificación de errores de redondeo con una penalidad de tiempo marginal, ofreciendo un balance excelente.
    *   **SVD:** Es computacionalmente el más exigente en tu rutina de C++ debido a los múltiples barridos iterativos del algoritmo de Jacobi. Sin embargo, proporciona máxima robustez matemática al exponer los valores singulares explícitamente ($\sigma_{min} = 0.3736$).
*   **Diferencias de Entorno de Ejecución:** El operador SVD nativo en Octave resulta más rápido que el bucle `qr_householder` interpretado del mismo script, ya que la función `svd()` de Octave delega los cálculos a librerías de álgebra lineal precompiladas y fuertemente optimizadas (LAPACK/BLAS). Por el contrario, en C++, el método directo compilado supera en velocidad a las factorizaciones más robustas, respetando la complejidad teórica de $\mathcal{O}(n^3)$.
