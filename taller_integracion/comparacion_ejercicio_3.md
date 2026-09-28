# Ejercicio 3: Diferenciación Numérica y Análisis Comparativo de Rendimiento

## 1. Resumen de Resultados Numéricos

El cálculo de la primera derivada de los datos experimentales $y(x)$ con paso de malla $h = 0.200000$ arrojó una coincidencia exacta entre los tres entornos de desarrollo (GNU Octave, Python y C). Tomando como referencia la diferencia centrada de orden $O(h^2)$, se cuantificó el error introducido por la aproximación hacia adelante (Forward Difference) de orden $O(h)$.

* **Paso de muestreo ($h$):** $0.200000$
* **Valor en punto de prueba $x[9]$ (Elemento 10):**
  * **Diferencia Hacia Adelante (Forward):** $0.322968$
  * **Diferencia Centrada (Central):** $0.286924$
  * **Función Nativa (`gradient` / `np.gradient`):** $0.286924$
* **Métricas de Error (Forward vs Centrada):**
  * **Error Absoluto Medio (MAE):** $0.040580$
  * **Error Cuadrático Medio (RMSE):** $0.046652$

---

## 2. Tablas Comparativas

### Tabla 1: Comparativa de Resultados Numéricos y Errores
| Método / Métrica | GNU Octave | Python | C | Valor / Métrica |
| :--- | :---: | :---: | :---: | :---: |
| **Forward Difference $O(h)$** | $0.322968$ | $0.322968$ | $0.322968$ | Punto $x[9]$ |
| **Central Difference $O(h^2)$** | $0.286924$ | $0.286924$ | $0.286924$ | Punto $x[9]$ |
| **Función Nativa** | $0.286924$ | $0.286924$ | N/A | Punto $x[9]$ |
| **MAE ($f'_{fwd}$ vs $f'_{cent}$)** | $0.040580$ | $0.040580$ | $0.040580$ | Error Promedio |
| **RMSE ($f'_{fwd}$ vs $f'_{cent}$)** | $0.046652$ | $0.046652$ | $0.046652$ | Dispersión |

### Tabla 2: Comparativa de Tiempos de Ejecución
| Entorno / Lenguaje | Diferencia Adelante $O(h)$ | Diferencia Centrada $O(h^2)$ | Función Nativa |
| :--- | :---: | :---: | :---: |
| **C** | **$0.0002\text{ ms}$** | **$0.0001\text{ ms}$** | N/A |
| **GNU Octave** | $0.0889\text{ ms}$ | $0.0279\text{ ms}$ | $4.0002\text{ ms}$ |
| **Python (NumPy)** | $0.2953\text{ ms}$ | $0.1485\text{ ms}$ | $0.1586\text{ ms}$ |

---

## 3. Análisis Técnico y Discusión

### A. Análisis Numérico y Teórico de los Métodos
1. **Precisión y Orden de Truncamiento:**
   La expansión en serie de Taylor demuestra que el método hacia adelante posee un error de truncamiento lineal de orden $O(h)$:
   $$f'_{fwd}(x_i) = \frac{f(x_{i+1}) - f(x_i)}{h} - \frac{h}{2}f''(\xi)$$
   En contraste, la diferencia centrada cancela los términos pares de la serie de Taylor, logrando una precisión cuadrática $O(h^2)$:
   $$f'_{cent}(x_i) = \frac{f(x_{i+1}) - f(x_{i-1})}{2h} - \frac{h^2}{6}f'''(\eta)$$
   Con un tamaño de paso $h = 0.2$, el término de error cuadrático disminuye aproximadamente en un factor de $h^2 = 0.04$, lo que explica por qué el valor de la derivada pasa de $0.322968$ (Forward) a $0.286924$ (Centrada).

2. **Evaluación global del Error (MAE y RMSE):**
   * El **MAE de $0.040580$** refleja la desviación promedio constante producida por el esquema Forward a lo largo del dominio.
   * El **RMSE de $0.046652$** (mayor que el MAE) evidencia que en los puntos donde la señal experimenta mayores variaciones de curvatura ($f''(x)$ elevado), el error del esquema de primer orden se amplifica sustancialmente.

### B. Análisis del Desempeño Computacional
1. **Lenguaje C vs Entornos Interpretados:**
   C demuestra la máxima eficiencia con un tiempo de $0.0001\text{ ms}$ para el algoritmo centrado, siendo aproximadamente $279$ veces más rápido que Octave y $1485$ veces más rápido que Python. Esto se debe a la compilación directa a lenguaje máquina y a la ausencia de la sobrecapa de gestión de memoria de los lenguajes interpretados.
2. **Funciones Nativas vs Algoritmos Manuales:**
   * En **Python**, `np.gradient()` ($0.1586\text{ ms}$) rinde de forma muy similar al bucle vectorizado manual ($0.1485\text{ ms}$), ya que ambas operaciones aprovechan las llamadas optimizadas en C subyacentes en NumPy.
   * En **GNU Octave**, la función nativa `gradient()` ($4.0002\text{ ms}$) fue sensiblemente más lenta que la implementación matricial manual ($0.0279\text{ ms}$). Esto sucede porque `gradient()` ejecuta validaciones adicionales de dimensiones, soporte para matrices multidimensionales y bordes de segundo orden.

---

## 4. Conclusión de las Gráficas

1. **Comportamiento y Desfase Phase-Shift (Gráfica Inferior):**
   * Como se observa en la figura, la curva roja discontinua (**Diferencia Adelante**) se encuentra ligeramente desplazada a la izquierda (adelantada en fase) respecto a la curva azul (**Diferencia Centrada**). Este desfase ocurre porque el esquema Forward evalúa la pendiente utilizando un punto futuro ($x_{i+1}$), asignando la tasa de cambio al punto actual $x_i$.
2. **Coincidencia Exacta con Métodos Nativos:**
   * La curva punteada negra de la función nativa (`np.gradient` / `gradient`) coincide de forma 100% idéntica con la línea azul continua de la **Diferencia Centrada**. Esto confirma que las librerías estándar en procesamiento numérico emplean esquemas centrados de orden $O(h^2)$ para todos los puntos interiores del dominio.
3. **Tratamiento de Fronteras y Estabilidad:**
   * La derivada aproximada sigue suavemente las oscilaciones de la señal original $y(x)$. En los bordes ($x_0$ y $x_{N-1}$), la transición mediante diferencias Forward/Backward evita discontinuidades bruscas en la curva de la derivada.