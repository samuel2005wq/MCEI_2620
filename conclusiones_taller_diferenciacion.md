# Comparación de resultados, análisis y conclusiones

## 7. Comparación de resultados

| Criterio | Octave | Python | C/C++ + GSL |
|---|---|---|---|
| Carga de datos | Manual con `fileread` y `sscanf` por el formato `;` y coma decimal (`dlmread` si fuera estándar) | `np.loadtxt` con `io.StringIO`, o `pandas.read_csv(sep=";", decimal=",")` | Manual: `ifstream`, `getline`, reemplazo de caracteres y `istringstream` |
| Cálculo de ẋ, ẏ | Bucle `for` explícito con diferencias centrales | `np.gradient(x, t)` vectorizado | Función `derivar` con bucle y denominador `t[i+1]-t[i-1]` |
| Cálculo de v | `sqrt(xd.^2 + yd.^2)` vectorizado | `np.sqrt(vx**2 + vy**2)` | Bucle con `sqrt` (`<cmath>`) |
| Cálculo de θ | `atan2` + `unwrap` | `np.arctan2` + `np.unwrap` | `atan2` + función propia de desenvolvimiento (`desenvolver`) |
| Cálculo de ω | Bucle con diferencias centrales sobre `theta` | `np.gradient(theta, t)` | Reutiliza `derivar` sobre `theta` |
| Manejo de arreglos | Matrices y vectores nativos, operaciones elemento a elemento con `.*` y `.^` | Arreglos `ndarray`, vectorización y slicing | `std::vector` con índices, memoria y bucles manuales |
| Facilidad de implementación | Alta, sintaxis cercana a las matemáticas | Muy alta, es la más corta (pocas líneas) | Baja, más código y hay que implementar `unwrap` |
| Control sobre el algoritmo | Alto (se ve cada paso) | Medio (`np.gradient` oculta el detalle, pero se puede reemplazar) | Total (todo es explícito: extremos, paso variable, precisión) |
| Tiempo de ejecución | Medio, los bucles de Octave son lentos | Rápido, vectorizado en C por debajo | El más rápido, código compilado |

> Para unos pocos cientos de puntos la diferencia de tiempos es imperceptible; solo se nota con millones de datos.

---

## 8. Preguntas de análisis

### 1. ¿Qué diferencia hay entre calcular la velocidad lineal a partir de ẋ, ẏ y calcularla directamente desde diferencias de posición?

Las dos formas buscan lo mismo: saber qué tan rápido se mueve el robot. La diferencia está en **qué se mide y en qué momento**.

- **Con ẋ y ẏ:** primero se calcula qué tan rápido cambia cada coordenada por separado y luego se combinan con `v = √(ẋ² + ẏ²)`. Con diferencias centrales, el resultado queda "centrado" en el punto que estamos mirando, porque usa el punto anterior y el siguiente. Además, al tener ẋ y ẏ por separado, conservamos la dirección del movimiento, que después necesitamos para calcular θ.
- **Directamente desde la posición:** se mide la distancia entre dos puntos consecutivos y se divide entre el tiempo que pasó. Esto da la velocidad promedio entre esos dos instantes, no la velocidad en un punto exacto.

En resumen: si se usa el mismo tipo de diferencia, los números son casi iguales. La ventaja de pasar por ẋ y ẏ es que la versión central es más precisa en cada punto y nos entrega la dirección, que es necesaria para el resto del análisis.

### 2. ¿Por qué un error pequeño en ẋ, ẏ puede producir un efecto mayor al calcular ω?

Porque **ω se obtiene derivando dos veces**: primero se derivan las posiciones para obtener ẋ y ẏ, de ahí sale el ángulo θ, y luego se vuelve a derivar θ para obtener ω.

Cada vez que se deriva, se divide entre un intervalo de tiempo muy pequeño, y eso **agranda los errores**. Un error chiquito en ẋ o ẏ cambia un poco el ángulo θ, y al derivarlo otra vez ese cambio se vuelve mucho más notorio en ω. Es como una gota de tinta que se va extendiendo cada vez más a medida que pasa por cada paso del cálculo.

Además, cuando el robot va muy despacio, su dirección es difícil de determinar y el ángulo se vuelve muy sensible a cualquier error.

### 3. ¿Qué ocurre si se reduce h manteniendo un nivel de ruido fijo en las posiciones?

Aquí hay un efecto contrario al que uno esperaría. Reducir `h` (el paso de tiempo) normalmente hace que la aproximación de la derivada sea más exacta, pero si los datos tienen ruido, **el ruido se amplifica más**.

La razón es que al calcular la derivada se divide el cambio de posición entre `h`. El ruido en las posiciones es siempre del mismo tamaño, pero si `h` es muy pequeño, esa división lo vuelve enorme. Entonces:

- Con `h` grande, el método es menos preciso pero el ruido molesta poco.
- Con `h` pequeño, el método es más preciso en teoría, pero el ruido domina y la gráfica se ve muy ruidosa.

Por eso existe un punto intermedio: no conviene un `h` demasiado grande ni demasiado pequeño.

