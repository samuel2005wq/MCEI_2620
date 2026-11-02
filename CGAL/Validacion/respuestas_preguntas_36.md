### 3.6. Preguntas de análisis de resultados

**Condiciones de los experimentos.** Todos los resultados que se discuten a continuación salen del programa `taller.cpp`. Se probaron cuatro tamaños de nube ($n = 100, 500, 1000$ y $5000$) con tres semillas ($7$, $314$ y $1999$) sobre el dominio $[0,100]\times[0,100]$. Para cada combinación la nube se genera **una sola vez** y la envolvente y la triangulación se calculan **5 veces** sobre esa misma nube, para un total de $4 \times 3 \times 5 = 60$ mediciones. Los datos se corrieron en un portátil con AMD Ryzen 7 6800H with Radeon Graphics (3.20 GHz) y 16 GB de RAM, usando Ubuntu sobre WSL. En las 60 mediciones la triangulación pasó la validación `is_valid()` de CGAL y la relación de Euler.

---

#### Pregunta 1. ¿Qué proporción de los puntos generados pertenece a la frontera de la envolvente convexa? ¿Cómo cambia al aumentar el número de puntos?

| $n$ | Vértices de la envolvente $h$ (promedio de 3 semillas) | Rango de $h$ según la semilla | Proporción $h/n$ |
|---|---|---|---|
| 100 | 12.67 | 11 – 15 | 12.67 % |
| 500 | 18.33 | 17 – 21 | 3.67 % |
| 1000 | 21.00 | 18 – 23 | 2.10 % |
| 5000 | 21.67 | 20 – 25 | 0.43 % |

La proporción **disminuye muy rápido** al aumentar $n$: con 100 puntos casi 1 de cada 8 está en la frontera, mientras que con 5000 puntos es menos de 1 de cada 200. La razón es que el número de vértices de la envolvente crece muy lentamente. Al multiplicar $n$ por 50 (de 100 a 5000), $h$ solo se multiplica por 1.7 (de 12.7 a 21.7), así que el cociente $h/n$ tiende a cero.

En la gráfica `grafica_comparacion.jpeg` se muestra esto: los vértices de la envolvente (rojo) suben despacio y su porcentaje (verde) cae.

---

#### Pregunta 2. ¿Por qué la envolvente convexa no contiene información sobre la conectividad de los puntos interiores?

Porque la envolvente convexa solo responde a la pregunta *"¿qué puntos forman el contorno?"* y su resultado es únicamente una lista ordenada de $h$ vértices.

Además, aunque se conociera la lista de puntos interiores, la envolvente no dice cuáles son vecinos de cuáles, ni cuántos vecinos tiene cada uno, ni cómo se podrían unir en triángulos. Dos nubes muy distintas por dentro pueden tener exactamente la misma envolvente. Solo describe la frontera externa de la nube, no su estructura interna.

---

#### Pregunta 3. ¿Qué información adicional aporta la triangulación de Delaunay?

La triangulación de Delaunay conecta **todos** los puntos, no solo los del borde, y aporta justo lo que le falta a la envolvente:

* **Conectividad y vecindad:** cada arista une dos puntos vecinos, de modo que se sabe quién es vecino de quién. En nuestros datos cada punto tiene en promedio $2E/V$ vecinos: 5.69, 5.91, 5.95 y 5.99 para $n = 100, 500, 1000$ y $5000$, es decir, tiende a 6 (el valor típico de una triangulación plana grande).
* **Una malla de triángulos** sobre toda la región, con la propiedad del círculo circunscrito vacío, que evita triángulos demasiado alargados en la medida de lo posible.
* **Incluye a la envolvente:** la frontera exterior de la triangulación coincide con el contorno convexo. En la figura de $n = 100$ (semilla 7), las 12 aristas rojas de la envolvente son también aristas de la triangulación (en gris).

---

#### Pregunta 4. ¿Cuántos vértices, aristas y caras finitas aparecen para cada tamaño de nube? ¿Los resultados concuerdan con las relaciones de Euler para una triangulación plana?

Promedio de las tres semillas:

| $n$ | Vértices $V$ | Aristas finitas $E$ | Caras finitas $F$ | $3V-3-h$ | $2V-2-h$ | $V-E+F$ |
|---|---|---|---|---|---|---|
| 100 | 100 | 284.33 | 185.33 | 284.33 | 185.33 | 1 |
| 500 | 500 | 1478.67 | 979.67 | 1478.67 | 979.67 | 1 |
| 1000 | 1000 | 2976.00 | 1977.00 | 2976.00 | 1977.00 | 1 |
| 5000 | 5000 | 14975.33 | 9976.33 | 14975.33 | 9976.33 | 1 |

Y el detalle de cada corrida (las tres semillas por tamaño):

