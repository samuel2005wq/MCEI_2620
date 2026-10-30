#### 1. ¿Qué es la geometría computacional y cuáles son sus diferencias con la geometría analítica y la geometría euclidiana tradicional?

La **geometría computacional** (*Computational Geometry*) es el área de las ciencias de la computación que se encarga de estudiar y diseñar algoritmos y estructuras de datos para resolver problemas geométricos mediante un computador. La idea central acá no es solo saber qué fórmula matemática resuelve un problema en el papel, sino cómo programarlo para que sea rápido (baja complejidad computacional cuando tenemos miles o millones de datos) y que no falle por culpa de cómo los computadores guardan los números decimales.

Si la comparamos con las otras dos ramas clásicas, las diferencias se ven es en cómo abordan el problema:

* **Geometría Euclidiana tradicional:** Es la geometría clásica de axiomas, teoremas y construcciones ideales (como las que se hacen con la regla y el compás). Trabaja sobre figuras abstractas sin usar coordenadas numéricas y busca demostrar propiedades de forma lógica y deductiva.

* **Geometría Analítica:** Acá ya metemos un sistema de coordenadas (como el plano cartesiano $\mathbb{R}^2$ o el espacio $\mathbb{R}^3$) y representamos todo con ecuaciones algebraicas. El objetivo es resolver sistemas de ecuaciones para hallar una intersección o una distancia.

* **Geometría Computacional:** Toma la base analítica pero la lleva a la realidad del computador. En vez de despejar ecuaciones a mano para dos rectas, piensa en cómo procesar $N$ objetos usando estructuras de datos discretas, cuidando la complejidad asintótica (por ejemplo, bajar de $O(n^2)$ a $O(n \log n)$) y lidiando con la aritmética de punto flotante (IEEE-754) para que un error de redondeo no dañe el resultado.

---

#### 2. ¿Qué tipos de objetos geométricos se utilizan para representar problemas computacionales en dos y tres dimensiones?

Para poder trabajar estos problemas en código, las librerías como CGAL dividen los objetos en primitivas geométricas básicas, formas compuestas y estructuras de datos más armadas dependiendo de la dimensión:

* **En dos dimensiones (2D):** Se usan puntos (`Point_2`), vectores, rectas infinitas, rayos (semirrectas), segmentos de recta, triángulos, círculos y rectángulos o cajas alineadas a los ejes (*Bounding Boxes* o AABB). Cuando necesitamos representar formas más complejas, usamos cadenas poligonales, polígonos (simples o con huecos) y mallas planas (como las triangulaciones).
* **En tres dimensiones (3D):** Trabajamos con puntos (`Point_3`), planos, rectas y segmentos en el espacio, tetraedros (que vienen a ser como el equivalente del triángulo pero en 3D), esferas y cajas delimitadoras 3D (`Box_3`). Para objetos completos se usan mallas poliedrales (superficies hechas de triángulos o polígonos) y mallas volumétricas.

---

#### 3. ¿Qué son los puntos, segmentos, polígonos, poliedros, superficies y nubes de puntos desde la perspectiva computacional?

Estos objetos, desde la perspectiva de la programación, no son solo dibujos, sino estructuras en memoria donde toca separar muy bien la **geometría** (las coordenadas numéricas como tal) de la **topología** (quién está conectado con quién):

* **Puntos:** Son el elemento base sin dimensión. Computacionalmente se guardan como una tupla de coordenadas cartesianas $(x, y)$ o $(x, y, z)$, aunque a veces también se usan coordenadas homogéneas $(hx, hy, hw)$ para evitar hacer divisiones y no perder precisión.
* **Segmentos:** Son líneas acotadas definidas por un par ordenado de puntos $(p_1, p_2)$, que son su inicio y su fin. Ese orden permite darles una dirección u orientación en el espacio.
* **Polígonos:** Son regiones cerradas del plano formadas por una secuencia ordenada de segmentos (aristas) que se conectan en sus extremos (vértices) formando un ciclo. En el computador se suelen guardar como una lista o vector de vértices en orden horario o antihorario, cuidando que las aristas no se crucen entre sí (polígono simple).
* **Poliedros:** Son sólidos en 3D cerrados por caras planas. Computacionalmente no basta con guardar las caras sueltas, sino que se usan estructuras de datos como la *Halfedge Data Structure* (estructura de media arista) o listas doblemente conectadas (DCEL), que permiten saber rápidamente qué cara es vecina de cuál y cómo se conectan los vértices y las aristas.
* **Superficies:** Son capas bidimensionales metidas en un espacio 3D (como la cáscara de un objeto). Para que el computador las entienda y las procese, casi siempre se discretizan aproximándolas con mallas de triángulos (*Triangle Meshes*).
* **Nubes de puntos (*Point Clouds*):** Son simplemente un conjunto grande de puntos sueltos $P = \{p_1, p_2, \dots, p_n\}$ en 2D o 3D. Lo importante acá es que vienen "crudos", es decir, solo tenemos sus coordenadas pero no hay ninguna información de cómo se conectan entre sí ni qué forma tienen hasta que les aplicamos algún algoritmo encima.

