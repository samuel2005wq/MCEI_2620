### Pregunta 5. ¿Cómo se puede utilizar CGAL para resolver problemas reales de ingeniería?

A continuación se analizan cinco campos de ingeniería de los propuestos en el taller, conectando el problema físico con las estructuras de datos y paquetes específicos del catálogo de CGAL (*CGAL Package Overview*):

---

#### 1. Robótica (Planificación de trayectorias y detección de colisiones)

* **1. ¿Cuál es el problema de ingeniería?** Hacer que un robot móvil (como un AGV en una bodega o un dron) o un brazo manipulador se desplace desde una postura inicial hasta una meta esquivando obstáculos estáticos y dinámicos, asegurando que en ningún instante su chasis choque contra el entorno y que la ruta sea segura y suave.
* **2. ¿Qué información geométrica se necesita?** La geometría del robot (representada como un polígono 2D o una malla poliedral cerrada 3D), la geometría de los obstáculos del entorno (polígonos con huecos en mapas 2D o nubes de puntos/mallas 3D capturadas por sensores LiDAR), y las coordenadas $(x, y, \theta)$ de inicio y destino.
* **3. ¿Qué algoritmo de geometría computacional podría utilizarse?** 
  * **Suma de Minkowski** ($P \oplus (-R)$) para expandir los obstáculos según la forma del robot $R$ y reducir el robot a un punto en el espacio de configuraciones (*C-Space*).
  * **Diagramas de Voronoi de segmentos** o grafos de visibilidad para trazar rutas por el centro de los corredores libres (máxima holgura).
  * **Jerarquías de volúmenes envolventes (*AABB Trees*)** para pruebas rápidas de colisión e intersección a lo largo de la trayectoria.
* **4. ¿Qué paquete de CGAL permitiría abordar el problema?** `2D Minkowski Sums` (`Minkowski_sum_2`) para el cálculo exacto del espacio libre en 2D, `2D Segment Delaunay Graphs` (`Segment_Delaunay_graph_2`) para el esqueleto de Voronoi entre paredes poligonales, y `3D Fast Intersection and Distance Computation` (`AABB_tree`) para detección de colisiones y distancias mínimas en 3D.
* **5. ¿Qué dificultades computacionales y numéricas deberían considerarse?** La suma de Minkowski de polígonos no convexos genera contornos con auto-intersecciones y huecos internos que requieren operaciones booleanas exactas (kernel `EPECK`) para no dejar "grietas" falsas por donde el planificador crea que el robot cabe. Además, si el robot puede rotar en 3D, la complejidad del *C-Space* crece exponencialmente, por lo que en tiempo real toca combinar pruebas discretas de distancia con `AABB_tree` usando predicados exactos (`EPICK`).

---

#### 2. Diseño mecánico CAD (Operaciones booleanas y modelado de sólidos)

* **1. ¿Cuál es el problema de ingeniería?** Construir y modificar piezas mecánicas complejas combinando sólidos primitivos (abrir perforaciones para tornillos, hacer vaciados, chaflanes o verificar interferencias entre piezas de un ensamblaje) garantizando que el modelo resultante siga siendo un sólido válido, cerrado (*watertight*) y fabricable (por CNC o impresión 3D).
* **2. ¿Qué información geométrica se necesita?** Mallas superficiales cerradas y orientadas (*Boundary Representation* o B-Rep) compuestas por caras triangulares o poligonales en 3D (`Point_3`, índices de caras y orientación de normales hacia afuera) que representan los volúmenes de las piezas $A$ y $B$.
* **3. ¿Qué algoritmo de geometría computacional podría utilizarse?** **Correfinamiento de mallas (*Mesh Corefinement*)** seguido de **Operaciones Booleanas Regularizadas** (unión $A \cup^* B$, intersección $A \cap^* B$ y diferencia $A \setminus^* B$), o bien representación mediante Poliedros de Nef (*Nef Polyhedra*).
* **4. ¿Qué paquete de CGAL permitiría abordar el problema?** `Polygon Mesh Processing` (`PMP`, específicamente el módulo de *Corefinement and Boolean Operations* sobre `CGAL::Surface_mesh`), y `3D Boolean Operations on Nef Polyhedra` (`Nef_3`) cuando se necesita manejar geometrías no-variedad (*non-manifold*) o caras abiertas con exactitud algebraica total.
* **5. ¿Qué dificultades computacionales y numéricas deberían considerarse?** Este es uno de los problemas donde más fallan los softwares CAD comerciales: cuando dos piezas comparten exactamente una cara coplanar, tienen aristas tangentes o vértices que se tocan apenas por micrómetros (casos degenerados). Si las coordenadas de los nuevos vértices de intersección se redondean a `double` durante el corte, la topología se rompe y aparecen caras invertidas o huecos. Por eso, el algoritmo de correfinamiento de CGAL necesita obligatoriamente construcciones exactas (o evaluación perezosa con `EPECK`) sobre la línea de corte.

