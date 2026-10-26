# Análisis de Resultados Obtenidos en el Ejercicio de Aplicación del Taller

## 1. Preprocesamiento y Estadísticas de Referencia del Conjunto de Datos

Al inspeccionar el archivo `Datos_MCEI.csv` ($N = 100$ sensores), se identificaron las variables medidas en la red eléctrica: `voltaje_V`, `corriente_A` y `potencia_W_aprox` (potencia activa aproximada $P \approx V \cdot I$). Asimismo, se observó un artefacto de exportación decimal en los datos crudos (multiplicación por $100$ en valores con dos decimales y por $10$ en valores terminados en cero), por lo cual el análisis se estructuró en dos escenarios:

1. **Escenario A (Escala Física Reconstruida con Normalización $Z\text{-score}$):** Se restaura la magnitud física original de la red ($\sim 120\text{ V}$, $\sim 5\text{ A}$, $\sim 600\text{ W}$). El sensor anómalo (**Muestra 100**) registra $V = 151.50\text{ V}$, $I = 1.08\text{ A}$ y $P = 163.62\text{ W}$, representando una sobretensión severa a $8.59\sigma$ por encima de la media.
2. **Escenario B (Datos Crudos del CSV con Normalización $Z\text{-score}$):** Evalúa los valores enteros directos del archivo, donde la **Muestra 100** presenta $V = 1515$, $I = 108$ y $P = 16362$ siendo igualmente el sensor anómalo.

### Tabla 1: Estadísticas Descriptivas ($N = 100$ Sensores)

| Estadístico | `voltaje_V` (Crudo) | `corriente_A` (Crudo) | `potencia_W_aprox` (Crudo) | Voltaje Reconstruido $[\text{V}]$ | Corriente Reconstruida $[\text{A}]$ | Potencia Reconstruida $[\text{W}]$ |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Cantidad ($N$)** | $100$ | $100$ | $100$ | $100$ | $100$ | $100$ |
| **Media ($\mu$)** | $11129.7900$ | $441.5900$ | $57086.0400$ | $120.2799$ | $4.9046$ | $588.3024$ |
| **Desv. Estándar ($\sigma$)** | $2919.2848$ | $154.7697$ | $12375.8073$ | $3.6361$ | $0.7266$ | $86.5663$ |
| **Mínimo ($\text{mín}$)** | $1181.0000$ | $42.0000$ | $6360.0000$ | $115.2300$ | $1.0800$ | $163.6200$ |
| **Mediana ($P_{50}$)** | $11967.5000$ | $481.0000$ | $57927.5000$ | $119.9150$ | $4.9300$ | $584.1500$ |
| **Máximo ($\text{máx}$)** | $12447.0000$ | $637.0000$ | $76739.0000$ | $151.5000$ | $6.3700$ | $767.3900$ |

Para evitar que la magnitud nominal de la potencia domine numéricamente el cálculo de distancias frente a la corriente y el voltaje, se aplicó estandarización $Z\text{-score}$ sobre cada dimensión $j \in \{1, 2, 3\}$:

$$\tilde{x}_{ij} = \frac{x_{ij} - \mu_j}{\sigma_j}$$

---

## 2. Consolidado de Resultados Experimentales (KNN vs. $\text{kd-tree}$)

Se evaluó la búsqueda de los $k = 5$ vecinos más cercanos para la **Muestra 100** empleando tres métricas de la familia de normas de Minkowski de orden $p$:

$$d_p(\mathbf{x}_q, \mathbf{x}_i) = \left( \sum_{j=1}^{D} |x_{q,j} - x_{i,j}|^p \right)^{1/p}$$

* **Euclidiana ($L_2, p = 2$):** Distancia geométrica directa en el espacio tridimensional.
* **Manhattan ($L_1, p = 1$):** Suma de diferencias absolutas por coordenada.
* **Chebyshev ($L_\infty, p = \infty$):** Diferencia máxima absoluta entre todas las dimensiones ($\max_j |x_{q,j} - x_{i,j}|$).