---

#### 4. ¿Cuáles son cinco problemas clásicos de la geometría computacional?

1. **Envolvente Convexa (*Convex Hull*):** Dado un grupo de puntos regados en el plano o el espacio, encontrar el polígono o poliedro convexo más pequeño que los encierre a todos (como si les pusiéramos un caucho alrededor).
2. **Triangulación de Delaunay y Diagramas de Voronoi:** Dividir el espacio en regiones según a qué punto están más cerca (Voronoi) o conectar una nube de puntos formando triángulos que sean lo más "parejos" posibles, evitando triángulos muy flacos o alargados (Delaunay).
3. **Búsqueda de Vecinos Más Cercanos (*Nearest Neighbor Search*):** Organizar los puntos en estructuras de árboles espaciales (como los *k-d trees*) para que, cuando preguntemos cuál es el punto más cercano a una coordenada $q$, no toque revisar todos los puntos uno por uno.
4. **Detección de Intersecciones (*Intersection Detection*):** Revisar si un conjunto grande de segmentos, polígonos o cuerpos en 3D se chocan o se cruzan en alguna parte, usando algoritmos de barrido (*Sweep Line*) para hacerlo de forma eficiente.
5. **Localización de Puntos (*Point Location*):** Teniendo un mapa dividido en varias regiones o un polígono cerrado, determinar rápidamente si un punto de prueba cae adentro, afuera o exactamente en el borde (*Point-in-Polygon*).

---

#### 5. ¿En qué aplicaciones de ingeniería se utilizan estos algoritmos?

En ingeniería estos algoritmos están por debajo de casi cualquier software que maneje espacio o formas físicas: desde programas de diseño mecánico (CAD) y simulación por elementos finitos (FEM), hasta navegación de robots móviles, sistemas de información geográfica (SIG/GIS), gráficos por computador y procesamiento de imágenes o visión artificial.

---

#### Ejercicio de contextualización

A continuación se describen tres aplicaciones reales viendo qué entra y qué problema geométrico se resuelve por detrás:

1. **Robótica Móvil (Planificación de trayectorias y evasión de obstáculos):**
   * **Información de entrada:** Una nube de puntos 2D o 3D que entrega un sensor LiDAR o una cámara de profundidad mostrando dónde están las paredes y obstáculos, las dimensiones del robot (representado como un polígono) y las coordenadas de salida y llegada.
   * **Problema geométrico involucrado:** Primero hay que saber por dónde cabe el robot sin estrellarse; para eso se expanden los obstáculos usando la **Suma de Minkowski** con la forma del robot. Luego, se puede armar un **Diagrama de Voronoi** o un grafo de visibilidad entre los obstáculos para trazar un camino seguro que vaya por el medio de los pasillos, y se usa **detección de intersecciones** con cajas delimitadoras (AABB) para verificar que la trayectoria planeada no toque ningún objeto.

2. **Visión Artificial (Seguimiento de objetos y reconstrucción 3D):**
   * **Información de entrada:** Puntos clave (*keypoints*) o contornos 2D sacados al segmentar una imagen de cámara, o bien una nube de puntos 3D capturada con cámaras estéreo.
   * **Problema geométrico involucrado:** Cuando se hace detección o seguimiento en 2D (por ejemplo, detectar una mano o una pieza en una banda), se calcula la **Envolvente Convexa** (*Convex Hull*) de los puntos del contorno para encerrar el objeto y sacar métricas rápidas de su forma o orientación. Si estamos en 3D con una nube de puntos cruda, el problema es reconstruir la superficie del objeto usando una **Triangulación de Delaunay 3D** para conectar los puntos y armar una malla sólida.

3. **Diseño Mecánico CAD y Simulación (Modelado de piezas y mallado):**
   * **Información de entrada:** Los modelos geométricos de las piezas mecánicas representados por sus bordes y superficies (mallas poliedrales cerradas).
   * **Problema geométrico involucrado:** Al diseñar una pieza se hacen **operaciones booleanas** (unión, resta e intersección de poliedros) para abrir huecos o juntar partes. Además, cuando esa pieza se pasa a simulación (para ver cómo aguanta fuerzas o calor con elementos finitos), toca usar algoritmos de **Generación y Refinamiento de Mallas de Delaunay** para partir todo el sólido en miles de tetraedros bien formados, ya que si quedan triángulos o tetraedros muy aplastados la simulación numérica da errores.