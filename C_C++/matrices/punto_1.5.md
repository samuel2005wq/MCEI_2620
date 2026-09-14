# Punto 5 — Método de Mínimos Cuadrados para Ajuste de Modelos Lineales

**Materia:** Métodos Computacionales en Ingeniería (electiva técnica)
**Herramientas del taller:** Octave y C++ en entorno WSL / VS Code
**Tema:** Indagación con un LLM sobre mínimos cuadrados, usando la plantilla de prompt dada

---

## 1. Prompt utilizado (plantilla del taller)

Siguiendo la estructura `Actúa como [ROL]. El contexto es [SITUACIÓN]. Necesito que desarrolles [TAREA]. El resultado debe estar en formato [FORMATO]. Incluye [CRITERIOS].`, el prompt construido para esta indagación fue:

> **Rol:** Actúa como un profesional en álgebra lineal numérica.
> **Situación:** El contexto es un taller de la materia Métodos Computacionales en Ingeniería, donde se está aprendiendo a usar Octave y C++ en entornos simulados (WSL, VS Code) para resolver problemas de álgebra lineal (sistemas de ecuaciones, factorizaciones LU/QR/SVD, condicionamiento numérico).
> **Tarea:** Necesito que desarrolles una explicación detallada del método de mínimos cuadrados para el ajuste de modelos lineales.
> **Formato:** El resultado debe estar en formato Markdown (.md), con estilo de informe/solución de taller.
> **Criterios:** Incluye la formulación matemática del problema, la derivación de las ecuaciones normales, al menos dos métodos numéricos alternativos (QR y SVD) con su justificación de estabilidad, un ejemplo numérico resuelto, comparación de costo/estabilidad entre métodos, y referencias de implementación en Octave y C++ coherentes con lo trabajado en los puntos anteriores del taller.

A continuación se desarrolla la respuesta a ese prompt.

---

## 2. Planteamiento del problema

En la práctica de ingeniería es común tener **más datos (ecuaciones) que incógnitas**. Por ejemplo, al ajustar una recta o un polinomio a un conjunto de mediciones experimentales, se obtiene un sistema **sobredeterminado**:

```
A x ≈ b,        A ∈ ℝ^(m×n),  b ∈ ℝ^m,  x ∈ ℝ^n,   m > n
```

Este sistema, en general, **no tiene solución exacta** (el vector `b` casi nunca cae en el espacio columna de `A`). En vez de resolverlo exactamente, se busca el vector `x` que **minimiza el error cuadrático**:

```
minimizar   J(x) = ||A x - b||₂²  =  (Ax - b)ᵀ (Ax - b)
```

Esta es la idea central del **método de mínimos cuadrados**: no exigimos `Ax = b`, sino que el residuo `r = Ax - b` sea lo más pequeño posible en norma euclídea. Geométricamente, `Ax` es la **proyección ortogonal** de `b` sobre el espacio columna de `A`.

### Caso de uso típico: ajuste de un modelo lineal

Si tenemos `m` pares de datos `(t_i, y_i)` y queremos ajustar un modelo lineal en los parámetros, por ejemplo una recta `y = c₀ + c₁ t`, se construye:

```
A = [ 1   t₁ ]        b = [ y₁ ]        x = [ c₀ ]
    [ 1   t₂ ]            [ y₂ ]            [ c₁ ]
    [ ⋮   ⋮  ]            [ ⋮  ]
    [ 1   tₘ ]            [ yₘ ]
```

El mismo planteamiento se extiende a polinomios de mayor grado (agregando columnas `t_i²`, `t_i³`, ...) o a modelos con varias variables independientes — mientras el modelo sea **lineal en los parámetros** `x`, el problema sigue siendo mínimos cuadrados lineal.

---

## 3. Derivación: las ecuaciones normales

Para minimizar `J(x) = (Ax-b)ᵀ(Ax-b)`, se deriva respecto a `x` y se iguala a cero (condición de mínimo, ya que `J` es una función cuadrática convexa):

