### Pregunta 3. ¿Por qué la precisión numérica y la robustez son desafíos fundamentales en geometría computacional?

#### 1. ¿Qué problemas introduce la representación numérica IEEE-754 en los cálculos geométricos?

El estándar **IEEE-754** es la forma en que los procesadores representan números reales usando punto flotante (`float` de 32 bits o `double` de 64 bits). Aunque en cálculo numérico tradicional un pequeño error en el último decimal casi no importa, en geometría computacional introduce problemas críticos porque las decisiones topológicas dependen del signo exacto de una operación:

* **Discretización y truncamiento:** Como la mantisa tiene un número fijo de bits (53 bits en un `double`, que equivale a unos 15-17 dígitos decimales significativos), la recta real continua $\mathbb{R}$ se reemplaza por una rejilla discreta de números representables. Cualquier coordenada o resultado intermedio que caiga entre dos valores de esa rejilla se redondea al más cercano.
* **Cancelación catastrófica:** Ocurre cuando restamos dos números de magnitud muy similar. Los dígitos más significativos se cancelan entre sí y el resultado queda formado únicamente por los bits menos significativos, que son puro ruido de redondeo acumulado en operaciones previas.
* **Pérdida de propiedades algebraicas:** En punto flotante, la suma y la multiplicación dejan de ser asociativas; es decir, $(a + b) + c$ no siempre da exactamente lo mismo que $a + (b + c)$. Esto hace que evaluar una misma expresión geométrica en distinto orden pueda cambiar el signo del resultado.
* **Subdesbordamiento (*underflow*) y desbordamiento (*overflow*):** Al calcular determinantes (donde se multiplican coordenadas entre sí), los valores intermedios pueden superar el exponente máximo o hacerse tan pequeños que colapsan a cero.

---

#### 2. ¿Qué significa que tres puntos sean colineales y por qué determinarlo computacionalmente puede ser difícil?

Matemáticamente, tres puntos $A(x_A, y_A)$, $B(x_B, y_B)$ y $C(x_C, y_C)$ son **colineales** si caen exactamente sobre la misma línea recta. En geometría computacional, esto se verifica evaluando el **predicado de orientación** (el determinante de $2 \times 2$ o producto cruz de los vectores $\vec{AB}$ y $\vec{AC}$):

$$
\text{Orientación}(A, B, C) = \det \begin{pmatrix} x_B - x_A & y_B - y_A \\ x_C - x_A & y_C - y_A \end{pmatrix} = (x_B - x_A)(y_C - y_A) - (y_B - y_A)(x_C - x_A)
$$

Si el determinante es positivo ($> 0$), los puntos hacen un giro a la izquierda (sentido antihorario); si es negativo ($< 0$), hacen un giro a la derecha (sentido horario); y si es **exactamente igual a cero ($= 0$)**, los tres puntos son colineales.

Determinarlo computacionalmente es muy difícil por dos razones:
1. **El cero exacto se pierde por redondeo:** Al evaluar $(x_B - x_A)(y_C - y_A) - (y_B - y_A)(x_C - x_A)$ en punto flotante, estamos restando dos productos. Si los puntos son realmente colineales pero sus coordenadas no son potencias exactas de 2 (por ejemplo, múltiplos de $0.1$ o resultados de una rotación), los dos productos sufren pequeños errores de redondeo distintos y la resta da algo como $1.11 \times 10^{-16}$ en vez de $0$, haciendo creer al programa que hay una curva donde hay una recta.
2. **Usar una tolerancia fija ($\epsilon$) rompe la consistencia lógica:** Si intentamos arreglarlo diciendo que son colineales cuando $|\det| < \epsilon$, la relación de colinealidad deja de ser transitiva ($A, B, C$ pueden parecer colineales, $B, C, D$ también, pero $A, B, D$ no), lo que termina rompiendo las estructuras de datos del algoritmo.

---

#### 3. ¿Qué son las configuraciones degeneradas (*degenerate cases*)?

En la teoría de algoritmos geométricos, casi siempre se diseña bajo el supuesto de **posición general** (*general position*) para simplificar el análisis. Una **configuración degenerada** es cualquier disposición de los datos de entrada donde se viola esa posición general y ocurre una coincidencia geométrica exacta que reduce los grados de libertad del sistema.

Ejemplos clásicos de casos degenerados son:
* Dos o más puntos con exactamente las mismas coordenadas (puntos duplicados).
* Tres o más puntos colineales en 2D, o cuatro o más puntos coplanares en 3D.
* Cuatro o más puntos **cocirculares** en el plano (o cinco coesféricos en 3D), lo que hace que la Triangulación de Delaunay deje de ser única porque el círculo circunscrito pasa por cuatro puntos a la vez.
* Tres o más segmentos de recta que se cruzan exactamente en el mismo punto, o segmentos que se solapan parcialmente.

