# Ejercicio 1: Integración de una función $f(x) = e^{-0.4x}(1 + 0.5\sin(3x))$ en $[0, 8]$

## A. Análisis matemático

### 1. Obtención de la expresión analítica (Referencia de alta precisión)

La función a integrar en el intervalo $x \in [0, 8]$ es:
$$f(x) = e^{-0.4x} + 0.5 e^{-0.4x} \sin(3x)$$

Separando la integral en dos partes $I = I_1 + 0.5 \, I_2$:

1. **Primera integral ($I_1$):**
   $$I_1 = \int_{0}^{8} e^{-0.4x} \, dx = \left[ -\frac{1}{0.4} e^{-0.4x} \right]_{0}^{8} = 2.5 \left( 1 - e^{-3.2} \right)$$

2. **Segunda integral ($I_2$):**
   Utilizando la fórmula de integración por partes para funciones del tipo $\int e^{ax} \sin(bx) \, dx = \frac{e^{ax}}{a^2 + b^2} (a \sin(bx) - b \cos(bx))$ con $a = -0.4$ y $b = 3$:
   $$a^2 + b^2 = (-0.4)^2 + 3^2 = 0.16 + 9 = 9.16$$

   $$I_2 = \int_{0}^{8} e^{-0.4x} \sin(3x) \, dx = \left[ \frac{e^{-0.4x}}{9.16} \Big( -0.4 \sin(3x) - 3 \cos(3x) \Big) \right]_{0}^{8}$$

   Evaluando en los límites:
   * En $x = 8$: $\frac{e^{-3.2}}{9.16} \Big( -0.4 \sin(24) - 3 \cos(24) \Big)$
   * En $x = 0$: $\frac{1}{9.16} \Big( 0 - 3 \Big) = -\frac{3}{9.16}$

   $$I_2 = \frac{3 - e^{-3.2} \big( 0.4 \sin(24) + 3 \cos(24) \big)}{9.16}$$

3. **Valor exacto de la referencia:**
   $$I_{\text{ref}} = I_1 + 0.5 \, I_2 = 2.5(1 - e^{-3.2}) + \frac{1.5 - e^{-3.2} \big( 0.2 \sin(24) + 1.5 \cos(24) \big)}{9.16}$$

   Agrupando términos:
   $$I_{\text{ref}} = \frac{610}{229} - e^{-3.2} \left( 2.5 + \frac{0.2 \sin(24) + 1.5 \cos(24)}{9.16} \right) \approx \mathbf{2.559824508302063}$$

**Conclusión:** Sí es posible obtener una expresión analítica exacta, la cual utilizaremos como valor de referencia absoluto.