```
∇J(x) = 2 Aᵀ(Ax - b) = 0
      =>   Aᵀ A x = Aᵀ b
```

Estas son las **ecuaciones normales**. Si `A` tiene rango columna completo (`rank(A) = n`), entonces `AᵀA` es simétrica y definida positiva, y la solución es única:

```
x = (AᵀA)⁻¹ Aᵀ b
```

La matriz `A⁺ = (AᵀA)⁻¹Aᵀ` se conoce como la **pseudo-inversa de Moore-Penrose** de `A` (para el caso de rango completo).

### ⚠️ Problema numérico de las ecuaciones normales

Aunque la fórmula es elegante, **no es la forma recomendada de resolver el problema en la práctica**, porque:

```
cond(AᵀA) = cond(A)²
```

Es decir, al formar `AᵀA` se **eleva al cuadrado el número de condición** de `A`. Si `A` ya está algo mal condicionada (como vimos en el Punto 3 del taller), `AᵀA` puede quedar catastróficamente mal condicionada, amplificando muchísimo los errores de redondeo. Por eso, en software numérico serio, casi nunca se resuelve mínimos cuadrados factorizando `AᵀA` directamente; se prefieren métodos que trabajan sobre `A` sin formar ese producto.

---

## 4. Método 1: Factorización QR (recomendado en la práctica)

Si `A = QR`, con `Q` ortogonal (`m×m`, o su versión "thin" `m×n`) y `R` triangular superior, entonces:

```
||Ax - b||₂ = ||QRx - b||₂ = ||Rx - Qᵀb||₂     (pues Q preserva norma)
```

Minimizar esta expresión se reduce a resolver el sistema triangular:

```
R x = Qᵀ b     (por sustitución hacia atrás — igual que en el Punto 2 del taller)
```

