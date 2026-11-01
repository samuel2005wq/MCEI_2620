### Pregunta 4. ¿Qué es CGAL, cómo está organizada y qué ventajas ofrece frente a una implementación propia?

#### 1. ¿Qué significa CGAL y cuál es el propósito de la librería?

**CGAL** significa *Computational Geometry Algorithms Library* (Librería de Algoritmos de Geometría Computacional). Es un proyecto de software libre y de código abierto (iniciado originalmente por un consorcio de universidades y centros de investigación europeos como INRIA, ETH Zürich y el Instituto Max Planck) cuyo propósito principal es hacer que la gran cantidad de algoritmos y estructuras de datos desarrollados en la literatura académica de geometría computacional estén disponibles para la industria y la academia en una librería fácil de usar. 

La idea central de CGAL no es solo dar fórmulas geométricas, sino ofrecer implementaciones que cumplan con tres pilares al mismo tiempo: **eficiencia** (baja complejidad asintótica), **flexibilidad** (adaptable a los tipos de datos del usuario) y, sobre todo, **robustez numérica** garantizada (que los algoritmos no fallen ni se cuelguen por errores de punto flotante o casos degenerados).

---

#### 2. ¿Por qué está implementada principalmente en C++?

CGAL está escrita en **C++** porque se apoya fuertemente en el paradigma de **programación genérica** (*Generic Programming*) mediante el uso de plantillas (*templates*), muy similar a como funciona la librería estándar (STL). Las razones técnicas de esta elección son:

* **Abstracción sin costo en tiempo de ejecución (*Zero-overhead abstraction*):** A diferencia de la orientación a objetos clásica con funciones virtuales (polimorfismo dinámico) que mete sobrecarga en cada llamada, las plantillas de C++ se resuelven en **tiempo de compilación** (polimorfismo estático). Esto permite que el compilador haga *inlining* de operaciones pequeñitas (como evaluar un predicado millones de veces) logrando velocidades de código ensamblador nativo.
* **Desacoplamiento entre algoritmos y tipos numéricos:** Gracias a los *templates* y a las clases de rasgos (*traits classes*), un mismo algoritmo en CGAL (por ejemplo, una Triangulación de Delaunay) se escribe una sola vez, pero se puede instanciar con coordenadas en `double`, números racionales exactos (`GMP`), o incluso con puntos propios del usuario, sin tocar el código del algoritmo.
* **Control directo de memoria y rendimiento:** En problemas de ingeniería donde se procesan mallas con millones de vértices o nubes de puntos LiDAR masivas, C++ permite manejar contenedores compactos en memoria y aprovechar al máximo la caché del procesador.

---

#### 3. ¿Qué es un Geometry Kernel y qué funciones desempeña?

En la arquitectura de CGAL, el **Geometry Kernel** (núcleo geométrico) es la capa base sobre la que se montan todos los algoritmos y estructuras de datos de alto nivel. Se puede entender como un bloque intercambiable que define dos cosas fundamentales:

1. **Las primitivas geométricas de dimensión constante:** Define los tipos de objetos básicos en 2D y 3D (`Point_2`, `Vector_2`, `Segment_2`, `Triangle_2`, `Line_2`, `Point_3`, `Plane_3`, `Tetrahedron_3`, etc.) y cómo se representan internamente sus coordenadas (por ejemplo, en coordenadas cartesianas o en coordenadas homogéneas, y con qué tipo numérico `FT` o `RT`).
2. **Las operaciones elementales sobre esas primitivas:** Contiene la implementación de todos los **predicados** (orientación, `in_circle`, comparaciones de coordenadas) y **construcciones** (intersecciones, circuncentros, distancias cuadradas) que los algoritmos necesitan llamar.

El papel del Kernel es aislar por completo la aritmética numérica del resto de la librería: el algoritmo de alto nivel nunca suma o multiplica coordenadas directamente, sino que le pide al Kernel que haga la operación o tome la decisión geométrica por él.

---

#### 4. ¿Qué son los predicates y las constructions?

Dentro del Kernel, todas las operaciones geométricas elementales se dividen estrictamente en estas dos categorías:

* **Predicados (*Predicates*):** Son funciones u objetos función (*functors*) que evalúan una propiedad o relación espacial entre primitivas y devuelven un **resultado discreto** tomado de un conjunto finito de posibilidades (como un booleano `true`/`false`, o enumeraciones como `CGAL::LEFT_TURN`, `CGAL::RIGHT_TURN`, `CGAL::COLLINEAR`, `CGAL::ON_BOUNDED_SIDE`). Los predicados **no crean nuevos objetos geométricos**; son los que dictan el flujo de control (`if/else`) y la topología del algoritmo.
* **Construcciones (*Constructions*):** Son operaciones que **producen nuevos objetos geométricos** o valores escalares continuos a partir de los datos de entrada (por ejemplo, construir el punto de intersección entre dos segmentos, hallar la recta mediatriz entre dos puntos o calcular el centro de una esfera circunscrita). A diferencia de los predicados, las construcciones generan nuevas coordenadas numéricas que requieren memoria y pueden acumular error de redondeo si el tipo numérico no es exacto.