En la práctica de ingeniería, los casos degenerados no son raros sino todo lo contrario: los diseños en CAD, las grillas de sensores o los planos arquitectónicos están llenos de líneas paralelas, ángulos rectos y puntos alineados a propósito.

---

#### 4. ¿Cómo pueden los errores de redondeo alterar la detección de intersecciones, la orientación de puntos o la construcción de triangulaciones?

El problema de fondo en geometría computacional es que los algoritmos toman decisiones **combinatorias y topológicas** (quién se conecta con quién) basándose en cálculos **numéricos** (predicados). Si un error de redondeo cambia el signo de un predicado, no solo obtenemos un número un poquito corrido, sino que el algoritmo puede entrar en un estado lógicamente imposible:

* **En la orientación de puntos y Envolvente Convexa:** Si tres puntos casi alineados tienen un determinante que por error de redondeo cambia de $+10^{-17}$ a $-10^{-17}$, el algoritmo interpreta un giro a la izquierda como un giro a la derecha. Esto puede hacer que un algoritmo de *Convex Hull* descarte un punto que iba en el borde exterior, deje una abolladura cóncava en un polígono que debía ser convexo, o incluso elimine todos los vértices al dar la vuelta.
* **En la detección de intersecciones:** Para saber si el segmento $\overline{AB}$ cruza al segmento $\overline{CD}$, se verifica que $C$ y $D$ estén en lados opuestos de $\overline{AB}$ y viceversa. Un error de signo cerca de los extremos puede reportar falsos positivos (cruces que no existen) o falsos negativos (ignorar una colisión real en la ruta de un robot). Peor aún, en un algoritmo de barrido (*Sweep Line*), si el punto de intersección calculado queda por redondeo a la izquierda de la línea de barrido actual, la cola de eventos se desordena y el programa falla.
* **En la construcción de triangulaciones (Delaunay):** Durante la inserción incremental, el algoritmo decide si voltear una arista (*edge flip*) evaluando el predicado `in_circle`. Si por errores de punto flotante el predicado viola las propiedades geométricas entre dos triángulos vecinos $T_1$ y $T_2$, el programa puede creer simultáneamente que $T_1$ invade el círculo de $T_2$ y que la diagonal opuesta también es inválida. El resultado es que el algoritmo se queda atrapado en un **bucle infinito** volteando la misma arista una y otra vez, o termina corrompiendo la estructura de datos creando triángulos invertidos o superpuestos.

---

#### 5. ¿Qué diferencia existe entre exactitud geométrica, precisión numérica y robustez algorítmica?

Aunque suenen parecido, apuntan a tres niveles distintos del problema:

* **Precisión numérica:** Es una propiedad de la representación de los datos. Se refiere a la cantidad de bits o dígitos significativos con los que se guarda y opera un número (por ejemplo, pasar de `float` de 32 bits a `double` de 64 bits o `long double` de 80/128 bits aumenta la precisión numérica, pero sigue siendo aproximada).
* **Exactitud geométrica:** Es una propiedad de las decisiones del algoritmo. Significa que todos los predicados geométricos (orientación, `in_circle`, comparación de distancias) devuelven el resultado matemático **100% verdadero** ($+1$, $0$, o $-1$), sin importar qué tan cerca de cero estén los valores, garantizando que la topología construida sea exactamente la correcta.
* **Robustez algorítmica:** Es la capacidad del programa completo para no fallar (no colgarse, no caer en bucles infinitos ni corromper sus punteros o estructuras topológicas) ante **cualquier** entrada válida, incluyendo datos con ruido o configuraciones totalmente degeneradas, entregando siempre una salida consistente y bien formada.

---

#### Experimento conceptual y solución en CGAL

##### 1. Análisis de los puntos $A=(0,0)$, $B=(1,1)$ y $C=(2, 2+\epsilon)$

Queremos saber si los puntos $A$, $B$ y $C$ son colineales o hacia qué lado giran. Matemáticamente, si evaluamos el determinante de orientación en el mundo ideal de los números reales:

$$
\det \begin{pmatrix} x_B - x_A & y_B - y_A \\ x_C - x_A & y_C - y_A \end{pmatrix} = \det \begin{pmatrix} 1 - 0 & 1 - 0 \\ 2 - 0 & (2 + \epsilon) - 0 \end{pmatrix} = 1 \cdot (2 + \epsilon) - 1 \cdot 2 = \epsilon
$$