| $n$ | Semilla | $h$ | $V$ | $E$ | $3V-3-h$ | $F$ | $2V-2-h$ | $V-E+F$ |
|---|---|---|---|---|---|---|---|---|
| 100 | 7 | 12 | 100 | 285 | 285 | 186 | 186 | 1 |
| 100 | 314 | 11 | 100 | 286 | 286 | 187 | 187 | 1 |
| 100 | 1999 | 15 | 100 | 282 | 282 | 183 | 183 | 1 |
| 500 | 7 | 21 | 500 | 1476 | 1476 | 977 | 977 | 1 |
| 500 | 314 | 17 | 500 | 1480 | 1480 | 981 | 981 | 1 |
| 500 | 1999 | 17 | 500 | 1480 | 1480 | 981 | 981 | 1 |
| 1000 | 7 | 23 | 1000 | 2974 | 2974 | 1975 | 1975 | 1 |
| 1000 | 314 | 22 | 1000 | 2975 | 2975 | 1976 | 1976 | 1 |
| 1000 | 1999 | 18 | 1000 | 2979 | 2979 | 1980 | 1980 | 1 |
| 5000 | 7 | 25 | 5000 | 14972 | 14972 | 9973 | 9973 | 1 |
| 5000 | 314 | 20 | 5000 | 14977 | 14977 | 9978 | 9978 | 1 |
| 5000 | 1999 | 20 | 5000 | 14977 | 14977 | 9978 | 9978 | 1 |

**Sí concuerdan, exactamente, en las 12 ejecuciones.** Para una triangulación plana de $V$ puntos en posición general, con $h$ vértices en la frontera, la fórmula de Euler con la cara exterior excluida da $V - E + F = 1$, y de ella se derivan $E = 3V - 3 - h$ y $F = 2V - 2 - h$. En todos los casos el número de vértices de Delaunay es igual a $n$ (no se pierde ningún punto) y las columnas teóricas coinciden con las medidas.

Dos observaciones que sirven para interpretar la tabla:

* Cuando $n$ crece, $E/V \to 3$ y $F/V \to 2$ (en $n = 5000$ son 2.995 y 1.995). Es decir, el tamaño de la triangulación es **lineal** en $n$, como se espera.
* Como $E = 3V - 3 - h$, entre semillas distintas la variación de $E$ es exactamente la variación de $h$ en sentido contrario: para $n = 100$, $h$ va de 11 a 15 y $E$ de 286 a 282. Por eso $E$ casi no cambia en términos relativos, mientras que $h$ sí.

(Nota técnica: en nuestra versión de CGAL (5.6) `number_of_edges()` no existe, así que las aristas se contaron con `std::distance` sobre las aristas finitas.)

---

#### Pregunta 5. ¿Cómo cambia el tiempo de ejecución al aumentar el número de puntos? ¿Qué factores pueden afectar la medición?

Promedio de 15 mediciones por tamaño (3 semillas × 5 repeticiones):

| $n$ | Delaunay (ms) | Desv. est. | Envolvente (ms) | Desv. est. | Delaunay $\mu s$/punto | Factor de tiempo Delaunay vs. $n$ anterior | Factor de $n$ |
|---|---|---|---|---|---|---|---|
| 100 | 0.0361 | 0.0185 | 0.0067 | 0.0054 | 0.361 | — | — |
| 500 | 0.2231 | 0.0438 | 0.0251 | 0.0073 | 0.446 | 6.18 | 5 |
| 1000 | 0.4636 | 0.1901 | 0.0553 | 0.0343 | 0.464 | 2.08 | 2 |
| 5000 | 2.3905 | 0.2501 | 0.2531 | 0.0384 | 0.478 | 5.16 | 5 |

**Cómo cambia.** El tiempo crece de forma **casi lineal con un pequeño factor logarítmico**, coherente con $O(n \log n)$: al multiplicar $n$ por 5, el tiempo se multiplica por 5 a 6 (no por 25, como sería en un algoritmo cuadrático), y el costo por punto crece muy despacio, de 0.36 a 0.48 $\mu s$. En la gráfica `grafica_comparacion.jpeg` (escala log-log) la curva de Delaunay sigue de cerca la referencia $n\log n$. Además, la envolvente es entre 5 y 9 veces más rápida que la triangulación (0.253 ms frente a 2.39 ms con 5000 puntos), lo cual tiene sentido: la envolvente solo produce unos 20 vértices, mientras que Delaunay construye una estructura de casi $3n$ aristas.

**Factores que afectan la medición.** Los datos mismos muestran varios:

* **Primera repetición más lenta.** Por efectos de arranque (memoria, cachés, asignación dinámica), la repetición 1 es la más costosa. Con $n = 100$, Delaunay tarda en promedio 0.064 ms en la primera repetición frente a 0.029 ms en las demás, y la envolvente 0.015 ms frente a 0.005 ms. Por eso se calculó además el promedio sin la primera repetición (0.0292, 0.2116, 0.4520 y 2.3473 ms para Delaunay). Las diferencias son grandes para $n = 100$ (un 19 %) y pequeñas para $n \ge 500$ (entre 2 % y 5 %). En la tabla principal se reporta el promedio de las 5 repeticiones y el otro valor se usa como comprobación.
* **Valores atípicos por el sistema.** Con $n = 1000$ y semilla 7, la repetición 4 tardó 1.123 ms mientras las otras cuatro estuvieron cerca de 0.39 ms (casi tres veces más). Es una interrupción del sistema, no un cambio en el algoritmo, y explica que la desviación en $n = 1000$ (0.19 ms) sea mayor que en $n = 500$. Aquí la mediana (0.401 ms) es más representativa que la media (0.464 ms). Para $n = 5000$ la dispersión baja a un 8 – 13 %, porque la medición es más larga que el ruido.
* **Entorno de ejecución.** El código corre sobre WSL dentro de Windows, con otros procesos activos, escalado de frecuencia de la CPU y 16 GB de memoria compartida.
* **Qué se mide.** Solo se cronometra la construcción (`convex_hull_2` y `insert`); la generación de puntos, `is_valid()` y la escritura de archivos quedan fuera.
* **Geometría de la nube.** Cada semilla produce una nube distinta y el costo real de la inserción depende ligeramente de la distribución; esto es mucho menos importante que el ruido, pero existe.

---

#### Pregunta 6. ¿Qué ocurre si se introducen puntos repetidos, colineales o cocirculares? Diseñe experimentos para estudiar estos casos.

Para estudiar los casos degenerados se escribió el programa `experimentos_pregunta_6.cpp`. En cada experimento se plantea una hipótesis, se construye una nube específica y se comparan los conteos de CGAL.

| Experimento | Entrada | Puntos insertados | Vértices de envolvente | Vértices de Delaunay | Aristas | Caras | Dimensión |
|---|---|---|---|---|---|---|---|
| 1a. Sin repetidos | 100 puntos aleatorios | 100 | 9 | 100 | 288 | 189 | 2 |
| 1b. Con repetidos | los mismos 100 puntos, cada uno dos veces | 200 | 9 | 100 | 288 | 189 | 2 |
| 2. Colineales | 50 puntos sobre $y = x$ | 50 | 2 | 50 | 49 | 0 | 1 |
| 3a. Cocirculares | 12 puntos sobre una circunferencia de radio 5 | 12 | 12 | 12 | 21 | 10 | 2 |
| 3b. Malla regular | cuadrícula $10 \times 10$ | 100 | 4 | 100 | 261 | 162 | 2 |
| 5. Casi colineales | 200 puntos sobre $y = x$ con ruido de $10^{-9}$ | 200 | 14 | 200 | 583 | 384 | 2 |

**1. Puntos repetidos.** Se insertó dos veces cada uno de 100 puntos. CGAL **ignora los duplicados**: la triangulación sigue teniendo 100 vértices y el mismo número de aristas y caras que la nube sin repetidos, y la envolvente tampoco cambia. Esto es lo adecuado, porque un punto repetido no aporta información geométrica, pero hay que tener cuidado al interpretar los conteos: `number_of_vertices()` no es igual a la cantidad de puntos insertados. Si se necesita conservar el identificador de cada punto repetido, hay que manejarlo aparte.

**2. Puntos colineales.** Con 50 puntos sobre $y = x$ la triangulación deja de ser plana: su dimensión es 1, es una cadena de 49 aristas y **no tiene caras**. La envolvente se reduce a un segmento con sus dos extremos. Las fórmulas $E = 3V - 3 - h$ y $F = 2V - 2 - h$ ya no aplican (darían $E = 145$), así que antes de usarlas hay que verificar con `dt.dimension()` que la triangulación sea de dimensión 2. Un programa que asuma que siempre existen caras fallaría con esta entrada.

**3. Puntos cocirculares.** Con los 12 puntos sobre una circunferencia, los 12 pertenecen a la envolvente y la triangulación resultante tiene $E = 21$ y $F = 10$, que cumple las fórmulas de Euler ($3 \cdot 12 - 3 - 12 = 21$ y $2 \cdot 12 - 2 - 12 = 10$). El detalle es que el criterio del círculo vacío no distingue entre varias triangulaciones: cuando 4 o más puntos son cocirculares, **la triangulación de Delaunay no es única**. Aquí cualquiera de las triangulaciones posibles de un polígono convexo de 12 lados (hay $C_{10} = 16796$) es válida, y CGAL devuelve una de ellas.