---

#### 3. Ingeniería civil (Generación de mallas y modelado de terrenos)

* **1. ¿Cuál es el problema de ingeniería?** A partir de miles de puntos de elevación tomados en campo (con estación total, GPS diferencial o dron LiDAR), reconstruir la superficie continua de un terreno para calcular volúmenes de corte y relleno en movimiento de tierras, diseñar el trazado de una vía o simular escorrentía e inundaciones respetando bordes físicos obligatorios (ríos, bordillos, muros o taludes).
* **2. ¿Qué información geométrica se necesita?** Una nube de puntos con coordenadas $(x, y, z)$ donde $(x, y)$ es la posición geográfica y $z$ es la cota o altura, junto con una lista de polilíneas o segmentos 2D/3D (*breaklines* o líneas de quiebre) que representan fronteras donde la pendiente cambia bruscamente.
* **3. ¿Qué algoritmo de geometría computacional podría utilizarse?** **Triangulación de Delaunay 2.5D (Terrenos)** y **Triangulación de Delaunay Restringida (*Constrained Delaunay Triangulation* - CDT)** proyectada sobre el plano $XY$, complementada con refinamiento de mallas 2D de Ruppert o Chew para análisis hidráulico.
* **4. ¿Qué paquete de CGAL permitiría abordar el problema?** `2D Triangulations` (`Triangulation_2`) combinando `CGAL::Constrained_Delaunay_triangulation_2` con la clase de rasgos `CGAL::Projection_traits_xy_3` (que permite triangular en el plano $XY$ usando directamente puntos `Point_3` sin perder su coordenada $z$), y `2D Conforming Triangulations and Meshes` (`Mesh_2`).
* **5. ¿Qué dificultades computacionales y numéricas deberían considerarse?** En topografía es muy común que los datos vengan en coordenadas UTM con números gigantescos (por ejemplo, $X \approx 500\,000\text{ m}$ y $Y \approx 4\,600\,000\text{ m}$) pero con variaciones locales de pocos centímetros; al elevar al cuadrado en un determinante en punto flotante normal se pierden casi todos los decimales significativos. Además, las líneas de quiebre ingresadas por distintos levantamientos suelen cruzarse entre sí, lo que exige usar `Exact_predicates_tag` o `Exact_intersections_tag` en la CDT de CGAL para subdividir automáticamente los segmentos que se chocan sin que el programa aborte.

---

#### 4. Visión artificial y Agricultura de precisión (Reconstrucción 3D y procesamiento de nubes de puntos)

* **1. ¿Cuál es el problema de ingeniería?** Digitalizar objetos reales o parcelas agrícolas a partir de escaneos 3D (cámaras RGB-D, fotogrametría con drones o LiDAR terrestre) para medir automáticamente el volumen de la copa de los árboles, detectar defectos dimensionales en piezas industriales o reconstruir el gemelo digital de una escena.
* **2. ¿Qué información geométrica se necesita?** Una nube de puntos 3D desorganizada $P = \{p_1, \dots, p_n\} \subset \mathbb{R}^3$ (a menudo con millones de puntos, ruido de medición, densidad variable y zonas ocluidas), opcionalmente acompañada de vectores normales estimados en cada punto.
* **3. ¿Qué algoritmo de geometría computacional podría utilizarse?** Filtrado estadístico de *outliers*, estimación de normales por ajuste polinomial local (*Jet Fitting*) y orientación por Árbol de Expansión Mínima (MST), seguido de algoritmos de reconstrucción como **Reconstrucción de Superficies de Poisson**, **Advancing Front** o **Alpha Shapes 3D**.
* **4. ¿Qué paquete de CGAL permitiría abordar el problema?** `Point Set Processing` (`Point_set_processing_3`) para la limpieza y cálculo de normales, `Poisson Surface Reconstruction` (`Poisson_surface_reconstruction_3`) para superficies suaves cerradas, `Advancing Front Surface Reconstruction` para conectar directamente los puntos medidos, o `3D Alpha Shapes` (`Alpha_shapes_3`) para envolver follaje o estructuras con cavidades.
* **5. ¿Qué dificultades computacionales y numéricas deberían considerarse?** Los sensores reales meten ruido gaussiano, puntos fantasma (*outliers* por polvo o reflejos solares) y dejan huecos grandes donde el láser no alcanzó a ver (oclusión). Si se aplica una triangulación directa sobre puntos ruidosos, la superficie queda llena de picos irregulares; y si el muestreo es muy denso, el consumo de memoria RAM y el tiempo de cómputo explotan si no se hace primero una simplificación espacial jerárquica o por grilla.

