# Análisis Comparativo: Integración Directa vs. Interpolación + Integración en C/C++ (GSL)

## 1. Integración Directa de Datos (Trapecio Explícito)

* **Ventajas:**
  * **Velocidad de ejecución prácticamente instantánea:** Tomó tan solo **0.0002 ms** (200 nanosegundos). Al ser un bucle lineal de complejidad $\mathcal{O}(N)$ sin reserva de memoria ni operaciones matriciales, es óptimo para ejecución en sistemas de tiempo real.
  * **Excelente relación precisión-costo:** Logra un error relativo de **0.005530%** ($I = 21.19201822$) de manera directa sin requerir librerías externas ni código complejo.
  * **Simplicidad y ligereza:** No requiere asignación dinámica de memoria ni resolución de sistemas de ecuaciones.

* **Limitaciones:**
  * **Menor precisión analítica:** Al aproximar el área mediante rectas entre nodos ($h = 0.20$), su error es aproximadamente **9 veces mayor** que el obtenido con la interpolación cúbica.
  * **Rigidez en los puntos de evaluación:** Queda restringido a integrar estrictamente sobre las posiciones discretas en las que fue muestreada la señal.

---

## 2. Interpolación Previa + Integración (Spline Cúbico con GSL)

* **Ventajas:**
  * **Mayor exactitud:** Alcanza un valor de $I = 21.19188826$, reduciendo el error relativo a **0.000613%**. Al reconstruir polinómicamente curvas de tercer grado ($C^2$), se ajusta con alta fidelidad a la forma suave de la función subyacente.
  * **Flexibilidad matemática:** Una vez construido el spline, se puede evaluar la función o calcular la integral en cualquier subintervalo continuo dentro del rango $[0.00, 9.80]$, independientemente de los nodos discretos.

* **Limitaciones:**
  * **Mayor costo computacional:** Tardó **1.3215 ms** en completar el proceso. Esto representa un tiempo significativamente superior respecto al método directo (aproximadamente 6,600 veces más lento).
  * **Sobrecosto de inicialización:** Requiere asignar memoria en Heap (`gsl_spline_alloc`) y resolver un sistema tridiagonal de ecuaciones para calcular los coeficientes de las parábolas cúbicas en cada intervalo.

---

## 3. Tabla Comparativa de Resultados

| Criterio / Método | Integración Directa (Trapecio Explícito) | Interpolación + Integración (Spline Cúbico GSL) |
| :--- | :--- | :--- |
| **Valor Integrado ($I$)** | $21.19084634$ | $21.19188826$ |
| **Error Relativo (%)** | **$0.005530\%$** | **$0.000613\%$** ($\approx 9\times$ más preciso) |
| **Tiempo de Ejecución** | **$0.0002\text{ ms}$** ($\approx 6600\times$ más rápido) | **$1.3215\text{ ms}$** |
| **Complejidad Algorítmica** | $\mathcal{O}(N)$ en tiempo, $\mathcal{O}(1)$ en memoria | $\mathcal{O}(N)$ con setup tridiagonal y memoria dinámica |
| **Uso Recomendado** | Algoritmos en tiempo real, microcontroladores y alta frecuencia de muestreo. | Post-procesamiento, análisis *offline* y simulación que exijan máxima precisión. |

---

## 4. Conclusión e Implicaciones de Ingeniería

1. **Balance de Selección:** La elección del método depende del compromiso entre **precisión** y **recursos de cómputo**. Si la densidad de puntos es adecuada ($N=50, h=0.20$), la regla del Trapecio ofrece un error menor al $0.006\%$ con una carga de CPU despreciable.
2. **Criterio de Aplicación:** 
  * Para **sistemas embebidos o monitoreo continuo**, el **Trapecio Explícito** es superior por su baja latencia y mínimo consumo de memoria.
  * Para **herramientas de simulación o cálculo analítico *offline***, el **Spline Cúbico (GSL)** es la mejor opción para minimizar el error numérico y permitir interpolaciones en cualquier punto.