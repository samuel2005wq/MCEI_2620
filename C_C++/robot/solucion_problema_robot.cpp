// Parte 3 - C++: diferencias centrales con paso no necesariamente uniforme
// Compilar:  g++ -O2 -std=c++17 parte3_cpp.cpp -o parte3
// Ejecutar:  ./parte3

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;

// Lee t;x;y (separador ';', coma decimal, 1 fila de encabezado)
bool leerCSV(const string &archivo, vector<double> &t,
             vector<double> &x, vector<double> &y)
{
    ifstream f(archivo);
    if (!f)
        return false;

    string linea;
    getline(f, linea); // saltar encabezado
    while (getline(f, linea))
    {
        if (linea.find_first_not_of(" \t\r\n") == string::npos)
            continue;
        replace(linea.begin(), linea.end(), ',', '.'); // coma decimal -> punto
        replace(linea.begin(), linea.end(), ';', ' '); // separador -> espacio
        istringstream ss(linea);
        double a, b, c;
        if (ss >> a >> b >> c)
        {
            t.push_back(a);
            x.push_back(b);
            y.push_back(c);
        }
    }
    return !t.empty();
}

// Derivada de q respecto a t:
//  - interior: diferencia central (q[i+1]-q[i-1])/(t[i+1]-t[i-1])
//  - extremos: diferencia hacia adelante / atrás (no existe vecino a ambos lados)
vector<double> derivar(const vector<double> &t, const vector<double> &q)
{
    size_t N = t.size();
    vector<double> d(N);
    for (size_t i = 1; i + 1 < N; i++)
        d[i] = (q[i + 1] - q[i - 1]) / (t[i + 1] - t[i - 1]);
    d[0] = (q[1] - q[0]) / (t[1] - t[0]);
    d[N - 1] = (q[N - 1] - q[N - 2]) / (t[N - 1] - t[N - 2]);
    return d;
}

// Desenvolvimiento angular (equivalente a unwrap): evita saltos de +-2*pi
void desenvolver(vector<double> &th)
{
    const double DOSPI = 2.0 * M_PI;
    for (size_t i = 1; i < th.size(); i++)
    {
        while (th[i] - th[i - 1] > M_PI)
            th[i] -= DOSPI;
        while (th[i] - th[i - 1] < -M_PI)
            th[i] += DOSPI;
    }
}

int main()
{
    const string archivo = "/home/sag/MCEI_2620/trayectoria_robot.csv";
    vector<double> t, x, y;

    if (!leerCSV(archivo, t, x, y))
    {
        cerr << "No se pudo leer " << archivo << endl;
        return 1;
    }
    size_t N = t.size();
    if (N < 3)
    {
        cerr << "Se necesitan al menos 3 puntos." << endl;
        return 1;
    }

    // Velocidades en x e y
    vector<double> vx = derivar(t, x);
    vector<double> vy = derivar(t, y);

    // Velocidad lineal y orientación
    vector<double> v(N), theta(N);
    for (size_t i = 0; i < N; i++)
    {
        v[i] = sqrt(vx[i] * vx[i] + vy[i] * vy[i]);
        theta[i] = atan2(vy[i], vx[i]);
    }
    desenvolver(theta); // ANTES de calcular omega

    // Velocidad angular
    vector<double> omega = derivar(t, theta);

    // Guardar resultados (separador ';' y punto decimal)
    ofstream out("resultados_cpp.csv");
    out << "t;x;y;vx;vy;v;theta;omega\n";
    for (size_t i = 0; i < N; i++)
        out << t[i] << ';' << x[i] << ';' << y[i] << ';' << vx[i] << ';'
            << vy[i] << ';' << v[i] << ';' << theta[i] << ';' << omega[i] << '\n';

    // Vista rápida en pantalla
    cout << "Puntos leidos: " << N << "\n";
    cout << "t\tv\ttheta\tomega\n";
    for (size_t i = 0; i < min<size_t>(N, 5); i++)
        cout << t[i] << "\t" << v[i] << "\t" << theta[i] << "\t" << omega[i] << "\n";
    cout << "Resultados completos en resultados_cpp.csv\n";

    return 0;
}