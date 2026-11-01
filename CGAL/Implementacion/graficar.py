# Uso: python3 graficar.py resultados/n100_s42
# Lee puntos.csv, envolvente.csv y aristas.csv de la carpeta y guarda figura.png
import csv
import os
import sys
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

def leer(ruta):
    with open(ruta) as f:
        lector = csv.reader(f)
        next(lector)
        return [list(map(float, fila)) for fila in lector]

carpeta = sys.argv[1].rstrip("/")
pts = leer(os.path.join(carpeta, "puntos.csv"))
hull = leer(os.path.join(carpeta, "envolvente.csv"))
edges = leer(os.path.join(carpeta, "aristas.csv"))

fig, ax = plt.subplots(figsize=(8, 8))
for x1, y1, x2, y2 in edges:
    ax.plot([x1, x2], [y1, y2], color="lightgray", lw=0.6, zorder=1)
hx = [p[0] for p in hull] + [hull[0][0]]
hy = [p[1] for p in hull] + [hull[0][1]]
ax.plot(hx, hy, color="red", lw=2, label="Envolvente convexa", zorder=3)
ax.scatter([p[0] for p in pts], [p[1] for p in pts], s=8, color="blue",
           label="Puntos", zorder=2)
ax.plot([], [], color="lightgray", label="Aristas de Delaunay")
ax.set_aspect("equal")
ax.set_title(f"{len(pts)} puntos - {os.path.basename(carpeta)}")
ax.legend(loc="upper right")
salida = os.path.join(carpeta, "figura.png")
fig.savefig(salida, dpi=150, bbox_inches="tight")
print("Guardado:", salida)
