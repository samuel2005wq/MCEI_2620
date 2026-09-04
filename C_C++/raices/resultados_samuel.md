---------------------------------------------
Resultados 
---------------------------------------------
Método          | raíz     | iteraciones | Observaciones |
---------------------------------------------
bisección       | 0.20164  |      29     | Dependiendo de los limites inferiores y superiores que le pongas, se va a  tener una raiz diferente, esto se debe a que la función tiene 2 raices, y si por ejemplo, los limites los pones de 0 a 1, aparece la raiz en 0.20164, pero si le pones los limites van de 1 a 3, va a dar la raiz 2.12842. No obstante, si pones los limites que tomen las dos raices, va a generar problemas. El número de iteraciones en ambos casos son muy parecidos, van entre 27 a 29 iteraciones.

----------------------------------------------

falsa posición  | 0.20164  |      8      | Con los limites de 0 a 1, se demora menos en encontrar la raiz que en el caso de la raiz que se encuentra entre 1 y 3; entre 0 y 1 se demora 8 iteraciones, mientras que con los limites de 1 a 3 da 26 iteraciones, dando la raíz en 2.12842.

----------------------------------------------

brent           | 0.20164  |      6      | El número de iteraciones en ambos limites (de 0 a 1 y de 1 a 3) son muy cercanos, siendo en el primer caso de 6 iteraciones y en el otro caso de 9 iteraciones, dando como resultado 2.12842.