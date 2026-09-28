# Respuestas a las Preguntas de Análisis

## 1. ¿Qué cambia en el trapecio al aumentar $n$?

Al incrementar el número de subdivisiones $n$ en la regla del trapecio compuesto:

* **Reducción del tamaño de paso ($h$):** El ancho de los subintervalos $h = (b-a)/n$ disminuye progresivamente. Esto permite que la aproximación lineal siga de forma mucho más fiel las oscilaciones de la función.
* **Disminución cuadrática del error:** Al tratarse de un método de orden 2, el error decae de forma proporcional a $O(h^2) = O(1/n^2)$. Por ejemplo, al duplicar $n$ (de 10 a 20), el error disminuye aproximadamente a la cuarta parte (de $6.58 \times 10^{-2}$ a $1.49 \times 10^{-2}$).
* **Incremento lineal del costo computacional:** Se requieren $n+1$ evaluaciones de la función $f(x)$. En C++, el tiempo de ejecución crece de forma proporcional a $n$, mientras que en entornos interpretados como Octave el tiempo inicial se ve dominado por la sobrecarga del sistema.

El orden del método se mantiene constante en 2; la única vía para mejorar la precisión mediante este esquema es densificar la malla de puntos.

---

## 2. ¿Qué diferencia hay entre aumentar $n$ y reducir la tolerancia de un método adaptativo?

La diferencia principal radica en el mecanismo de control y en la flexibilidad de la malla:

* **Aumentar $n$ (Trapecio):** Implica un **control manual y global**. El usuario define una malla uniforme fija sin recibir una estimación del error ni un refinamiento inteligente.
* **Reducir la tolerancia (Métodos adaptativos como GSL/SciPy/`quadgk`):** Corresponde a un **control automático y local** centrado en la calidad. El usuario establece un margen de error admisible (`epsabs`, `epsrel`), y el algoritmo subdivide de forma dinámica únicamente las regiones de mayor variabilidad mediante reglas de alto orden (Gauss–Kronrod).

En el trapecio, cada ganancia en exactitud exige indefectiblemente elevar el número de puntos. En los métodos adaptativos, si el refinamiento base ya satisface una tolerancia más estricta, el costo computacional no se incrementa (por ejemplo, SciPy mantiene 63 evaluaciones para tolerancias desde $10^{-4}$ hasta $10^{-10}$).

---

## 3. ¿Mayor precisión implica necesariamente mayor costo?

No de forma absoluta; la relación entre costo y precisión depende de si comparamos algoritmos distintos o ajustes dentro de un mismo esquema:

* **Entre algoritmos distintos:** Una cuadratura adaptativa de alto orden resulta abrumadoramente más eficiente. Por ejemplo, GSL `qags` alcanza precisión de máquina ($\sim 10^{-16}$) con solo 63 evaluaciones de la función, siendo mucho más preciso y veloz que la regla del trapecio con $n=1000$.
* **Dentro de un mismo método adaptativo:** Exigir mayor exactitud sí puede incrementar el costo cuando el algoritmo requiere realizar nuevas subdivisiones (como al pasar a una tolerancia de $10^{-12}$ en SciPy, donde las evaluaciones subieron de 63 a 147).
* **Influencia de la plataforma:** El tiempo real también está condicionado por el lenguaje de programación y la sobrecarga de ejecución (C++ ejecuta las mismas evaluaciones mucho más rápido que Python o Octave).

---

## 4. Comparación del error estimado por GSL/SciPy con el error respecto a la referencia

Al contrastar la estimación interna de los algoritmos adaptativos frente al error verdadero obtenido con la referencia analítica:

* **Estimación conservadora:** El error estimado devuelto por los métodos de QUADPACK (GSL y SciPy, aproximadamente $1.458 \times 10^{-10}$) funciona como una **cota superior de seguridad**. El error real alcanzado es sustancialmente menor ($\approx 4.44 \times 10^{-16}$), demostrando que el algoritmo sobreestima el error de forma preventiva para garantizar la tolerancia.
* **Límite de precisión numérica:** El error real alcanzado ($\sim 10^{-16}$) se encuentra en el límite teórico de la representación en coma flotante de doble precisión ($\epsilon_{\text{mach}}$).
* **Consistencia entre librerías:** GSL y SciPy entregan la misma estimación debido a que comparten el motor QAGS. Por su parte, `quadgk` de Octave aplica un esquema distinto pero mantiene el mismo principio de cota segura ($6.39 \times 10^{-12}$).
* **Utilidad práctica:** En problemas reales sin solución analítica previa, este error estimado constituye la única garantía matemática disponible para verificar la validez del resultado.