**Ventaja clave:** nunca se calcula `AᵀA`, así que **no se duplica el número de condición**. Es el método estándar en la mayoría de librerías numéricas (LAPACK, Octave `\`, MATLAB `\`) cuando el sistema es rectangular.

---

## 5. Método 2: Descomposición SVD (el más robusto)

Con `A = U Σ Vᵀ` (como en el Punto 4 del taller), la solución de mínimos cuadrados de norma mínima es:

```
x = A⁺ b = V Σ⁺ Uᵀ b
```

donde `Σ⁺` se obtiene invirtiendo los valores singulares no nulos de `Σ` (y dejando cero donde `σᵢ ≈ 0`).

**¿Por qué es el más robusto?**
- Funciona incluso si `A` **no tiene rango completo** (columnas casi linealmente dependientes), caso en el que las ecuaciones normales y a veces incluso QR simple pueden fallar o volverse muy inestables.
- Permite aplicar **truncamiento de valores singulares pequeños** (regularización), evitando que ruido en los datos se amplifique en la solución cuando `A` está mal condicionada.
- Expone directamente el número de condición: `cond(A) = σ_max/σ_min`.

**Costo:** es el método más caro computacionalmente de los tres, por lo que se reserva para casos donde se sospecha mal condicionamiento, rango deficiente, o cuando se necesita diagnóstico numérico (igual que se concluyó en el Punto 4).

---

## 6. Comparación de los tres métodos

| Método              | Fórmula                          | Costo aprox.       | Estabilidad numérica                          | Cuándo usarlo |
|---------------------|-----------------------------------|---------------------|-----------------------------------------------|---------------|
| Ecuaciones normales  | `x=(AᵀA)⁻¹Aᵀb`                    | ~ `mn² + n³/3`      | **Baja**: `cond(AᵀA)=cond(A)²`                | Solo si `A` está muy bien condicionada y se prioriza velocidad |
| QR                  | `Rx = Qᵀb`                        | ~ `2mn² - 2n³/3`    | **Buena**: no eleva al cuadrado `cond(A)`     | Método estándar de propósito general |
| SVD                 | `x = VΣ⁺Uᵀb`                      | ~ `2mn² + 11n³`     | **Excelente**: maneja mal condicionamiento y rango deficiente | Datos ruidosos, sospecha de colinealidad, diagnóstico numérico |

Esta tabla es consistente con lo observado en los Puntos 2 y 4 del taller: SVD > QR > método directo, en cuanto a estabilidad, a costa de mayor tiempo de cómputo.

---

## 7. Ejemplo numérico resuelto

Ajustemos una recta `y = c₀ + c₁ t` a estos 5 puntos:

| t | 1 | 2 | 3 | 4 | 5 |
|---|---|---|---|---|---|
| y | 2.1 | 3.9 | 6.2 | 7.8 | 10.1 |

Se construye:

```
A = [1 1; 1 2; 1 3; 1 4; 1 5]        b = [2.1; 3.9; 6.2; 7.8; 10.1]
```

**Por ecuaciones normales:**

```
AᵀA = [ 5   15 ]      Aᵀb = [ 30.1  ]
      [15   55 ]            [102.9 ]

x = (AᵀA)⁻¹ Aᵀb ≈ [0.01 ; 2.00]
```

**Interpretación:** `c₀ ≈ 0.01`, `c₁ ≈ 2.00` → el modelo ajustado es aproximadamente `y ≈ 2.00·t`, consistente con los datos (que fueron generados alrededor de `y = 2t` con ruido pequeño).

Resolviendo el mismo sistema por **QR** o por **SVD** se obtiene, hasta la precisión de máquina, el mismo resultado — la diferencia entre métodos solo se hace evidente cuando `A` está mal condicionada (columnas casi paralelas, por ejemplo si se agregan potencias altas de `t` sin centrar/escalar los datos).

---

## 8. Referencia de implementación (coherente con el taller)

### Octave

```matlab
% Mínimos cuadrados con matriz rectangular A (m > n)
% Opción recomendada: dejar que Octave elija QR internamente
x = A \ b;

% Explícito por QR (como en punto2/punto4 del taller)
[Q, R] = qr(A, 0);      % QR "económica" (thin QR)
x_qr = R \ (Q' * b);

% Explícito por SVD (pseudo-inversa)
[U, S, V] = svd(A, 0);
x_svd = V * (S \ (U' * b));   % equivalente a pinv(A) * b
```

### C++

Reutilizando las mismas funciones ya construidas en los Puntos 2 y 4 del taller (`qrHouseholder`, `svdOneSidedJacobi`), el ajuste por mínimos cuadrados se reduce a:

```cpp
// A es m x n (m > n). Con QR "delgada" (m x n) y R (n x n):
// 1) Descomponer A = Q*R
// 2) y = Qᵀ * b   (tomando solo las primeras n columnas de Q)
// 3) Resolver R*x = y por sustitución hacia atrás (backSub, ya implementada)

// Con SVD:
// 1) Descomponer A = U*S*Vᵀ
// 2) x = V * Sinv * (Uᵀ * b), con Sinv = diag(1/σᵢ) (0 si σᵢ ≈ 0)
```

No hace falta reescribir estas rutinas desde cero: son **exactamente las mismas** factorizaciones QR y SVD ya usadas para invertir matrices en el Punto 4, solo que aquí se aplican a una matriz `A` rectangular (`m×n`, `m>n`) en vez de cuadrada.

---

## 9. Conclusiones

1. Mínimos cuadrados resuelve sistemas **sobredeterminados** minimizando `||Ax-b||₂`, y su solución satisface las ecuaciones normales `AᵀAx=Aᵀb`.
2. **No se debe** resolver formando `AᵀA` explícitamente en problemas mal condicionados, porque `cond(AᵀA)=cond(A)²`.
3. **QR** es el estándar de la industria: buena estabilidad y costo razonable.
4. **SVD** es la opción más robusta, especialmente si `A` tiene columnas casi colineales o rango deficiente; también permite regularizar truncando valores singulares pequeños.
5. Esta jerarquía de estabilidad (SVD ≥ QR > ecuaciones normales) es la misma que se encontró al comparar métodos de inversión de matrices en el Punto 4 del taller — es un patrón general en álgebra lineal numérica, no exclusivo de mínimos cuadrados.