### Tabla 2: Resultados de Vecindad y Tiempos de Ejecución ($N = 100$)

| Escenario | Métrica ($p$) | $5$ Vecinos (KNN = $\text{kd-tree}$) | Distancias Calculadas | Tiempo KNN ($\mu\text{s}$) | Build $\text{kd-tree}$ ($\mu\text{s}$) | Query $\text{kd-tree}$ ($\mu\text{s}$) | Total $\text{kd-tree}$ ($\mu\text{s}$) | Razón Query / KNN |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Escenario A** | **Euclidiana ($L_2$)** | `[64, 38, 24, 26, 4]` | `[9.6263, 9.6681, 9.8375, 9.8514, 9.8666]` | $71.36$ | $146.16$ | $110.78$ | $256.94$ | $1.55\times$ |
| **Escenario A** | **Manhattan ($L_1$)** | `[64, 38, 4, 72, 24]` | `[15.1281, 15.2782, 15.6618, 15.9826, 16.1494]` | $82.74$ | $146.16$ | $133.33$ | $279.49$ | $1.61\times$ |
| **Escenario A** | **Chebyshev ($L_\infty$)** | `[94, 82, 97, 89, 80]` | `[7.2880, 7.4337, 7.6290, 7.6565, 7.7252]` | $94.78$ | $146.16$ | $200.50$ | $346.66$ | $2.12\times$ |
| **Escenario B** | **Euclidiana ($L_2$)** | `[86, 68, 48, 74, 94]` | `[3.3404, 3.7934, 4.1192, 4.3779, 4.4094]` | $79.55$ | $123.31$ | $196.33$ | $319.64$ | $2.47\times$ |
| **Escenario B** | **Manhattan ($L_1$)** | `[68, 86, 48, 94, 74]` | `[4.2386, 4.7655, 5.8728, 6.2138, 6.2289]` | $111.43$ | $123.31$ | $294.75$ | $418.06$ | $2.65\times$ |
| **Escenario B** | **Chebyshev ($L_\infty$)** | `[86, 48, 65, 16, 11]` | `[2.7138, 3.3056, 3.4563, 3.4577, 3.4632]` | $130.49$ | $123.31$ | $256.17$ | $379.48$ | $1.96\times$ |

---

## 3. Desarrollo de los Puntos 3 y 4 del Ejercicio de Aplicación

### Punto 3: Comparación de Resultados (Tiempo de Ejecución, Precisión y Facilidad de Interpretación)

#### A. Tiempo de Ejecución
* **Desempeño en escala pequeña ($N = 100$):** El algoritmo **KNN por fuerza bruta** obtuvo un tiempo promedio de **$82.96\ \mu\text{s}$** en el Escenario A ($71.36\ \mu\text{s}$ en $L_2$, $82.74\ \mu\text{s}$ en $L_1$ y $94.78\ \mu\text{s}$ en $L_\infty$) y de **$107.16\ \mu\text{s}$** en el Escenario B. En contraste, la consulta del **$\text{kd-tree}$** requirió en promedio **$148.20\ \mu\text{s}$** en el Escenario A y **$249.08\ \mu\text{s}$** en el Escenario B, sumado a un costo inicial de construcción del árbol de **$146.16\ \mu\text{s}$** y **$123.31\ \mu\text{s}$**, respectivamente.
* **Causa computacional:** Para $N = 99$ candidatos en $\mathbb{R}^3$, la resta matricial y el cálculo de normas vectorizadas en NumPy operan sobre arreglos contiguos en la memoria caché L1 del procesador utilizando instrucciones vectoriales SIMD. Por el contrario, el recorrido de un árbol $\text{kd-tree}$ implica desreferenciar punteros entre nodos no contiguos, evaluar condicionales de poda (*backtracking*) e incurrir en sobrecosto de planificación de hilos (`workers=-1`).
* **Punto de cruce y escalabilidad:** En pruebas de escalabilidad sintética con $D = 3$ variables, a partir de **$N \approx 600\text{ a }1\,000$ sensores**, la complejidad logarítmica $\mathcal{O}(\log N)$ del $\text{kd-tree}$ supera al enfoque lineal $\mathcal{O}(N)$ de KNN. Para $N = 100\,000$ registros, la consulta del $\text{kd-tree}$ toma apenas $\sim 45.59\ \mu\text{s}$, mientras que KNN tarda $\sim 7\,600.38\ \mu\text{s}$ ($166\times$ más lento).