En la malla regular $10 \times 10$ el mismo fenómeno aparece en cada cuadrado unitario (4 puntos cocirculares, y la diagonal elegida es arbitraria). Hay además un resultado que vale la pena destacar: la envolvente reporta solo $h = 4$ vértices (las esquinas), pero las fórmulas de Euler solo cuadran si se usa el número de vértices de la **frontera de la triangulación**, que incluye los puntos colineales de los lados: $3 \cdot 100 - 3 - 36 = 261$ aristas y $2 \cdot 100 - 2 - 36 = 162$ caras, que coinciden con lo medido. Con $h = 4$ se obtendría 293 y 194, que sería incorrecto. Es decir, en configuraciones con puntos colineales sobre la frontera, $h$ en las fórmulas debe contar también esos puntos.

**4. Nube casi colineal.** Con 200 puntos sobre $y = x$ con un ruido de $10^{-9}$, CGAL construye sin problemas una triangulación de dimensión 2 con 200 vértices, 583 aristas y 384 caras, que cumple Euler con $h = 14$ ($3 \cdot 200 - 3 - 14 = 583$ y $2 \cdot 200 - 2 - 14 = 384$). El programa no falla, pero los triángulos son extremadamente delgados, porque todos los puntos caen en una franja de ancho del orden de $10^{-9}$ m. Es un caso en el que la robustez del predicado evita errores de clasificación, pero la calidad de los triángulos (por ejemplo, para una malla de elementos finitos) sería muy mala.

**Conclusión:** los predicados exactos hacen que CGAL no se caiga ni dé estructuras inconsistentes ante estos casos, pero el programador sigue siendo responsable de interpretarlos: manejar duplicados, verificar la dimensión de la triangulación antes de leer caras, y contar correctamente los vértices de frontera cuando hay puntos colineales.

---

#### Pregunta 7. ¿Qué diferencias observa entre ejecutar el mismo experimento con distintas semillas y repetirlo con la misma semilla?

**Misma semilla: la geometría es idéntica y solo cambia el tiempo.** El generador `std::mt19937` es determinista, así que una semilla fija produce siempre la misma nube. Como cada combinación $(n, \text{semilla})$ se midió 5 veces sobre la misma nube, se puede comprobar: en las 60 mediciones la columna `geometria_reproducible` vale 1, es decir, el número de vértices de la envolvente, de aristas y de caras fue **el mismo en las 5 repeticiones** de cada ejecución. Lo único que varió fue el tiempo, y esa variación es ruido del sistema.

**Distintas semillas: cambia la geometría.** Cada semilla es una nube distinta del mismo experimento aleatorio:

| $n$ | $h$ (mín. – máx.) | $E$ (mín. – máx.) | $F$ (mín. – máx.) |
|---|---|---|---|
| 100 | 11 – 15 | 282 – 286 | 183 – 187 |
| 500 | 17 – 21 | 1476 – 1480 | 977 – 981 |
| 1000 | 18 – 23 | 2974 – 2979 | 1975 – 1980 |
| 5000 | 20 – 25 | 14972 – 14977 | 9973 – 9978 |

El número de vértices de la envolvente varía bastante en términos relativos (con $n = 100$ va de 11 a 15, más de un 30 % entre el mínimo y el máximo), mientras que $E$ y $F$ casi no cambian (menos del 1.5 %), porque están ligados a $n$ por las relaciones de Euler y solo dependen de $h$ por un valor pequeño. En los tiempos, las diferencias entre semillas son pequeñas y están dominadas por el ruido: por ejemplo, para $n = 1000$ el tiempo medio de Delaunay fue 0.552, 0.425 y 0.414 ms para las semillas 7, 314 y 1999, pero la semilla 7 se ve más lenta solo por el valor atípico de una de sus repeticiones.

**Cuándo usar cada una:**

* **Misma semilla** sirve para **reproducir y depurar**: cualquiera que corra el programa con los mismos argumentos obtiene la misma nube y los mismos conteos. Es lo que permite comparar con otro compañero o con otra implementación. Eso sí, la reproducibilidad completa se asegura con el mismo compilador y la misma biblioteca estándar, porque el algoritmo interno de `uniform_real_distribution` depende de la implementación.
* **Distintas semillas** sirven para saber **qué tan representativo** es un resultado: dan una idea de la variabilidad natural del experimento (por ejemplo, el rango de $h$) y evitan sacar conclusiones de una sola nube que podría ser atípica.
* **Repeticiones sobre la misma nube** permiten separar la variación del **tiempo** (ruido del sistema) de la variación de la **geometría** (entre semillas), que fue justo el diseño que se adoptó en este taller.