---

#### 5. ¿Qué diferencias existen entre `Exact_predicates_inexact_constructions_kernel` y `Exact_predicates_exact_constructions_kernel`?

CGAL ofrece estos dos *kernels* predefinidos (`typedefs`) para que el programador elija el balance ideal entre velocidad y exactitud según lo que haga su algoritmo:

* **`Exact_predicates_inexact_constructions_kernel` (conocido como `EPICK`):**
  * **Cómo funciona:** Guarda las coordenadas de los puntos directamente como `double` estándar (64 bits). Cuando evalúa un **predicado**, usa filtros de aritmética de intervalos y, si hay duda en el signo, pasa a aritmética multiprecisión para dar una respuesta **100% exacta**. En cambio, cuando hace una **construcción** (como calcular un circuncentro), opera directamente con `double` normal, por lo que el nuevo punto puede tener un ligero error de redondeo.
  * **Cuándo se usa:** Es el kernel por defecto y el más rápido para todos los algoritmos que solo dependen de los puntos de entrada originales y no realimentan puntos construidos en nuevos predicados (por ejemplo: Envolvente Convexa, Triangulaciones de Delaunay 2D/3D, generación de mallas y procesamiento de nubes de puntos).

* **`Exact_predicates_exact_constructions_kernel` (conocido como `EPECK`):**
  * **Cómo funciona:** Garantiza exactitud matemática total tanto en las decisiones (**predicados**) como en las coordenadas de los nuevos objetos creados (**construcciones**). Para que no sea lento todo el tiempo, utiliza evaluación perezosa (*lazy evaluation*): guarda las construcciones como un árbol o grafo dirigido acíclico (DAG) de operaciones con intervalos flotantes, y solo calcula el valor racional exacto de precisión arbitraria (usando librerías como `GMP` o `CORE`) si un predicado posterior realmente necesita desempatar un caso muy cerrado.
  * **Cuándo se usa:** Es obligatorio en algoritmos donde los objetos construidos en un paso sirven de entrada para decisiones topológicas en el paso siguiente (por ejemplo: operaciones booleanas de polígonos o poliedros de Nef, arreglos de curvas 2D e intersecciones encadenadas). Es más pesado en memoria y tiempo que `EPICK`, pero evita que los errores acumulados rompan el algoritmo.

---

#### 6. ¿Qué ventajas y limitaciones tiene utilizar CGAL frente a implementar directamente los algoritmos?

* **Ventajas de usar CGAL:**
  1. **Robustez a prueba de balas:** Maneja automáticamente casos degenerados (puntos colineales, cocirculares, duplicados) y problemas de precisión de punto flotante que en una implementación casera con `double` tomarían meses depurar y casi siempre fallarían en casos borde.
  2. **Algoritmos en el estado del arte:** Ya trae implementados métodos con complejidades asintóticas óptimas y estructuras de datos avanzadas (como *Halfedge*, *DCEL* o *AABB Trees*) probadas en aplicaciones industriales.
  3. **Ahorro de tiempo de desarrollo:** Permite pasar directo a resolver el problema de ingeniería ensamblando paquetes compatibles entre sí en vez de programar desde cero estructuras topológicas complejas en 3D.

* **Limitaciones y desventajas:**
  1. **Curva de aprendizaje alta:** El uso intensivo de plantillas anidadas (*templates*), *traits*, *iterators*, *circulators* y conceptos de C++ hace que la sintaxis sea intimidante al principio y que los errores de compilación sean bastante largos de leer.
  2. **Tiempos de compilación y dependencias:** Como casi toda la librería es *header-only* (solo cabeceras con plantillas), compilar proyectos grandes toma más tiempo y requiere instalar dependencias externas como `Boost`, `GMP` y `MPFR`.
  3. **Licenciamiento en software comercial:** Aunque gran parte del núcleo y paquetes básicos tienen licencia permisiva (LGPL), muchos paquetes avanzados (como triangulaciones, mallas 3D u operaciones booleanas) están bajo licencia **GPLv3+**, lo que obliga a liberar el código fuente si se distribuye un software comercial o a comprar una licencia comercial de GeometryFactory.

---

#### Ejercicio de contextualización: Exploración de 5 paquetes de CGAL

Revisando el catálogo oficial (*CGAL Package Overview*), a continuación se identifican cinco paquetes clave para distintos problemas de ingeniería:

1. **Paquete: *2D Triangulations* (`Triangulation_2`)**
   * **Problema que resuelve:** Construir, modificar y consultar triangulaciones en el plano a partir de conjuntos de puntos o segmentos de restricción, manteniendo siempre la validez topológica y geométrica de la malla.
   * **Principales tipos de datos y algoritmos:** Ofrece las clases `CGAL::Triangulation_2`, `CGAL::Delaunay_triangulation_2`, `CGAL::Regular_triangulation_2` (para puntos con peso) y `CGAL::Constrained_Delaunay_triangulation_2` (que fuerza a que ciertos segmentos o bordes obligatorios aparezcan como aristas de la malla). Incluye algoritmos de inserción incremental, localización rápida de puntos y extracción del grafo dual (Voronoi).
   * **Posible aplicación en ingeniería:** En **Ingeniería Civil y Topografía**, para construir Modelos Digitales de Terreno (MDT / redes TIN) usando `Constrained_Delaunay_triangulation_2`, donde los puntos son mediciones GPS de elevación y los segmentos restringidos son quiebres obligatorios del terreno como carreteras, ríos o muros de contención.