#### B. Precisión en la Identificación de Vecinos
* **Exactitud numérica:** Ambos métodos son algoritmos de **búsqueda exacta**. En las $6$ configuraciones evaluadas, tanto KNN como $\text{kd-tree}$ identificaron exactamente las mismas muestras y calcularon las mismas distancias hasta la cuarta cifra decimal (concordancia del $100\%$).
* **Fidelidad física e ingenieril:** La precisión física del agrupamiento depende del preprocesamiento. En el Escenario A, los vecinos identificados para la Muestra 100 (`[64, 38, 24, 26, 4]` en $L_2$) corresponden a sensores reales que operan con tensión superior al promedio ($120.83\text{ V}$ a $123.29\text{ V}$) y baja demanda de corriente/potencia. En el Escenario B, la pérdida del cero decimal agrupa a la Muestra 100 con las muestras `[86, 68, 48, 74, 94]`, las cuales comparten el mismo artefacto de escala en la columna `voltaje_V`.

#### C. Facilidad de Interpretación
* **KNN (Fuerza Bruta):** Ofrece una interpretación directa y tabular, ideal para inspeccionar el vector completo de distancias desde el sensor en falla hacia todos los demás sensores de la planta.
* **$\text{kd-tree}$ (Particionamiento Espacial):** Brinda una interpretación estructural y jerárquica del espacio de estados eléctrico. Cada nivel del árbol define hiperplanos de corte ortogonales sobre umbrales de voltaje ($V$), corriente ($I$) y potencia ($P$), dividiendo el régimen operativo de la planta en regiones o cajas tridimensionales fácilmente interpretables como reglas de decisión.

---

### Punto 4: Método Más Adecuado para un Sistema de Monitoreo en Tiempo Real en Ingeniería Eléctrica

La elección óptima para monitoreo en tiempo real dentro de una planta industrial o subestación eléctrica depende de la arquitectura de adquisición de datos:

1. **Régimen de Instantáneas Dinámicas de Pequeña Escala ($N \le 500$ sensores actualizados en cada ciclo $\Delta t$):**
   * Si un sistema SCADA o concentrador de datos evalúa en cada ventana de muestreo la similitud mutua entre los $100$ sensores de la planta (cuyas lecturas cambian simultáneamente en cada instante), el árbol espacial tendría que reconstruirse en cada ciclo ($\text{Build} + \text{Query} = 146.16\ \mu\text{s} + 110.78\ \mu\text{s} = 256.94\ \mu\text{s}$).
   * En este caso, **KNN por fuerza bruta vectorizado ($71.36\ \mu\text{s}$)** es **$3.60\times$ más rápido**, evita la reconstrucción de estructuras de datos y garantiza una latencia determinista.
2. **Régimen de Diagnóstico contra Bases Históricas Masivas ($N \ge 10^4$ estados) o Consultas Concurrentes en Paralelo:**
   * Cuando el sistema compara las lecturas entrantes en tiempo real contra un repositorio histórico de estados operativos normales y patrones de falla previamente indexados, el árbol **$\text{kd-tree}$** se construye una sola vez en memoria.
   * Cada nueva consulta se resuelve en tiempo logarítmico $\mathcal{O}(\log N)$, manteniendo tiempos de respuesta de decenas de microsegundos incluso con cientos de miles de registros.
   * Adicionalmente, ante eventos de falla múltiple en la red donde decenas de sensores requieren evaluación simultánea, la estructura $\text{kd-tree}$ (al ser de solo lectura durante la búsqueda) permite paralelizar las consultas por lotes (`workers=-1`) distribuyendo el trabajo entre todos los núcleos de la CPU.

