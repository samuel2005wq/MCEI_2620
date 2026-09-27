# Ejercicio 2: Integración Numérica con Datos Discretos Experimentales

## 1. Tablas Comparativas de Resultados

### Resultados Numéricos y Rendimiento

| Entorno / Método | Integral Calculada ($I$) | Error Relativo (%) | Tiempo de Ejecución (ms) |
| :--- | :--- | :--- | :--- |
| **GNU Octave — Trapecio Compuesto** | 21.190846 | 0.005530% | 4.6661 ms |
| **GNU Octave — Simpson Combined** | 21.191953 | 0.000308% | 0.0780 ms |
| **C/C++ + GSL — Trapecio Explícito** | 21.19084634 | 0.005530% | 0.0002 ms |
| **C/C++ + GSL — Spline Cúbico** | 21.19188826 | 0.000613% | 1.2814 ms |
| **Python + SciPy — Trapecio (`scipy.integrate.trapezoid`)** | 21.19084634 | 0.005530% | 0.2376 ms |
| **Python + SciPy — Simpson (`scipy.integrate.simpson`)** | 21.19211310 | 0.000448% | 1.0613 ms |

*Valor analítico de referencia:* $I_{ref} = 21.19201822$

---

### Cuadro Comparativo

| Criterio | GNU Octave | C / C++ + GSL | Python + SciPy |
| :--- | :--- | :--- | :--- |
| **Facilidad de implementación** | **Muy Alta:** Lenguaje interpretado orientado a matrices; sintaxis matemática directa. | **Baja / Media:** Requiere gestión manual de memoria, compilación y enlace a bibliotecas externas (`-lgsl`). | **Alta:** Sintaxis limpia con paquetes maduros (`numpy`, `scipy`, `matplotlib`). |
| **Control del algoritmo** | **Medio:** Funciones nativas encapsuladas, aunque permite vectorizar operaciones vectoriales explícitas. | **Total:** Máximo nivel de control sobre estructuras de datos, precisión de punto flotante y ciclo de ejecución. | **Medio / Alto:** Permite alternar entre llamadas vectorizadas a bibliotecas de alto nivel y lógica procedural. |
| **Manejo de tolerancias** | **Limitado en datos discretos:** El paso $h$ viene fijo por el dataset; no admite malla adaptativa. | **Alto:** Control explícito mediante la configuración de tipo de spline e interpolador en GSL. | **Alto:** Rutinas avanzadas integradas para manejar tolerancias relativas y absolutas. |
| **Integración de funciones** | **Excelente:** Uso de `quad` y `trapz` en expresiones analíticas o numéricas. | **Excelente:** Implementación eficiente mediante cuadratura adaptativa (QAGS / QUADPACK en GSL). | **Excelente:** Rutinas dedicadas como `scipy.integrate.quad` con manejo de singularidades. |
| **Integración de datos** | **Directa:** Operaciones vectoriales nativas sobre arreglos (`trapz(x, y)`). | **Estructurada:** Requiere parsear el archivo CSV e iterar sobre arreglos o vectores GSL. | **Muy Directa:** Carga limpia con `pandas` o `numpy.loadtxt` e integración directa en `scipy`. |
| **Estimación del error** | **Manual:** Comparación con el valor de referencia o diferencias entre órdenes de aproximación. | **Manual / Métrico:** Análisis de residuos del spline o diferencia respecto al trapecio directo. | **Manual / Integrado:** Comparación de la diferencia relativa $\vert{}I_{simp} - I_{trap}\vert{} / I_{simp}$. |
| **Tiempo de ejecución** | **Variable:** Influenciado por el *cold start* inicial al cargar la función. | **Ultra Rápido:** Compilado a código nativo; escala de microsegundos en trapecio ($0.0002\text{ ms}$). | **Rápido:** Ejecución basada en código C optimizado en NumPy/SciPy ($0.2376\text{ ms}$). |
| **Visualización** | **Integrada:** Motor gráfico nativo potente (`figure`, `area`, `plot`, `grid`). | **Nula / Externa:** Requiere exportar datos a archivos o usar utilidades externas como `gnuplot`. | **Excelente:** Control total a través de `matplotlib` y `seaborn` para gráficos de alta calidad. |

---

## 2. Análisis Técnico de los Resultados

