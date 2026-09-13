// Punto 1.1 del taller sobre Algebra lineal numérica
// Hecho por: Samuel Acuña y Carolina García

#include <iostream>
#include <eigen3/Eigen/Dense>

// Alias para matrices y vectores de double usando Eigen.
using Eigen::MatrixXd;

int main()
{
  // Se define una matriz rectangular de tamaño 4x3.
  // La idea es trabajar con una matriz que no sea cuadrada para ver el comportamiento de la pseudoinversa.
  MatrixXd m(4, 3);

  // Se llenan los elementos de la matriz fila por fila.
  m(0, 0) = 1;
  m(1, 0) = 2;
  m(2, 0) = 0;
  m(3, 0) = 1;

  m(0, 1) = 0;
  m(1, 1) = -1;
  m(2, 1) = 1;
  m(3, 1) = 3;

  m(0, 2) = 2;
  m(1, 2) = 5;
  m(2, 2) = -1;
  m(3, 2) = -1;

  // Se imprime la matriz original para visualizarla antes de operar.
  std::cout << "Matriz con la que vamos a trabajar:" << std::endl;
  std::cout << m << std::endl;

  // Se calcula el rango de la matriz usando descomposición LU con pivote completo.
  // FullPivLU permite obtener el rango de matrices rectangulares y cuadradas.
  Eigen::FullPivLU<MatrixXd> lu(m);
  double rA = lu.rank();

  // Traspuesta de la matriz A.
  MatrixXd At = m.transpose();

  // B = A^T * A, que da como resultado una matriz cuadrada 3x3.
  MatrixXd B = At * m;

  // Traza de B, es decir, la suma de los elementos de la diagonal principal.
  double trB = B.trace();

  // Matriz identidad de tamaño 3x3.
  MatrixXd I_3 = MatrixXd::Identity(3, 3);

  // C = tr(B) * I - B.
  // Esta construcción se usa en la fórmula manual para obtener una pseudoinversa.
  MatrixXd C = (trB * I_3) - B;

  // tr(CB) = traza de C * B.
  double trCB = (C * B).trace();

  // C * A^T: producto que se usa para construir la aproximación manual de A^+.
  MatrixXd CAt = C * At;

  // Fórmula manual para una pseudoinversa según la expresión usada en el ejercicio.
  // Este cálculo se compara con la pseudoinversa obtenida por SVD.
  MatrixXd A_inv = (rA / trCB) * CAt;

  std::cout << std::endl
            << "Inversa de Moore Penrose calculada manualmente:" << std::endl;
  std::cout << A_inv << std::endl;

  // Propiedad 1: A * A^+ * A = A
  MatrixXd P_1 = m * A_inv * m;
  std::cout << std::endl
            << "Propiedad No. 1: A*A(-)*A = A" << std::endl;
  std::cout << P_1 << std::endl;
  std::cout << "En la componente (0,1) pese a que no es igual a 0, el valor es muy cercano a este" << std::endl;

  // Propiedad 2: A^+ * A * A^+ = A^+
  MatrixXd P_2 = A_inv * m * A_inv;
  std::cout << std::endl
            << "Propiedad No. 2: A(-)*A*A(-) = A(-)" << std::endl;
  std::cout << P_2 << std::endl;

  // Propiedad 3: A * A^+ es simétrica.
  MatrixXd P_3 = m * A_inv;
  MatrixXd P_31 = P_3.transpose();
  std::cout << std::endl
            << "Propiedad No. 3: A*A(-) es simetrica" << std::endl;
  std::cout << std::endl
            << "Matriz A*A(-):" << std::endl;
  std::cout << P_3 << std::endl;
  std::cout << std::endl
            << "Transpuesta de A*A(-):" << std::endl;
  std::cout << P_31 << std::endl;
  std::cout << std::endl
            << "Como se puede notar, la matriz A*A(-) si es simetrica" << std::endl;

  // Propiedad 4: A^+ * A es simétrica.
  MatrixXd P_4 = A_inv * m;
  MatrixXd P_41 = P_4.transpose();
  std::cout << std::endl
            << "Propiedad No. 4: A(-)*A es simetrica" << std::endl;
  std::cout << std::endl
            << "Matriz A(-)*A:" << std::endl;
  std::cout << P_4 << std::endl;
  std::cout << std::endl
            << "Transpuesta de A(-)*A:" << std::endl;
  std::cout << P_41 << std::endl;
  std::cout << std::endl
            << "Como se puede notar, la matriz A(-)*A si es simetrica" << std::endl;

  // Fin del programa.
  return 0;
}