---

## 4. Respuestas Argumentadas a las Preguntas Exploratorias (P1 a P5)

### P1: ¿Qué diferencias conceptuales encuentras entre un método basado en distancias como KNN y una estructura espacial como el $\text{kd-tree}$?
* **KNN (Evaluación Exhaustiva Sin Estructura Previa):** Es un método de aprendizaje perezoso (*lazy evaluation*) que carece de una fase de indexación geométrica. Trata el conjunto de datos como una lista plana de vectores en $\mathbb{R}^D$ y, ante una consulta $\mathbf{x}_q$, calcula explícitamente la distancia hacia la totalidad de los $N$ puntos con complejidad temporal $\mathcal{O}(N \cdot D)$.
* **$\text{kd-tree}$ (Particionamiento Espacial y Poda Jerárquica):** Es una estructura de datos basada en un árbol binario de búsqueda multidimensional que divide recursivamente el espacio $\mathbb{R}^D$ mediante hiperplanos ortogonales alineados a los ejes coordenados ($\text{eje} = \text{profundidad} \bmod D$), seleccionando la mediana en cada partición con un costo de construcción $\mathcal{O}(D \cdot N \log N)$. Conceptualmente, sustituye la fuerza bruta por una búsqueda dirigida (*branch-and-bound*): durante el recorrido de retroceso (*backtracking*), utiliza la distancia perpendicular al hiperplano divisor para descartar (podar) subárboles enteros sin evaluar los puntos dentro de ellos si:
  $$|x_{q,\text{eje}} - x_{\text{nodo},\text{eje}}| \ge d_{\text{máx}}(k)$$

### P2: ¿En qué escenarios prácticos crees que KNN puede ser más eficiente que un método basado en $\text{kd-tree}$, y viceversa?
* **Escenarios donde KNN es más eficiente:**
  1. **Redes de sensores pequeñas ($N < 500$):** Como se comprobó experimentalmente ($71.36\ \mu\text{s}$ en KNN frente a $110.78\ \mu\text{s}$ de consulta y $146.16\ \mu\text{s}$ de construcción en $\text{kd-tree}$ con métrica $L_2$), el acceso secuencial a memoria contigua y la vectorización hardware superan la sobrecarga de recorrer punteros en un árbol.
  2. **Flujos de datos altamente cambiantes (*Streaming* total):** Si todas las coordenadas del conjunto de referencia se modifican en cada instante de muestreo, el costo de reconstruir el $\text{kd-tree}$ en cada paso no se amortiza frente a una única consulta.
  3. **Alta dimensionalidad ($D > 20$):** Donde la poda espacial pierde eficacia y el árbol recorre casi todos los nodos con mayor costo administrativo que un barrido lineal.
* **Escenarios donde $\text{kd-tree}$ es más eficiente:**
  1. **Bases de datos de gran escala ($N \gg 10^3$) en baja dimensión ($D \le 10$):** Con $D = 3$ variables eléctricas ($V, I, P$) y grandes historiales de operación, el $\text{kd-tree}$ reduce miles de evaluaciones de distancia a unas pocas comparaciones logarítmicas $\mathcal{O}(\log N)$.
  2. **Sistemas de múltiples consultas sobre un catálogo estático o semi-estático:** El tiempo invertido en construir el árbol ($\mathcal{O}(N \log N)$) se amortiza rápidamente al ejecutar miles de consultas sucesivas.