2. **Paquete: *3D Fast Intersection and Distance Computation* (`AABB_tree`)**
   * **Problema que resuelve:** Acelerar drásticamente las consultas de intersección (conocer si una línea o plano corta un objeto 3D) y el cálculo de distancias mínimas contra mallas grandes compuestas por miles o millones de primitivas (triángulos o segmentos).
   * **Principales tipos de datos y algoritmos:** La estructura principal es `CGAL::AABB_tree`, que organiza las primitivas en una jerarquía de cajas delimitadoras alineadas a los ejes (*Axis-Aligned Bounding Boxes*). Provee métodos directos como `do_intersect()`, `any_intersection()`, `all_intersections()`, `closest_point()` y `squared_distance()`.
   * **Posible aplicación en ingeniería:** En **Robótica y Simulación de Sensores**, para simular un sensor LiDAR o verificar colisiones de un brazo robótico: cada rayo láser se modela como un `Ray_3` y se lanza contra el `AABB_tree` del entorno para encontrar en microsegundos contra qué triángulo choca y a qué distancia está el obstáculo.

3. **Paquete: *3D Mesh Generation* (`Mesh_3`)**
   * **Problema que resuelve:** Generar mallas isotrópicas de alta calidad (tanto mallas de superficie hechas de triángulos como mallas volumétricas rellenas de tetraedros) a partir de dominios 3D definidos por superficies poliedrales, funciones implícitas o imágenes médicas segmentadas (vóxeles).
   * **Principales tipos de datos y algoritmos:** Utiliza la estructura `CGAL::Mesh_complex_3_in_triangulation_3` (`C3t3`) y funciones como `CGAL::make_mesh_3()` y `CGAL::refine_mesh_3()`. Implementa el algoritmo de refinamiento de Delaunay restringido (mueve y agrega puntos de Steiner para eliminar tetraedros deformes) junto con optimización variacional (suavizado de Lloyd y *sliver exudation*).
   * **Posible aplicación en ingeniería:** En **Ingeniería Mecánica y Biomédica (Simulación FEM/CFD)**, para tomar una pieza mecánica en formato STL o una tomografía 3D de un hueso y discretizar todo su volumen interior en tetraedros con ángulos bien condicionados para poder simular esfuerzos mecánicos o flujo sanguíneo sin que el solucionador numérico diverja.

4. **Paquete: *Polygon Mesh Processing* (`PMP`)**
   * **Problema que resuelve:** Procesar, reparar, medir y operar sobre mallas poligonales cerradas o abiertas en 3D que suelen venir con defectos desde escáneres 3D o software CAD.
   * **Principales tipos de datos y algoritmos:** Trabaja sobre contenedores como `CGAL::Surface_mesh` o `CGAL::Polyhedron_3`. Ofrece algoritmos para **operaciones booleanas** (`corefine_and_compute_union/intersection/difference`), rellenado de huecos (`triangulate_hole`), remallado isotrópico (`isotropic_remeshing`), cálculo de áreas/volúmenes y reparación de mallas (*stitching* de bordes sueltos y orientación de normales).
   * **Posible aplicación en ingeniería:** En **Manufactura Aditiva (Impresión 3D) y Diseño CAD**, para preparar modelos antes de imprimirlos: permite cerrar huecos automáticamente para que la malla sea estanca (*watertight*), verificar que no tenga auto-intersecciones y realizar cortes o uniones booleanas entre piezas de un ensamblaje.

5. **Paquete: *Point Set Processing* (`Point_set_processing_3`)**
   * **Problema que resuelve:** Limpiar, filtrar, analizar y preprocesar nubes de puntos 3D crudas (*unorganized point clouds*) antes de pasarlas a algoritmos de reconstrucción de superficies.
   * **Principales tipos de datos y algoritmos:** Opera sobre `CGAL::Point_set_3` o vectores estándar de puntos. Incluye algoritmos para eliminación de ruido o puntos atípicos (`remove_outliers`), reducción y simplificación de densidad (`grid_simplify_point_set`, `hierarchy_simplify_point_set`), suavizado bilateral/jet (`jet_smooth_point_set`) y estimación/orientación de vectores normales a la superficie (`jet_estimate_normals`, `mst_orient_normals`).
   * **Posible aplicación en ingeniería:** En **Agricultura de Precisión y Visión Artificial**, cuando un dron o vehículo autónomo escanea un cultivo o una estructura con un sensor LiDAR/RGB-D: la nube de puntos llega con ruido por el polvo o las hojas, por lo que se usa este paquete para filtrar *outliers*, submuestrear los millones de puntos a un tamaño manejable y estimar las normales de la superficie del terreno o de las plantas.