### 4. ¿Por qué ω requiere especial cuidado con el desenvolvimiento de θ?

La función `atan2` entrega el ángulo siempre entre **−π y π**. Cuando el robot gira y el ángulo pasa de π a −π, el número "da un salto" de unos 6.28 (2π) aunque físicamente el robot solo siguió girando de forma suave.

Si no se corrige ese salto antes de derivar, el cálculo de ω interpreta que el robot giró una vuelta completa en un instante, y aparecen **picos enormes y falsos** en la gráfica de velocidad angular. El desenvolvimiento (`unwrap`) arregla esto: suma o resta 2π donde hace falta para que el ángulo sea continuo. Por eso hay que hacerlo **antes** de calcular ω, nunca después.

---

## 9. Conclusiones

### Conclusión 1: sobre el método

Las diferencias centrales funcionan bien para estimar velocidades a partir de datos de posición, porque usan el punto anterior y el siguiente y dan un resultado más equilibrado que mirar solo hacia un lado. Sin embargo, tienen límites: en el primer y último punto hay que usar una versión más simple, y cada vez que se deriva se amplifica el ruido. Por eso ω es la cantidad más delicada, ya que depende de dos derivadas seguidas y de que el ángulo θ esté bien desenvuelto. Con datos reales y ruidosos, sería conveniente suavizar los datos antes de derivar o usar un paso de tiempo adecuado, ni muy grande ni muy pequeño.

### Conclusión 2: sobre el entorno computacional

Los tres entornos llegan a los mismos resultados, pero se diferencian en comodidad y control. **Python (NumPy)** es el más corto y rápido de escribir, porque una sola instrucción hace el trabajo de todo un ciclo. **Octave** es muy cercano a las matemáticas y sirve para ver cada paso del cálculo con claridad, aunque es más lento con ciclos largos. **C/C++** exige escribir más código, incluso para cosas simples como leer el archivo o desenvolver el ángulo, pero da control total sobre cada detalle y es el más rápido cuando hay muchísimos datos. En cuanto a GSL, su función de derivada no sirve directamente para datos en tabla, porque necesita una función que se pueda evaluar en cualquier punto. Para este trabajo, Python es la mejor opción por rapidez de desarrollo, y C++ conviene cuando importa el rendimiento o se necesita control fino.

### Conclusiones del Ejercicio Masa-Resorte

**1. Validación Numérica del Método de Butcher**
Al comparar la gráfica que generé en Python[cite: 18] con la obtenida en GNU Octave[cite: 19], pude evidenciar que las trayectorias calculadas son idénticas. Esto me demuestra que la implementación iterativa manual del método de Runge-Kutta de quinto orden (Butcher) es computacionalmente consistente y robusta, sin importar el lenguaje de programación que utilicemos[cite: 18, 19]. Además, el método logró resolver el sistema sin presentar inestabilidades numéricas con el tamaño de paso temporal ($h=0.1$) que elegí para los tres escenarios[cite: 18, 19].

**2. Análisis Físico del Sistema Masa-Resorte**
Ambas gráficas me permitieron validar a la perfección el comportamiento teórico de este sistema dinámico:
* **Caso Subamortiguado ($c=5$):** Pude observar claramente en la curva azul cómo el sistema sobrepasa la posición de equilibrio ($x=0$) múltiples veces, generando una oscilación cuya amplitud va decayendo con el tiempo[cite: 18, 19].
* **Caso Crítico ($c=40$):** La curva (naranja en Python, roja en Octave) representa el escenario ideal donde el sistema retorna a su posición de equilibrio en el menor tiempo posible, sin llegar a oscilar[cite: 18, 19].
* **Caso Sobreamortiguado ($c=200$):** Con la curva verde evidencié una alta resistencia al movimiento, disipando la energía tan rápido que el sistema retorna al origen de una forma sumamente lenta en comparación con el caso crítico[cite: 18, 19].

**3. Comparativa de las Herramientas Computacionales**
Durante el desarrollo del código, noté diferencias prácticas entre ambos entornos:
* **Python (Matplotlib):** La gráfica resultante tiene un diseño muy pulido y fácil de personalizar[cite: 18]. Me parece ideal para integrarlo en cuadernos interactivos como Jupyter (como hice en este caso) o para redactar informes académicos más visuales.
* **GNU Octave:** La gráfica que obtuve mantiene el estándar clásico de la ingeniería[cite: 19]. Sin embargo, la gran ventaja que le encontré a Octave es la naturalidad con la que me permite transcribir las ecuaciones y manejar los arreglos matriciales directamente en el código.

**Conclusión General**
La resolución de ecuaciones diferenciales ordinarias mediante métodos explícitos de alto orden, como el algoritmo de Butcher, me garantizó una alta precisión en la predicción del sistema. Como pude demostrar en las simulaciones, tanto Python como GNU Octave son herramientas idóneas para este propósito numérico, arrojando resultados matemáticamente equivalentes. La elección del entorno computacional depende más de las necesidades de visualización y del flujo de trabajo, siendo Python superior para la documentación interactiva y Octave excelente para un prototipado matricial rápido y directo.