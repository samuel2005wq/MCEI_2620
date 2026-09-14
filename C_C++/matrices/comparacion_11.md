------------------------------------------------------------
Comparación de resultados de Octave y C/C++ del punto 1.1 
------------------------------------------------------------
Como tal, los dos resultados obtenidos con el programa en C++ (usando Eigen3) 
como los obtenidos en Octave (usando pinv) coinciden en todas sus componentes significativas, 
confirmando que ambas implementaciones calculan correctamente la pseudoinversa de Moore-Penrose 
y verifican sus cuatro propiedades características. Las pequeñas diferencias observadas en las 
posiciones y magnitudes exactas del error de redondeo (del orden de 1e-16) se deben a diferencias 
en las implementaciones internas de la descomposición SVD utilizadas por cada software, 
pero estas no llegan a ser una diferencia significativa en el resultado y en su significado.
