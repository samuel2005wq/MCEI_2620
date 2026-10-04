"""Parte 2 - Python: solución vectorizada con NumPy"""
import numpy as np
import matplotlib
matplotlib.use("Agg")   # sin ventana: WSL no tiene pantalla
import matplotlib.pyplot as plt

import io
archivo = "/home/sag/MCEI_2620/trayectoria_robot.csv"
with open(archivo, encoding="utf-8") as f:
    txt = f.read().replace(",", ".")    # coma decimal -> punto
data = np.loadtxt(io.StringIO(txt), delimiter=";", skiprows=1)
t, x, y = data[:, 0], data[:, 1], data[:, 2]

vx = np.gradient(x, t)
vy = np.gradient(y, t)
v = np.sqrt(vx**2 + vy**2)
theta = np.unwrap(np.arctan2(vy, vx))
omega = np.gradient(theta, t)

# --- Gráficas ---
fig, ax = plt.subplots(2, 2, figsize=(10, 8))

ax[0, 0].plot(x, y, "b-")
ax[0, 0].set(xlabel="x", ylabel="y", title="Trayectoria y vs. x")
ax[0, 0].axis("equal")

ax[0, 1].plot(t, v, "r-")
ax[0, 1].set(xlabel="t", ylabel="v", title="v vs. t")

ax[1, 0].plot(t, theta, "g-")
ax[1, 0].set(xlabel="t", ylabel="theta (rad)", title="theta vs. t")

ax[1, 1].plot(t, omega, "m-")
ax[1, 1].set(xlabel="t", ylabel="omega (rad/s)", title="omega vs. t")

for a in ax.flat:
    a.grid(True)
plt.tight_layout()
plt.savefig("graficas.png", dpi=120)
print("Listo: grafica guardada en graficas.png")

# --- Comparación con la versión explícita de Octave (bucle en Python) ---
h = t[1] - t[0]
xd = np.empty_like(x)
xd[1:-1] = (x[2:] - x[:-2]) / (2 * h)   # central
xd[0] = (x[1] - x[0]) / h               # adelante
xd[-1] = (x[-1] - x[-2]) / h            # atrás
print("Máx. diferencia con np.gradient (vx):", np.max(np.abs(xd - vx)))

# Nota: np.gradient(x, t) usa diferencias centrales en el interior
# (reemplaza el bucle for de Octave) y diferencias de primer orden
# (adelante/atrás) en el primer y último punto. Si t es uniforme, el
# resultado coincide con (x[i+1]-x[i-1])/(2h); si no lo es, np.gradient
# ajusta la fórmula al espaciado real.