---

#### 5. Ingeniería biomédica (Reconstrucción y procesamiento de estructuras anatómicas)

* **1. ¿Cuál es el problema de ingeniería?** Convertir imágenes médicas volumétricas (como una Tomografía Computarizada CT o una Resonancia Magnética MRI de un paciente) en modelos geométricos 3D precisos de órganos, huesos o vasos sanguíneos, ya sea para imprimir prótesis personalizadas o para correr simulaciones biomecánicas de Elementos Finitos (FEM).
* **2. ¿Qué información geométrica se necesita?** Un arreglo tridimensional de vóxeles (imágenes 3D en escala de grises o segmentadas por etiquetas donde cada número indica un tejido distinto: hueso cortical, tejido blando, arteria) o un conjunto de contornos 2D apilados corte por corte.
* **3. ¿Qué algoritmo de geometría computacional podría utilizarse?** Extracción de isosuperficies y **Refinamiento de Delaunay Restringido 3D** con bolas protectoras para bordes agudos, seguido de optimización de malla (suavizado de Lloyd, *ODT* y *perturbation/exudation*) para eliminar tetraedros aplastados (*slivers*).
* **4. ¿Qué paquete de CGAL permitiría abordar el problema?** `3D Mesh Generation` (`Mesh_3`, utilizando `CGAL::Labeled_mesh_domain_3` para leer directamente imágenes 3D segmentadas) y `3D Surface Mesh Generation` (`Surface_mesher`).
* **5. ¿Qué dificultades computacionales y numéricas deberían considerarse?** Los vóxeles de una tomografía son cubitos discretos, por lo que una extracción ingenua produce un efecto de "escalera" (*aliasing*) que genera concentraciones de esfuerzo irreales en una simulación FEM. Además, en3D la propiedad de Delaunay por sí sola no garantiza que no aparezcan **tetraedros *sliver*** (cuatro vértices casi coplanares bien espaciados en un círculo cuyo tetraedro tiene volumen casi cero), los cuales hacen que la matriz de rigidez del simulador biomecánico sea singular o mal condicionada.

---

#### Reto de integración: Sistema de estimación volumétrica de acopios y detección de obstáculos en agricultura/minería con Dron LiDAR

Para integrar los conceptos anteriores, se propone resolver un problema real que combina **Agricultura de Precisión / Ingeniería Civil y Visión Artificial**: el cálculo automatizado del volumen de material almacenado a cielo abierto (como silos de grano, pilas de compostaje o acopios de agregados) y la delimitación de su huella de seguridad a partir de un vuelo de dron.

##### 1. Descripción del problema
En plantas agroindustriales y obras civiles se necesita medir periódicamente cuántas toneladas o metros cúbicos de material hay en pilas irregulares sobre el terreno sin detener la operación. Un dron equipado con un sensor LiDAR o cámara fotogramétrica sobrevuela la zona y entrega una nube de puntos cruda. El objetivo del sistema en C++ con CGAL es limpiar la nube, aislar la huella del acopio, reconstruir la superficie topográfica y calcular con exactitud su volumen y área superficial.

##### 2. Entradas del sistema (*Inputs*)
* **Nube de puntos 3D cruda:** Un archivo `.xyz` o `.ply` con $N \in [50\,000, 1\,000\,000]$ puntos $P = \{(x_i, y_i, z_i)\}$ en coordenadas locales (en metros), que contiene puntos del suelo plano, puntos sobre la pila de material y ruido ambiental (polvo, aves o vegetación cercana).
* **Parámetros de filtrado y plano base:** Cota promedio del suelo de referencia $z_{\text{base}}$ (o puntos de control del perímetro) y umbral de vecindad $k$ para filtrado de ruido.

