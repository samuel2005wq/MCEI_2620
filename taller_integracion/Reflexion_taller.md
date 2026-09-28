# 8. Reflexión y evocación

## 1. Estrategia al pasar de una función analítica $f(x)$ a datos experimentales discretos $(x_i, y_i)$

Al abandonar la expresión continua de una función $f(x)$ para trabajar exclusivamente con un conjunto discreto de datos $(x_i, y_i)$, la estrategia de cálculo numérico cambia radicalmente debido a la pérdida de información clave:

### Información que deja de estar disponible
* **Evaluación en puntos arbitrarios:** No es posible consultar el valor del sistema en coordenadas intermadias fuera de la grilla muestreada original.
* **Derivadas analíticas e información de curvatura:** Las derivadas de orden superior ($f''(x)$, $f^{(4)}(x)$, etc.) ya no son calculables de forma exacta, por lo que se pierde la capacidad de evaluar a priori las cotas teóricas del error de truncamiento.
* **Adaptabilidad de la malla:** El paso de integración o diferenciación $h$ queda prefijado por el sensor o la tasa de adquisición; no es posible reducir $h$ dinámicamente en zonas con fuertes fluctuaciones.

### Impacto en la selección del método
* **Para Integración:**
  * Se deben descartar los métodos adaptativos continuos (como la Cuadratura de Gauss-Kronrod en `qags` o `quadgk`) que evalúan $f(x)$ en nodos no uniformes.
  * Se debe recurrir a **fórmulas compuestas de Newton-Cotes** (Trapecio, Simpson) o **Splines Cúbicos**.
  * Si el número de datos $N$ es par, el número de subintervalos $n = N - 1$ es impar, lo que impide usar Simpson 1/3 puro en todo el dominio; esto obliga a utilizar un **esquema híbrido** (Simpson 1/3 en los primeros subintervalos + Trapecio en el último) o un ajuste por Spline Cúbico.
* **Para Diferenciación:**
  * Imposibilidad de aplicar diferenciación simbólica o exacta.
  * Obliga al uso de **diferencias finitas** (Forward, Backward, Central). Para los nodos interiores se priorizan las **diferencias centradas $O(h^2)$** por su mayor precisión de truncamiento, reservando los esquemas Forward/Backward de orden $O(h)$ o no centrados únicamente para los bordes del dominio ($x_0$ y $x_{N-1}$).

---

## 2. Reconstrucción del proceso en Octave, C/C++–GSL y Python–SciPy y conceptos fundamentales

De la implementación y análisis comparativo realizado en los tres entornos de desarrollo, se identifican los siguientes conceptos como pilares fundamentales para el cálculo numérico confiable:

### A. Discretización y Geometría de la Malla
Comprender la relación estructural entre $N$ puntos de datos y $n = N - 1$ subintervalos. Evaluar la paridad de $n$ determina si los algoritmos de orden superior (como Simpson 1/3) se pueden aplicar directamente o si requieren correcciones de borde para mantener la consistencia matemática.

### B. Orden del Error de Truncamiento vs. Selección del Algoritmo
* **En Integración:** Se evidencia claramente la brecha de precisión entre aproximaciones lineales ($O(h^2)$ del Trapecio) frente a cuadráticas o cúbicas ($O(h^4)$ en Simpson y Splines). Pasar a un método de orden superior reduce el error relativo en varios órdenes de magnitud sin representar un costo computacional significativo sobre datos ya almacenados.
* **En Diferenciación:** El método centrado $O(h^2)$ cancela los términos pares de la serie de Taylor, reduciendo drásticamente la dispersión y el error medio (MAE y RMSE) respecto al método hacia adelante $O(h)$.

### C. Tolerancias y Mecanismos de Control
* **Mallas Adaptativas (Funciones continuas):** La tolerancia (`epsabs`, `epsrel`) actúa como criterio de calidad, permitiendo al algoritmo subdividir localmente solo donde se requiere.
* **Mallas Fijas (Datos discretos):** La tolerancia no es un parámetro configurable del método, sino una consecuencia directa de la densidad de muestreo original ($h$).

### D. Representación de Datos en Punto Flotante
El uso estricto de tipos de datos de doble precisión (`double` IEEE 754) es indispensable para prevenir la acumulación de errores de redondeo en sumatorias extensas (integración) y evitar la cancelación catastrófica al restar valores muy cercanos (diferenciación).

### E. Nivel de Abstracción y Desempeño Computacional por Entorno
* **C/C++ + GSL:** Entrega el rendimiento computacional máximo (tiempos en la escala de microsegundos/nanosegundos) y control absoluto sobre la memoria, siendo ideal para sistemas embebidos o tiempo real, a costa de una implementación más verbosa.
* **Python + SciPy:** Proporciona el punto de equilibrio óptimo entre productividad de desarrollo y alta velocidad de cálculo, dado que sus rutinas internas están compiladas en C/Fortran.
* **GNU Octave:** Es excelente para prototipado rápido e inspección matemática vectorial directa. Sin embargo, se debe considerar la sobrecarga del intérprete en bucles continuos o funciones nativas que realizan validaciones exhaustivas en cada llamada.