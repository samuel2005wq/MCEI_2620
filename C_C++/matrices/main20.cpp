#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <eigen3/Eigen/Dense>

using Eigen::MatrixXd;
using Eigen::VectorXd;

int main()
{
  std::ifstream archivo("/home/sag/MCEI_2620/C_C++/matrices/datos_convertidor_realista.csv");

  if (!archivo.is_open())
  {
    std::cerr << "ERROR: No se pudo abrir el archivo CSV. "
              << "Verifica que este en la misma carpeta desde donde ejecutas el programa." << std::endl;
    return 1;
  }

  std::string linea;
  std::vector<double> V, I, T, P;

  std::getline(archivo, linea); // saltar encabezado
  while (std::getline(archivo, linea))
  {
    std::stringstream ss(linea);
    std::string valor;
    std::getline(ss, valor, ',');
    V.push_back(std::stod(valor));
    std::getline(ss, valor, ',');
    I.push_back(std::stod(valor));
    std::getline(ss, valor, ',');
    T.push_back(std::stod(valor));
    std::getline(ss, valor, ',');
    P.push_back(std::stod(valor));
  }

  int n = V.size();
  MatrixXd X(n, 4);
  VectorXd y(n);

  for (int i = 0; i < n; ++i)
  {
    X(i, 0) = 1.0;
    X(i, 1) = V[i];
    X(i, 2) = I[i];
    X(i, 3) = T[i];
    y(i) = P[i];
  }

  // --- 1. Coeficientes por minimos cuadrados (QR, estable) ---
  VectorXd beta = X.colPivHouseholderQr().solve(y);
  std::cout << "Coeficientes [beta0 beta1 beta2 beta3]:\n"
            << beta.transpose() << "\n\n";

  // --- 2. Influencia relativa ---
  double impactoV = beta(1) * (V.empty() ? 0 : (*std::max_element(V.begin(), V.end()) - *std::min_element(V.begin(), V.end())));
  double impactoI = beta(2) * (*std::max_element(I.begin(), I.end()) - *std::min_element(I.begin(), I.end()));
  double impactoT = beta(3) * (*std::max_element(T.begin(), T.end()) - *std::min_element(T.begin(), T.end()));
  std::cout << "Impacto relativo (beta_i * rango_i):\n";
  std::cout << "V: " << impactoV << " | I: " << impactoI << " | T: " << impactoT << "\n\n";

  // --- 3. Residuales y ECM ---
  VectorXd y_pred = X * beta;
  VectorXd residual = y - y_pred;
  double ECM = residual.array().square().mean();
  std::cout << "Error Cuadratico Medio (ECM): " << ECM << "\n\n";

  // --- 5. Numero de condicion de X ---
  Eigen::JacobiSVD<MatrixXd> svd(X);
  double condX = svd.singularValues()(0) / svd.singularValues()(svd.singularValues().size() - 1);
  std::cout << "Numero de condicion de X: " << condX << "\n";

  return 0;
}