1. **Consistencia Exacta en Trapecio:**
   El método del trapecio entregó **exactamente el mismo valor de la integral ($I = 21.19084634$) y error relativo ($0.005530\%$)** en las tres plataformas. Esto confirma que la fórmula discreta del trapecio es estrictamente determinista y que la representación de punto flotante de doble precisión (`double`) se mantiene idéntica entre Octave, C++ y Python.

2. **Orden de Convergencia y Precisión:**
   - La regla del Trapecio ($O(h^2)$) presenta el error más alto ($0.005530\%$) debido a la aproximación lineal entre puntos.
   - Los métodos de orden superior (Simpson $1/3$ combinado con Trapecio en Octave, Simpson en SciPy y Spline Cúbico en GSL) redujeron el error numérico significativamente (alcanzando $0.000308\%$ en Octave, $0.000448\%$ en Python y $0.000613\%$ en C++).

3. **Desempeño Computacional:**
   - **C/C++** registró el menor tiempo en la regla del trapecio ($0.0002\text{ ms}$), superando ampliamente a los lenguajes de alto nivel.
   - En **Python**, la función de Simpson requirió $1.0613\text{ ms}$ debido a las verificaciones internas de SciPy sobre las dimensiones del arreglo y la paridad de subintervalos.
   - En **Octave**, la llamada a `trapz` marcó $4.6661\text{ ms}$ producto del tiempo de inicialización de la función (*cold start*), mientras que el cálculo manual vectorizado para Simpson tomó $0.0780\text{ ms}$.

---

## 3. Respuestas a "Reflexión y Evocación" (Sección 8)

### Pregunta 1: Estrategia al pasar de una función analítica $f(x)$ a datos experimentales discretos $(x_i, y_i)$

Al trabajar únicamente con un conjunto discreto de mediciones $(x_i, y_i)$:

* **Información que deja de estar disponible:**
  * **Evaluación en puntos arbitrarios:** No se puede consultar el valor del sistema en puntos intermedios fuera de la rejilla muestreada.
  * **Derivadas analíticas e información de curvatura:** Las cotas teóricas de error de truncamiento (que requieren derivar $f(x)$) ya no son calculables analíticamente.
  * **Adaptabilidad de la malla:** El paso de integración $h$ está predeterminado por la frecuencia de adquisición del sensor; no es posible refinar $h$ dinámicamente en zonas de alta variación.

* **Impacto en la selección del método:**
  * Se deben descartar métodos adaptativos de evaluación continua (como Cuadratura Gaussiana o algoritmos tipo $QAGS$) y emplear fórmulas de Newton-Cotes compuestas o esquemas de interpolación por Splines.
  * Al existir un número impar de puntos $N$, los subintervalos $n = N - 1$ son impares, lo que impide usar Simpson $1/3$ puro en todo el rango. Esto exige usar un esquema híbrido (Simpson $1/3$ en la mayoría de tramos + Trapecio en el subintervalo sobrante) o un Spline Cúbico.

### Pregunta 2: Reconstrucción del proceso y conceptos fundamentales para la implementación confiable

De la experiencia desarrollada en GNU Octave, C/C++ (GSL) y Python (SciPy), destacan los siguientes conceptos esenciales:

1. **Discretización y Paridad:** Comprender que un conjunto de $N$ datos define $n = N - 1$ subintervalos. Evaluar la paridad de $n$ determina si se puede aplicar directo un método de orden $O(h^4)$ o si se requiere un ajuste de borde.
2. **Propagación y Orden del Error:** Identificar que los métodos de orden superior (Simpson o Splines) atenúan de forma drástica el error de truncamiento frente a aproximaciones lineales (Trapecio) sin comprometer el tiempo de ejecución.
3. **Representación de Datos en Punto Flotante:** Utilizar tipos de datos de doble precisión (`double`) previene la acumulación de errores de redondeo al sumar series numéricas sobre mallas con muchos datos.
4. **Nivel de Abstracción del Entorno:**
   * **C/C++ (GSL):** Brinda el rendimiento máximo de ejecución a costa de gestionar manualmente memoria, estructuras de interpolación y compilación.
   * **Python (SciPy):** Ofrece el punto de equilibrio óptimo entre productividad de desarrollo y alta velocidad de cálculo.
   * **GNU Octave:** Es ideal para verificación matemática rápida e inspección gráfica de arreglos vectorizados.