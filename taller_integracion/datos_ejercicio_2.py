import csv
import math

# Nombre del archivo de salida
filename = "datos_sensor.csv"

# Abrir y escribir el archivo
with open(filename, mode="w", newline="", encoding="utf-8") as file:
    writer = csv.writer(file)

    # 1. Escribir los encabezados
    writer.writerow(["x", "y"])

    # 2. Generar las 50 mediciones (i = 0 hasta 49)
    for i in range(50):
        x_i = 0.2 * i
        y_i = (
            2.0
            + 0.35 * math.sin(0.7 * x_i)
            + 0.15 * math.cos(2.1 * x_i)
            + 0.03 * x_i
        )

        # Guardar en el CSV
        writer.writerow([x_i, y_i])

print(f"¡Archivo '{filename}' generado con éxito con 50 mediciones!")