### P3: ¿Qué impacto tienen la dimensionalidad y el tamaño de los datos en el desempeño de estos métodos?
* **Impacto del tamaño de la muestra ($N$):** En KNN por fuerza bruta, el tiempo de ejecución crece linealmente con $N$ ($\mathcal{O}(N)$), volviéndose prohibitivo cuando $N \ge 10^5$. En el $\text{kd-tree}$, el tamaño $N$ incrementa el tiempo de construcción ($\mathcal{O}(N \log N)$) y la huella de memoria, pero el tiempo promedio de búsqueda escala logarítmicamente ($\mathcal{O}(\log N)$), permaneciendo casi constante al aumentar el número de registros.
* **Impacto de la dimensionalidad ($D$) — Maldición de la Dimensionalidad:** En KNN, el tiempo crece de forma lineal con el número de variables ($\mathcal{O}(N \cdot D)$). Por el contrario, en el $\text{kd-tree}$ el número de visitas requeridas crece exponencialmente con la dimensión ($\sim \mathcal{O}(2^D)$ en el peor caso cuando $N$ no es mucho mayor que $2^D$). En dimensiones altas ($D > 20$), la hiperesfera del vecino más cercano intersecta simultáneamente una gran cantidad de hiperrectángulos del árbol, anulando la poda de ramas y degradando el desempeño del $\text{kd-tree}$ a $\mathcal{O}(N)$. En el problema del taller, al contar con solo $D = 3$ variables ($V, I, P$), el $\text{kd-tree}$ opera en su región óptima de eficiencia.

### P4: ¿Cómo influye la elección de la métrica de distancia (euclidiana, Manhattan, etc.) en los resultados de KNN?
La elección de la métrica modifica la topología de las superficies equidistantes alrededor del punto de consulta (esferas en $L_2$, octaedros en $L_1$ y cubos en $L_\infty$), impactando tanto la selección de vecinos como la eficiencia computacional:
* **Ponderación cuadrática vs. lineal de desviaciones (Euclidiana $L_2$ vs. Manhattan $L_1$):**
  * En el **Escenario A**, la métrica Euclidiana ($L_2$) identificó como vecinos a `[64, 38, 24, 26, 4]`, mientras que Manhattan ($L_1$) seleccionó a `[64, 38, 4, 72, 24]` (reemplazando la Muestra 26 por la Muestra 72 y subiendo la Muestra 4 al tercer lugar).
  * Esto ocurre porque la Muestra 100 presenta su mayor desviación en el eje de voltaje ($z_V = +8.5861$). Al elevar las diferencias al cuadrado ($\Delta z_j^2$), la distancia **Euclidiana ($L_2$)** penaliza severamente la brecha de voltaje y favorece a la **Muestra 26** ($123.29\text{ V}$, $|\Delta z_V| = 7.758$), obteniendo $d_{L_2}(100, 26) = 9.8514 < d_{L_2}(100, 72) = 9.9337$.
  * Por el contrario, la distancia **Manhattan ($L_1$)** suma linealmente las diferencias absolutas ($|\Delta z_V| + |\Delta z_I| + |\Delta z_P|$), premiando a la **Muestra 72** ($121.25\text{ V}, 4.03\text{ A}, 475.56\text{ W}$) por tener menor corriente y menor potencia simultáneamente: $d_{L_1}(100, 72) = 15.9826 < d_{L_1}(100, 26) = 16.3420$.
* **Robustez frente a valores atípicos en un solo eje:** En el **Escenario B**, Manhattan invierte los dos primeros vecinos (`[68, 86, ...]` frente a `[86, 68, ...]` en $L_2$) porque la Muestra 68 coincide estrechamente con la Muestra 100 en voltaje y corriente crudos ($|\Delta z_V| = 0.102, |\Delta z_I| = 0.362$) y solo difiere fuertemente en potencia ($|\Delta z_P| = 3.775$).
* **Efecto extremo de Chebyshev ($L_\infty$):** Al evaluar únicamente la máxima diferencia coordenada ($\max_j |\Delta z_j|$), en el Escenario A la distancia $L_\infty$ queda gobernada en un $100\%$ por el eje de voltaje, seleccionando simplemente los $5$ sensores con mayor tensión de la red (`[94, 82, 97, 89, 80]`) e ignorando por completo la corriente y la potencia.

