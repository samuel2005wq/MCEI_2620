# Respuestas a las Preguntas Exploratorias
## P1: ¿Qué diferencias conceptuales encuentras entre un método basado en distancias como KNN y una estructura espacial como el kd-tree?

R: KNN es un algoritmo de aprendizaje automático para resolver un problema de clasificación o regresión, mientras que un kd-tree es una estructura de datos jerárquica diseñada para optimizar búsquedas espaciales.

Un kd-tree se utiliza para acelerar la búsqueda de vecinos, divide el espacio recursivamente con hiperplanos alineados a los ejes (alternando la variable de corte: voltaje, corriente, temperatura, potencia, etc).

## P2: ¿En qué escenarios prácticos crees que KNN puede ser más eficiente que un método basado en kd-tree, y viceversa?

R:
KNN de fuerza bruta es mejor cuando:

- El dataset es pequeño (como es en el caso de los 100 sensores del ejercicio de aplicación): construir el árbol cuesta más de lo que ahorra. La dimensionalidad es alta (más de ~15-20 variables). Se hace una sola consulta o pocas, o los datos cambian constantemente (reconstruir el árbol es costoso).

- También es la opción ideal si se hacen pocas consultas, si necesitas aprovechar el paralelismo de hardware (GPUs), o si los datos cambian constantemente (como apps de transporte o e-commerce), ya que reestructurar un árbol continuamente es computacionalmente muy costoso.

El K-d Tree es mejor cuando:

- Tienes un gran volumen de datos (miles o millones de registros, como 500,000 medidores inteligentes) concentrados en pocas dimensiones (menos de 10-20 variables, como voltaje, corriente y temperatura o coordenadas GPS en mapas 2D/3D).

- Es perfecto para sistemas con datos relativamente estáticos y lecturas masivas (sistemas de información geográfica, robótica con LIDAR o videojuegos).

## P3: ¿Qué impacto tienen la dimensionalidad y el tamaño de los datos en el desempeño de estos métodos?

R:

el impacto de la dimensionalidad (Número de variables o características) :

Cuando la dimensionalidad aumenta de forma desmedida en comparación con el número de muestras, se produce el fenómeno conocido como "La maldición de la dimensionalidad" (Curse of Dimensionality).

En espacios de alta dimensión, las distancias entre puntos tienden a **concentrarse**: el vecino más cercano y el más lejano quedan a distancias casi iguales, y el concepto de “cercanía” pierde significado. por eso métodos basados en distancias  como KNN, K-Means o SVM con ciertos kernels pierden efectividad

Dispersión de los datos: A mayor número de dimensiones, el volumen del espacio crece exponencialmente. Los datos se vuelven extremadamente dispersos, lo que significa que los puntos de datos parecen estar muy lejos unos de otros causando que estructuras como un kd-tree se degraden,haciendo que este se comporte como fuerza bruta.

El Impacto del Tamaño de los Datos (Número de registros o muestras):
Fuerza bruta: la consulta crece linealmente, O(n·d). kd-tree: crece aproximadamente de forma logarítmica en dimensiones bajas, O(log n).

Datos Insuficientes (Muestras pequeñas): Generan una alta varianza en el rendimiento del modelo.

- Modelos complejos (como redes neuronales profundas) fallan por completo o se sobre ajustan de inmediato.

- Grandes Volúmenes de Datos (Big Data):

- Permiten reducir el sesgo y mejorar la precisión de generalización.

- Cuello de botella: El impacto negativo es puramente computacional. El almacenamiento en memoria, la lectura de discos y los tiempos de entrenamiento se disparan,necesitando un  hardware más especializado (GPUs/TPUs).

## P4: ¿Cómo influye la elección de la métrica de distancia (euclidiana, Manhattan, etc.) en los resultados de KNN?

R:

La métrica define qué significa “cercano”, así que cambia los vecinos y los resultados:

| **Métrica** | **Fórmula** | **Geometría del Vecindario** | **Fortaleza Clave** | **Cuándo Usarla** |
| --- | --- | --- | --- | --- |
| **Euclidiana (****L****2****)** | $\sqrt{ Σ{(xi-yi)^2}}$ | Fronteras circulares / esféricas | Intuitiva y estándar | Datos continuos, homogéneos y sin *outliers*. |
| **Manhattan (****L****1****)** | Σ∥xi-yi∥ | Fronteras en forma de diamante / cuadrículas | Robusta ante *outliers* | Datos de alta dimensionalidad o de tipo rejilla. |
| **Chebyshev (****L****∞****)** | max∥xi-yi∥ | Fronteras cuadradas / hipercubos | Enfocada en el "peor caso" | Problemas de logística, movimiento de almacenes o ajedrez. |
| **Minkowski** | (Σ{∥xi-yi∥^p})^(1/p) | Variable según el parámetro p | Altamente adaptable | Ajustable mediante optimización de hiperparámetros (p). |
| **Mahalanobis** | $\sqrt{(x-y)^T S^(-1) (x-y)}$ | Fronteras elípticas | Ignora correlación y escala | Variables interdependientes (ej. voltaje y corriente). |

## P5: ¿Qué limitaciones prácticas podrían surgir al implementar estos métodos en sistemas de ingeniería eléctrica de gran escala?

R:

Al implementar KNN y kd-tree en sistemas de ingeniería eléctrica de gran escala surgen varias limitaciones prácticas:

- el costo computacional y de memoria: el KNN de fuerza bruta debe comparar cada consulta contra todos los registros, lo que genera un cuello de botella en tiempo real cuando se manejan millones de mediciones, como las de una red de medidores inteligentes.

- los datos llegan en flujo continuo y el kd-tree estándar maneja mal las inserciones y eliminaciones, pues el árbol se desbalancea y debe reconstruirse periódicamente.

- la alta dimensionalidad: los sistemas reales miden decenas de variables (armónicos, factor de potencia, frecuencia, vibración, etc.), y en ese escenario las distancias pierden capacidad discriminante y el kd-tree degrada su desempeño hasta comportarse casi como fuerza bruta. A esto se suma la sensibilidad a la escala y al ruido, ya que variables con magnitudes distintas (voltaje en cientos de voltios frente a corriente en pocos amperios) exigen normalización cuidadosa, y los valores atípicos o el ruido de medición pueden distorsionar la selección de vecinos. También influye la elección del parámetro k, pues un valor muy pequeño vuelve el método sensible al ruido y uno muy grande diluye los patrones locales; además, como las anomalías son escasas, sus vecinos pueden no representar bien el comportamiento anómalo.

- Finalmente, los requisitos de latencia en protección y control (respuestas en milisegundos) obligan a recurrir a hardware dedicado, búsquedas aproximadas o procesamiento distribuido en el borde (edge), y los cambios en las condiciones de operación (deriva de los datos) hacen necesario reindexar o reajustar el modelo periódicamente, lo que incrementa el costo de mantenimiento.
