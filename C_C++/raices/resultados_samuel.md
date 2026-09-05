---------------------------------------------
Resultados con la función e^(-x) - x
---------------------------------------------
Método          | raíz     | iteraciones | Observaciones |
---------------------------------------------

bisección       |          |             | 

----------------------------------------------

falsa posición  |          |             | 

----------------------------------------------

brent           |          |             | 

----------------------------------------------

newton          | 0.567143 |      4      | Al realizar las pruebas de ir disminuyendo el valor de la aproximación inicial de 5 a 1, el número de iteraciones fue disminuyendo, iniciando desde 6 hasta quedar en 4. Además, en los primeros intentos (con x0 = 5 y x0 = 4), las últimas iteraciones tenían una diferencia entre las raices muy pequeña, en el orden de 10^-16; mientras que, en los últimos intentos con valores de x0 más pequeños, la diferencia entre las raices obtenidas en cada iteración llegaban a estar en el orden de 10^-9, es una cosa que me pareció curiosa.

----------------------------------------------

secante         | 0.567143 |      5      | Se realizaron varios intentos con diferentes aproximaciones iniciales (x0), desde 5 a 1; con estos cambios se pasó de tener 7 iteraciones a 5 iteraciones. Da el mismo resultado en todas.

----------------------------------------------

steffenson      | 0.567143 |      6      | El número de iteraciones no se ve afectado si se le llega a cambiar el limite inferior en el que comience, probe desde 1 hasta 5 y las iteraciones se mantenían de 5 a 6 iteraciones. El resultado se mantiene constante en todas las veces que se cambió los limites.
