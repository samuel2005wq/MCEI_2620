# Análisis Comparativo del Sistema 10x10

En primer lugar, los vectores de solución $x$ obtenidos por ambos lenguajes y por los tres métodos (Gauss, LU, QR) son **idénticos** hasta la sexta cifra decimal impresa. 

Al analizar los residuos (la norma de $||Ax - b||$), observamos que los errores absolutos se encuentran en el orden de $10^{-14}$. Considerando que la precisión de máquina (épsilon) para coma flotante de doble precisión (`double`) es aproximadamente $2.22 \times 10^{-16}$, estos residuos indican que **los algoritmos están extrayendo la máxima precisión posible del hardware**.

La razón principal del excelente comportamiento de los tres métodos es el **número de condición de la matriz**. Como bien se calculó en el script de Octave, $\kappa(A) \approx 51.71$. Al ser una matriz tan bien condicionada (lejos de ser singular o mal condicionada), la eliminación de Gauss y la factorización LU (con pivoteo parcial) no sufren de cancelación catastrófica, igualando en la práctica la estabilidad numérica superior que naturalmente ofrece la factorización Householder (QR).

| Entorno / Método | Complejidad Teórica | Tiempo de Ejecución (s) | Residuo $||Ax - b||$ |
| :--- | :---: | :---: | :---: |
| **Octave (Interpretado)** | | | |
| Gauss | $\approx \frac{2}{3}n^3$ flops | $0.003133$ | $1.137 \times 10^{-14}$ |
| LU | $\approx \frac{2}{3}n^3$ flops | $0.001374$ | $1.081 \times 10^{-14}$ |
| QR | $\approx \frac{4}{3}n^3$ flops | $0.001258$ | $3.217 \times 10^{-14}$ |
| **C++ (Compilado `-O2`)**| | | |
| Gauss | $\approx \frac{2}{3}n^3$ flops | $0.000012$ | $1.353 \times 10^{-14}$ |
| LU | $\approx \frac{2}{3}n^3$ flops | $0.000016$ | $1.353 \times 10^{-14}$ |
| QR | $\approx \frac{4}{3}n^3$ flops | $0.000036$ | $2.525 \times 10^{-14}$ |

Aquí es donde radica el hallazgo empírico más valioso de esta comparativa: **la diferencia entre la teoría asintótica y el *overhead* o sobrecarga de ejecución del entorno**.

*   **El comportamiento en C++ (El reflejo de la teoría):** Al compilar el código con las optimizaciones del compilador Gnu (`g++ -O2`), estamos midiendo el verdadero costo computacional de las operaciones en la CPU. Aquí, la teoría se cumple a la perfección: Gauss ($\sim 12 \mu s$) y LU ($\sim 16 \mu s$) toman prácticamente el mismo tiempo, ya que ambos requieren $\frac{2}{3}n^3$ operaciones de punto flotante. Por su parte, la factorización QR ($\sim 36 \mu s$) toma un poco más del doble de tiempo que LU, lo cual encaja exactamente con su costo teórico de $\frac{4}{3}n^3$ flops.
*   **El comportamiento en Octave (El peso del intérprete):** Los tiempos en Octave son entre 80 y 260 veces más lentos que en la versión compilada en C++. Además, en Octave el método QR parece ejecutarse más rápido o al mismo tiempo que LU/Gauss. Esto no se debe a que QR requiera menos cálculos matemáticos, sino a cómo el motor interno de Octave maneja las asignaciones de memoria y la sobrecarga de los ciclos `for` anidados. En lenguajes interpretados, el número de iteraciones y las indexaciones a menudo impactan más el tiempo de ejecución que las sumas y multiplicaciones reales.

El diseño de ambos códigos es metodológicamente correcto y altamente robusto. Como recomendaciones de diseño para futuras implementaciones (por ejemplo, al integrar estas rutinas en sistemas de control, estimación de estados, elementos finitos o robótica):

1.  **Matrices constantes, vectores dinámicos:** Si en una simulación $A$ representa el modelo físico invariante de un sistema y $b$ representa las condiciones de contorno o lecturas de sensores que cambian en cada paso de tiempo, **LU en C++ es la herramienta ideal**. Se factoriza $A$ una sola vez en la fase de inicialización (`setup`) y se resuelve $b$ de forma ultrarrápida con sustitución hacia adelante y atrás ($O(n^2)$) en el bucle principal.
2.  **Sistemas críticos o mal condicionados:** Si el modelo empieza a involucrar valores que varían drásticamente en magnitud (haciendo que el número de condición $\kappa(A)$ se dispare a valores de $10^6$ o superiores), **QR en C++ es indispensable**. Aunque su costo computacional sea el doble, la ortogonalidad de las transformaciones de Householder evitará que la acumulación de errores de redondeo destruya la integridad de la solución, garantizando estabilidad donde Gauss o LU fallarían estrepitosamente.