En teoría, para cualquier $\epsilon > 0$ (por diminuto que sea), el determinante vale $\epsilon > 0$, lo que significa que los tres puntos **no son colineales** y forman un giro estricto a la izquierda.

Sin embargo, veamos qué pasa dentro del computador cuando usamos variables `double` (IEEE-754) donde el épsilon de máquina es $\varepsilon_{\text{mach}} = 2^{-52} \approx 2.22 \times 10^{-16}$:

1. **Absorción al construir la coordenada ($2 + \epsilon$):** Como el número $2.0$ se representa con exponente $2^1$, el espacio entre $2.0$ y el siguiente número flotante representable es $2 \cdot \varepsilon_{\text{mach}} = 2^{-51} \approx 4.44 \times 10^{-16}$. Si escogemos un $\epsilon = 10^{-16}$, en el momento mismo de evaluar la suma `2.0 + eps` en memoria, el valor de $\epsilon$ se pierde por completo (`fl(2.0 + eps) == 2.0`). El punto $C$ se guarda como $(2, 2)$ y el programa clasifica erróneamente los puntos como **colineales** ($\det = 0$).
2. **Dependencia del orden de las operaciones:** Si cambiamos el punto de referencia para calcular el mismo determinante (por ejemplo, tomando vectores desde $B$ o desde $C$, o trasladando el sistema de coordenadas sumándole una constante grande $M = 10^6$ a todos los puntos), los errores de redondeo al restar coordenadas antes de multiplicar cambian. Así, evaluating `orientacion(A, B, C)` podría dar $> 0$ (giro a la izquierda), mientras que `orientacion(C, B, A)` o `orientacion(B, C, A)` podría dar $0$ (colineal) o incluso cambiar de signo si hay operaciones intermedias, violando la antisimetría básica del determinante.

##### 2. ¿Cómo aborda CGAL estos problemas? (*Predicates*, *Constructions* y Cálculo Exacto)

CGAL adopta el **Paradigma de Cómputo Geométrico Exacto (EGC - *Exact Geometric Computation*)**, que parte de distinguir dos tipos de operaciones muy diferentes dentro de cualquier algoritmo:

* **Predicados (*Predicates*):** Son funciones que **no crean objetos nuevos**, sino que evalúan una relación entre objetos existentes y devuelven un valor discreto de un conjunto finito (un booleano `true`/`false`, o un signo `LEFT_TURN`, `RIGHT_TURN`, `COLLINEAR`, o `ON_ORIENTED_BOUNDARY`). Ejemplos: `CGAL::orientation(p, q, r)` o `CGAL::side_of_oriented_circle(p, q, r, test)`.
* **Construcciones (*Constructions*):** Son funciones que **calculan y generan nuevos objetos geométricos** con nuevas coordenadas numéricas. Ejemplos: hallar el punto de intersección entre dos rectas, calcular el circuncentro de un triángulo (para un vértice de Voronoi) o el punto medio de un segmento.

**Diferencia entre predicados exactos y construcciones exactas en los Kernels:**
* Para no sacrificar velocidad usando aritmética multiprecisión lenta (como `GMP` o `MPFloat`) en todo momento, CGAL usa **aritmética de intervalos (filtros flotantes dinámicos)**. Primero calcula el predicado rápido con `double` llevando un registro de la cota máxima de error; si el resultado está lejos de cero, el signo es seguro y responde de inmediato. Solo si el valor cae muy cerca de cero (como en nuestro experimento con $\epsilon$ o en casos degenerados), CGAL pasa automáticamente a evaluar el determinante con **aritmética multiprecisión exacta** para garantizar que nunca se equivoque de signo.
* Muchos algoritmos (como la Envolvente Convexa o la Triangulación de Delaunay) solo toman decisiones usando los puntos originales de entrada y no necesitan construir puntos nuevos para armar su topología. Para esos casos se usa un kernel con **predicados exactos y construcciones inexactas** (`Exact_predicates_inexact_constructions_kernel` o `EPICK`), que es supremamente rápido y 100% robusto en su topología.
* En cambio, cuando un algoritmo construye puntos nuevos (por ejemplo, hallar la intersección de dos polígonos) y luego mete esos puntos recién creados dentro de otro predicado más adelante, el redondeo de la construcción dañaría los pasos siguientes. Para esos casos, se requiere un kernel que garantice **tanto predicados exactos como construcciones exactas** (`Exact_predicates_exact_constructions_kernel` o `EPECK`), el cual guarda un árbol de expresión (*lazy evaluation*) y usa números racionales exactos de precisión arbitraria cuando hace falta.