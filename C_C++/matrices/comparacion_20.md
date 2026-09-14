## Comparación de resultados del punto 2.0: Octave vs C++ (Eigen3)

| Métrica                  | Octave      | C++ (Eigen) | Diferencia |
|---------------------------|-------------|-------------|------------|
| β₀ (intercepto)            | -123.4183   | -123.418    | ~0.0003    |
| β₁ (Voltaje)                | 6.3226      | 6.32257     | ~0.00003   |
| β₂ (Corriente)               | 19.0421     | 19.0421     | 0          |
| β₃ (Temperatura)             | -0.2068     | -0.206825   | ~0.00003   |
| Impacto V                  | 124.1120    | 124.112     | ~0.0000    |
| Impacto I                   | 186.4219    | 186.422     | ~0.0001    |
| Impacto T                    | -10.1861    | -10.1861    | 0          |
| ECM                        | 240.3765    | 240.377     | ~0.0005    |
| Número de condición de X   | 347.2169    | 347.217     | ~0.0001    |

### Análisis

**Coincidencia numérica.** Los resultados obtenidos en Octave y en C++ (usando `colPivHouseholderQr()` de Eigen) coinciden prácticamente cifra a cifra en las cuatro variables del modelo. Las mínimas diferencias observadas (del orden de 10⁻⁴ a 10⁻³) se deben únicamente a la precisión de impresión (Octave usa 4 decimales por defecto, C++ mostró entre 3 y 6 cifras significativas) y no a una discrepancia real en el cálculo: ambos lenguajes resuelven el sistema sobredeterminado mediante descomposición QR internamente (`\` en Octave, `colPivHouseholderQr()` en Eigen), por lo que el camino numérico es esencialmente el mismo.

**Interpretación del modelo:**

- **Variable con mayor impacto:** la **corriente (I)** tiene el mayor impacto relativo (≈186.4), seguida del voltaje (≈124.1). Esto es coherente físicamente, ya que la potencia eléctrica depende del producto V·I, y en este rango de datos la corriente domina la variación de la potencia de salida.
- **Efecto de la temperatura:** el coeficiente β₃ es negativo (-0.2068), lo que indica que a mayor temperatura del disipador, la potencia de salida tiende a disminuir levemente — consistente con el comportamiento esperado de un convertidor real, donde el aumento de temperatura incrementa las pérdidas y reduce la eficiencia.
- **Ajuste del modelo:** el ECM (≈240.38) da una idea de la dispersión promedio al cuadrado entre los valores predichos y los medidos; su magnitud debe interpretarse en relación con el rango típico de P en los datos (varias decenas a cientos de vatios), lo cual sugiere un ajuste razonable pero no perfecto — esperable dado que el modelo es lineal y el proceso físico real probablemente tiene componentes no lineales.
- **Condicionamiento numérico:** cond(X) ≈ 347, un valor moderado. No es un sistema mal condicionado (no está en el orden de 10⁴-10⁶+), pero tampoco es ideal (un sistema perfectamente condicionado tendría cond(X) ≈ 1). Esto indica que los coeficientes son razonablemente estables ante pequeñas perturbaciones en los datos, aunque conviene tener presente que la columna de 1's (intercepto) junto con variables de escalas muy distintas (V, I en unidades pequeñas vs T en unidades más grandes) contribuye a elevar este número.

**Conclusión:** Ambas implementaciones (Octave y C++/Eigen) producen resultados numéricamente equivalentes, validando tanto el planteamiento del modelo de mínimos cuadrados como su implementación en ambos lenguajes.
