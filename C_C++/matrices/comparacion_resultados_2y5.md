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

## Respuestas a preguntas de análisis

### 1. ¿Qué variable tiene mayor impacto en la potencia?

La **corriente de carga (I)** es la variable con mayor impacto, con un valor de impacto relativo de **186.42**, superando al voltaje (124.11) y muy por encima de la temperatura (-10.19). Esto tiene sentido físico: la potencia eléctrica está directamente relacionada con el producto V·I, y en el rango de valores presentes en los datos, la variación de la corriente explica una porción mayor de la variación de la potencia de salida que la variación del voltaje.

### 2. ¿Cómo influye la temperatura en el rendimiento?

El coeficiente β₃ = -0.2068 indica una relación **inversa**: a medida que aumenta la temperatura del disipador, la potencia de salida tiende a disminuir levemente (manteniendo V e I constantes). Esto es consistente con el comportamiento físico esperado de un convertidor DC-DC real, donde el incremento de temperatura eleva las pérdidas resistivas y de conmutación en los semiconductores, reduciendo la eficiencia global del sistema. Sin embargo, su impacto relativo (-10.19) es considerablemente menor que el de V o I, lo que sugiere que, dentro del rango de temperaturas medido, este efecto es secundario frente a las variables eléctricas.

### 3. ¿Qué ocurre al aumentar el número de muestras?

Al aumentar el número de mediciones (n), en general se espera:

- **Mayor precisión en la estimación de β̂**, ya que el sistema de mínimos cuadrados se apoya en más información y los coeficientes se vuelven menos sensibles a mediciones individuales ruidosas (outliers).
- **Reducción de la varianza del estimador**, dado que el error estándar de los coeficientes decrece aproximadamente con 1/√n.
- El **número de condición de X** puede mejorar (estabilizarse) si las nuevas muestras cubren mejor el espacio de las variables independientes (V, I, T), evitando que columnas queden casi linealmente dependientes entre sí.
- El **ECM** tiende a estabilizarse hacia el verdadero nivel de ruido del proceso, en lugar de ajustarse artificialmente a particularidades de una muestra pequeña (sobreajuste).

En pocas palabras: más muestras generalmente dan una estimación más confiable y robusta del modelo, siempre que los datos adicionales sean representativos y no introduzcan valores atípicos o errores sistemáticos.

### 4. ¿Por qué QR es preferible a ecuaciones normales?

El método de ecuaciones normales requiere calcular explícitamente:

$$\hat\beta = (X^TX)^{-1}X^Ty$$

El problema es que al formar el producto **XᵀX**, el número de condición del sistema se **eleva al cuadrado**: cond(XᵀX) ≈ cond(X)². En nuestro caso, cond(X) ≈ 347, lo que significaría trabajar con un sistema de condición ≈ 120,000 si se usaran ecuaciones normales — amplificando enormemente cualquier error de redondeo en aritmética de punto flotante.

La descomposición **QR**, en cambio, trabaja directamente sobre X (sin necesidad de formar XᵀX ni invertir nada explícitamente), preservando el número de condición original del problema. Por eso tanto el operador `\` de Octave como `colPivHouseholderQr()` de Eigen usan QR (o variantes) internamente: es la forma numéricamente estable de resolver sistemas de mínimos cuadrados, especialmente cuando X tiene columnas con escalas muy distintas o está cerca de ser mal condicionada.

### 5. ¿Cómo se interpreta físicamente cada coeficiente?

- **β₀ (-123.42):** es el intercepto del modelo — representaría la potencia de salida cuando V = I = T = 0. Físicamente no tiene un significado directo útil (un convertidor no opera en ese punto), es simplemente el término que ajusta el nivel base de la recta de regresión dentro del rango de datos observado.
- **β₁ (6.32):** por cada voltio adicional en la entrada (manteniendo I y T fijos), la potencia de salida aumenta en promedio 6.32 W. Refleja la sensibilidad de P respecto al voltaje de entrada.
- **β₂ (19.04):** por cada amperio adicional de corriente de carga (manteniendo V y T fijos), la potencia aumenta en promedio 19.04 W. Es el coeficiente de mayor magnitud, consistente con que la corriente es la variable de mayor impacto identificada en la pregunta 1.
- **β₃ (-0.21):** por cada grado Celsius adicional en el disipador (manteniendo V e I fijos), la potencia de salida disminuye en promedio 0.21 W — cuantificando el efecto de las pérdidas térmicas sobre el rendimiento del convertidor.
