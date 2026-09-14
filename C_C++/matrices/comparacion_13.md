# Análisis de resultados en el Punto 1.3

## Comparación de Resultados (C++ vs. Octave)

| Métrica | C++ (Norma Infinito) | Octave (Norma 2) |
| :--- | :--- | :--- |
| **Número de Condición ($\text{cond}(A)$)** | 89.7448 | 51.7150 |
| **Error Relativo en $A$ ($\frac{\|\Delta A\|}{\|A\|}$)** | $4.1666 \times 10^{-5}$ | $5.1755 \times 10^{-5}$ |
| **Error Relativo en $x$ ($\frac{\|\Delta x\|}{\|x\|}$)** | $1.7021 \times 10^{-4}$ | $2.1392 \times 10^{-4}$ |
| **Factor de Amplificación Real** | 4.0852 | 4.1333 |
| **¿Cumple la Cota Teórica?** | Sí (4.08 $\le$ 89.74) | Sí (4.13 $\le$ 51.71) |

## Análisis de Estabilidad y Comportamiento

*   **Diferencia de Normas:** La divergencia en los números de condición entre ambos códigos no es un error, sino el resultado de usar diferentes métricas. Tu código en C++ calcula la norma infinito (basada en la máxima suma de filas para ahorrar cómputo), mientras que Octave utiliza por defecto la norma 2 (basada en valores singulares). Ambas demuestran exactamente el mismo fenómeno físico/matemático.
*   **La Cota Teórica:** El número de condición no dictamina el error exacto, sino el peor escenario posible. La ecuación matemática fundamental establece que el error relativo de la solución está limitado por el error de la matriz multiplicado por la condición de la matriz original:

$$
\frac{\|\Delta x\|}{\|x\|} \leq \text{cond}(A) \frac{\|\Delta A\|}{\|A\|}
$$

*   **Estabilidad del Sistema:** Los números de condición obtenidos (89.7 y 51.7) se consideran moderados. Esto significa que tu matriz original $A$ está bien condicionada. Por lo tanto, el sistema rechaza el ruido: una pequeña perturbación deliberada ($\sim 10^{-4}$) no destruye la solución, sino que genera un error de magnitud muy similar en la respuesta.
*   **Comportamiento del Factor de Amplificación:** En ambas plataformas, la perturbación se amplificó aproximadamente 4.1 veces. Este valor real es apenas una fracción diminuta del límite teórico máximo permitido (~90 o ~51), demostrando que la solución es sumamente estable.
*   **Robustez de los Algoritmos:** Debido al buen condicionamiento del sistema matemático, los métodos numéricos pueden brillar. Tanto tu implementación manual de Gauss con pivoteo parcial en C++ como el operador optimizado `\` de Octave lograron mantener el error de precisión a raya sin introducir inestabilidades adicionales por redondeo.
