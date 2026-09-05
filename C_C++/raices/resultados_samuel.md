---------------------------------------------
Resultados con la función e^(-x) - x
---------------------------------------------
Método          | raíz     | iteraciones | Observaciones |
---------------------------------------------

bisección       | 0.567143 |     28      | Es la que presenta la mayor cantidad de iteraciones, esto no se ve afectado en gran medida si se le aumenta o se le disminuye el limite superior, probe cambiando este con valores desde 5 hasta 1 y el número de iteraciones pasó de 30 a 28. Además, este presenta la misma limitante de que la raiz se debe encontrar entre el rango que tu le indiques.

----------------------------------------------

falsa posición  | 0.567143 |      7      | Al aumentarle el limite superior de 1 a 5, el número de iteraciones fueron aumentando, llegando hasta tener 10 iteraciones. Además, al igual que el caso del metodo brent, se requiere que la raiz de la función se encuentre dentro del rango que uno indica con x_lo y x_hi, si no genera un error.

----------------------------------------------

brent           | 0.567143 |      6      | En este caso, se tiene la limitante de que la raiz debe estar dentro del limite que se estipule; por ejemplo, cuando los limites son [1,5], da un error. Ahora bien, dejando el limite inferior en 0, y variando el limite superior de 1 a 5, el número de iteraciones que se obtiene no varía mucho, manteniendose entre 6 a 7 iteraciones. La raiz final si se mantiene constante, no cambia.

----------------------------------------------

newton          | 0.567143 |      4      | Al realizar las pruebas de ir disminuyendo el valor de la aproximación inicial de 5 a 1, el número de iteraciones fue disminuyendo, iniciando desde 6 hasta quedar en 4. Además, en los primeros intentos (con x0 = 5 y x0 = 4), las últimas iteraciones tenían una diferencia entre las raices muy pequeña, en el orden de 10^-16; mientras que, en los últimos intentos con valores de x0 más pequeños, la diferencia entre las raices obtenidas en cada iteración llegaban a estar en el orden de 10^-9, es una cosa que me pareció curiosa.

----------------------------------------------

secante         | 0.567143 |      5      | Se realizaron varios intentos con diferentes aproximaciones iniciales (x0), desde 5 a 1; con estos cambios se pasó de tener 7 iteraciones a 5 iteraciones. Da el mismo resultado en todas.

----------------------------------------------

steffenson      | 0.567143 |      6      | El número de iteraciones no se ve afectado si se le llega a cambiar el limite inferior en el que comience, probe desde 1 hasta 5 y las iteraciones se mantenían de 5 a 6 iteraciones. El resultado se mantiene constante en todas las veces que se cambió los limites.