##### 3. Algoritmo y flujo de procesamiento en CGAL
El pipeline propuesto encadena cuatro etapas utilizando el kernel `CGAL::Exact_predicates_inexact_constructions_kernel` (`EPICK`):

1. **Preprocesamiento y limpieza (`Point_set_processing_3`):**
   * Se ejecuta `CGAL::remove_outliers()` calculando la distancia media a los $k = 24$ vecinos más cercanos (usando internamente un *k-d tree* de `Spatial_searching`) para descartar el porcentaje de puntos sueltos en el aire provocados por polvo.
   * Se aplica `CGAL::grid_simplify_point_set()` con una celda de tamaño $\delta$ (por ejemplo, $5\text{ cm}$) para homogeneizar la densidad de muestreo sin perder la forma de la pila.
2. **Delimitación del perímetro de seguridad en 2D (`Convex_hull_2` / `Alpha_shapes_2`):**
   * Se proyectan los puntos pertenecientes al montículo ($z_i > z_{\text{base}} + \Delta z$) sobre el plano horizontal $XY$.
   * Se calcula la **Envolvente Convexa 2D** (`CGAL::convex_hull_2`) para obtener el polígono convexo mínimo de cerramiento para maquinaria, y opcionalmente una **Alpha Shape 2D** para ajustar el borde exacto si la base de la pila tiene entrantes cóncavos.
3. **Reconstrucción de la superficie topográfica 2.5D (`Triangulation_2` con `Projection_traits_xy_3`):**
   * Se insertan los puntos 3D filtrados en una `CGAL::Delaunay_triangulation_2<Projection_traits_xy_3<EPICK>>`. Esto construye una malla triangular (TIN) óptima en $O(n \log n)$ donde cada cara finita $f$ es un triángulo en el espacio 3D con vértices $V_1(x_1, y_1, z_1)$, $V_2(x_2, y_2, z_2)$ y $V_3(x_3, y_3, z_3)$.
4. **Integración numérica de volumen y área (`Kernel_23`):**
   * Recorriendo todas las caras finitas de la triangulación (`finite_faces_iterator`), cada triángulo 3D define un prisma truncado vertical respecto al suelo $z_{\text{base}}$. El volumen total $V_{\text{total}}$ se calcula sumando el área proyectada en 2D de cada triángulo multiplicada por el promedio de las alturas de sus tres vértices:
     $$
     V_{\text{total}} = \sum_{f \in \text{Caras}} A_{xy}(f) \cdot \left( \frac{(z_1 - z_{\text{base}}) + (z_2 - z_{\text{base}}) + (z_3 - z_{\text{base}})}{3} \right)
     $$

##### 4. Resultados esperados (*Outputs*)
* **Polígono 2D de la huella perimetral:** Lista ordenada de vértices de la envolvente que delimita la zona ocupada en planta.
* **Malla superficial triangulada (TIN):** Archivo exportado en formato `.off` o `.obj` con los vértices, aristas y caras de Delaunay de la pila reconstruida para visualización 3D.
* **Reporte geométrico cuantitativo:** Volumen total acumulado ($m^3$), área proyectada en planta ($m^2$) y área superficial real 3D ($m^2$).

##### 5. Métricas para evaluar la solución
* **Exactitud volumétrica (Error relativo porcentual):** Validando el algoritmo primero con geometrías sintéticas de volumen analítico conocido (por ejemplo, una semiesfera o un cono muestreado con ruido gaussiano artificial) mediante:
  $$
  E_{\text{vol}} = \frac{\vert{}V_{\text{CGAL}} - V_{\text{real}}\vert{}}{V_{\text{real}}} \times 100\%
  $$
* **Calidad geométrica de la malla:** Relación de aspecto (*aspect ratio*) y distribución de ángulos mínimos de los triángulos generados para verificar que no existan triángulos degenerados en los bordes.
* **Desempeño computacional y escalabilidad:** Tiempo de ejecución en milisegundos (desglosado en tiempo de filtrado, tiempo de triangulación y tiempo de integración) y consumo pico de memoria RAM al escalar la nube desde $10^4$ hasta $10^6$ puntos.