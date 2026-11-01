#!/bin/bash
# Compila y corre todos los experimentos del punto 3.
# 4 tamanos x 3 semillas x 5 repeticiones de medicion.
set -e
FLAGS="-std=c++17 -O2 -DNDEBUG -frounding-math"
g++ $FLAGS taller.cpp -o taller -lgmp -lmpfr
g++ $FLAGS experimentos_pregunta_6.cpp -o experimentos_pregunta_6 -lgmp -lmpfr

SEMILLAS="7 314 1999"
TAMANOS="100 500 1000 5000"
REPS=5

mkdir -p resultados
echo "n,semilla,dimension,vertices_hull,vertices,aristas_finitas,caras_finitas,hull_ms,delaunay_ms,valida,euler,repeticion,geometria_reproducible" > resultados/mediciones.csv

for n in $TAMANOS; do
  for s in $SEMILLAS; do
    carpeta="resultados/n${n}_s${s}"
    mkdir -p "$carpeta"
    ./taller $n $s $REPS "$carpeta" | grep '^MED,' | sed 's/^MED,//' >> resultados/mediciones.csv
    echo "listo n=$n semilla=$s"
  done
done

./experimentos_pregunta_6 > resultados/experimentos_pregunta_6.txt
echo "Listo. Revisa la carpeta resultados/"
