# Taller de geometría computacional con CGAL: Sección 3

Nube de puntos, envolvente convexa y triangulación de Delaunay en C++ con CGAL, con scripts para repetir los experimentos, medir tiempos y graficar los resultados.

## Contenido

| Archivo | Descripción |
|---|---|
| `taller.cpp` | Genera una nube de puntos, calcula la envolvente convexa y la triangulación de Delaunay, mide tiempos y verifica los resultados. |
| `experimentos_pregunta_6.cpp` | Experimentos con casos degenerados: puntos repetidos, colineales, cocirculares y casi colineales. |
| `run_experimentos.sh` | Compila todo y ejecuta los experimentos completos (4 tamaños × 3 semillas × 5 repeticiones). |
| `graficar.py` | Dibuja los puntos, la envolvente y las aristas de Delaunay de una corrida. |

## Requisitos

- Ubuntu (por ejemplo, bajo WSL) con compilador C++ (g++), CGAL, GMP y MPFR.
- Python 3 con matplotlib, solo para las figuras.

Instalación:

```bash
sudo apt update
sudo apt install -y build-essential libcgal-dev libgmp-dev libmpfr-dev python3 python3-matplotlib
```

Verifica la instalación:

```bash
g++ --version
dpkg -s libcgal-dev | grep Version
```

## Uso rápido

Coloca los cuatro archivos en una misma carpeta (por ejemplo `~/taller_cgal`) y ejecuta:

```bash
cd ~/taller_cgal
bash run_experimentos.sh
```

El script compila `taller.cpp` y `experimentos_pregunta_6.cpp` y corre todos los experimentos. Si ya existía una carpeta `resultados/` de una corrida anterior, bórrala antes con `rm -rf resultados`.

### Configuración de los experimentos

Dentro de `run_experimentos.sh` puedes cambiar:

```bash
SEMILLAS="7 314 1999"
TAMANOS="100 500 1000 5000"
REPS=5
```

## Resultados generados

```
resultados/
├── mediciones.csv                       # una fila por repetición (60 filas)
├── experimentos_pregunta_6.txt          # salida de los casos degenerados
├── n100_s7/                             # una carpeta por tamaño y semilla
│   ├── puntos.csv
│   ├── envolvente.csv
│   ├── aristas.csv
│   └── resumen.csv
├── n100_s314/
└── ...
```

### Columnas de `mediciones.csv`

| Columna | Significado |
|---|---|
| `n`, `semilla`, `repeticion` | Identifican la corrida. |
| `dimension` | Dimensión de la triangulación (2 en el caso normal). |
| `vertices_hull` | Vértices de la envolvente convexa. |
| `vertices` | Vértices distintos en la triangulación de Delaunay. |
| `aristas_finitas`, `caras_finitas` | Conteos de la triangulación. |
| `hull_ms`, `delaunay_ms` | Tiempos de ejecución en milisegundos. |
| `valida` | 1 si `is_valid()` de CGAL confirma la triangulación. |
| `euler` | 1 si se cumple V − E + F = 1 (sin contar la cara exterior). |
| `geometria_reproducible` | 1 si la repetición dio exactamente la misma geometría que la primera. |

Las repeticiones se hacen sobre la misma nube de puntos, así que las diferencias de tiempo entre ellas vienen del ruido del sistema y no de la geometría.

## Ejecutar un experimento individual

Compilación manual:

```bash
g++ -std=c++17 -O2 -DNDEBUG -frounding-math taller.cpp -o taller -lgmp -lmpfr
```

Ejecución:

```bash
./taller N SEMILLA [REPETICIONES] [CARPETA_SALIDA]
```

Ejemplos:

```bash
# Solo mediciones por pantalla (líneas que empiezan con MED,)
./taller 1000 42

# 5 repeticiones y guardando los CSV en una carpeta existente
mkdir -p prueba
./taller 1000 42 5 prueba
```

- Sin `CARPETA_SALIDA` solo se imprimen las líneas de medición.
- La carpeta de salida debe existir antes de ejecutar.

## Figuras

```bash
# Una corrida
python3 graficar.py resultados/n100_s7

# Todas las corridas
for d in resultados/n*_s*; do python3 graficar.py $d; done
```

Cada figura se guarda como `figura.png` dentro de su carpeta. La nube de 5000 puntos produce una figura muy cargada; para el informe conviene usar las de 100 y 1000 puntos.

## Casos degenerados

```bash
g++ -std=c++17 -O2 -DNDEBUG -frounding-math degenerados.cpp -o degenerados -lgmp -lmpfr
./degenerados
```

Casos que cubre:

1. Puntos repetidos.
2. Puntos colineales.
3. Puntos cocirculares (circunferencia de radio 5 y malla regular 10×10).
4. Tres puntos casi colineales: A = (0,0), B = (1,1), C = (2, 2+ε), comparando el determinante en `double` con la orientación de CGAL.
5. Nube casi colineal con ruido de 1e-9.

## Notas técnicas

- **Kernel**: se usa `Exact_predicates_inexact_constructions_kernel`. La envolvente y Delaunay solo toman decisiones con predicados (orientación e in-circle) sobre los puntos de entrada, por lo que las construcciones inexactas no afectan el resultado.
- **Banderas de compilación**: `-O2 -DNDEBUG` equivale al modo Release (en Debug CGAL es mucho más lento y falsea los tiempos). `-frounding-math` es recomendada por CGAL con GCC.
- **Conteo de aristas**: se calcula con `std::distance` sobre las aristas finitas, porque algunas versiones de CGAL (por ejemplo la 5.6) no tienen `number_of_edges()` en la triangulación.
- **Relaciones de Euler** que se verifican: V − E + F = 1, E = 3V − 3 − h y F = 2V − 2 − h, donde h es el número de vértices de la envolvente.
- **Reproducibilidad**: con la misma semilla y la misma versión de la librería estándar, la nube de puntos es idéntica. Los tiempos sí varían entre ejecuciones.

## Problemas frecuentes

| Problema | Solución |
|---|---|
| `fatal error: CGAL/...: No such file or directory` | Instala CGAL: `sudo apt install -y libcgal-dev`. |
| `cannot find -lgmp` o `-lmpfr` | `sudo apt install -y libgmp-dev libmpfr-dev`. |
| `ModuleNotFoundError: No module named 'matplotlib'` | `sudo apt install -y python3-matplotlib`. |
| `Permission denied` al ejecutar el script | Ejecútalo con `bash run_experimentos.sh`. |
| La compilación tarda mucho | Es normal la primera vez: CGAL usa muchas plantillas de C++. |
| Error de CMake por una carpeta `build` de otro proyecto | Este taller no usa CMake; compila directo con `g++` como se indica arriba. |