### P5: ¿Qué limitaciones prácticas podrían surgir al implementar estos métodos en sistemas de ingeniería eléctrica de gran escala?
1. **Disparidad de escalas físicas y errores de telemetría:** En sistemas de potencia coexisten variables con diferentes órdenes de magnitud (voltajes en $\text{kV}$, corrientes en $\text{A}$, potencias en $\text{MW}$) y posibles errores de codificación o pérdida de decimales (como se demostró entre el Escenario A y el Escenario B). Sin un preprocesamiento riguroso de validación y estandarización dinámica, las métricas de distancia quedan sesgadas hacia la variable de mayor valor numérico.
2. **Rigidez estructural del $\text{kd-tree}$ ante flujos continuos de alta frecuencia (PMUs):** Las Unidades de Medición Fasorial (PMU) reportan entre $30$ y $60$ muestras por segundo. Dado que un $\text{kd-tree}$ estándar se desbalancea al insertar o eliminar nodos dinámicamente, requeriría reconstrucciones periódicas ($\mathcal{O}(N \log N)$) que pueden generar picos de latencia (*jitter*) incompatibles con protecciones eléctricas en tiempo real.
3. **Acoplamiento físico y colinealidad entre variables:** En un circuito eléctrico, la potencia depende físicamente del voltaje y la corriente ($P = V \cdot I \cdot \cos\phi$). Aplicar distancias euclidianas o Manhattan sobre $(V, I, P)$ asume que los ejes son independientes y ortogonales, sobreponderando la información energética. En sistemas de gran escala resulta necesario desacoplar las variables (mediante PCA) o emplear la **distancia de Mahalanobis**, que pondera las distancias utilizando la inversa de la matriz de covarianza $\mathbf{\Sigma}^{-1}$.
4. **Sobrecosto de paralelización en lotes pequeños:** Distribuir consultas en múltiples hilos (`workers=-1`) introduce tiempos de sincronización del sistema operativo que penalizan las consultas individuales (como se observó en los tiempos de consulta para $N = 100$), siendo rentable únicamente cuando se procesan lotes masivos de consultas concurrentes.

---

## 5. Conclusiones Generales

1. **Equivalencia de exactitud y rol del preprocesamiento:** Se verificó experimentalmente que tanto KNN por fuerza bruta como $\text{kd-tree}$ entregan exactamente el mismo conjunto de $5$ vecinos y las mismas distancias en todas las métricas evaluadas. Sin embargo, la reconstrucción de los decimales físicos y la normalización $Z\text{-score}$ resultaron determinantes para que la vecindad matemática refleje la realidad física del sistema eléctrico.
2. **Criterio de selección computacional:** Para la red de $N = 100$ sensores del taller, el enfoque **KNN vectorizado** es la alternativa más rápida ($71.36\ \mu\text{s}$ en $L_2$) debido a la localidad de caché y la ausencia de fase de construcción ($146.16\ \mu\text{s}$ en $\text{kd-tree}$). No obstante, para bases de datos históricas superiores a $N = 1\,000$ registros en baja dimensión ($D = 3$), el **$\text{kd-tree}$** se consolida como la estructura superior por su escalabilidad logarítmica $\mathcal{O}(\log N)$.
3. **Selección de la métrica de distancia en ingeniería eléctrica:** La métrica **Euclidiana ($L_2$)** es la más apropiada cuando se requiere priorizar desviaciones severas en variables críticas (como picos de sobretensión en la Muestra 100), mientras que la métrica **Manhattan ($L_1$)** ofrece mayor estabilidad cuando se busca similitud equilibrada en múltiples variables o existen ruidos puntuales de medición en un solo canal.