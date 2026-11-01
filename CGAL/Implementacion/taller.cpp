// Punto 3 del taller: nube de puntos, envolvente convexa y triangulacion de Delaunay.
// Uso: ./taller N SEMILLA [REPETICIONES] [CARPETA_SALIDA]
//  - La nube se genera UNA vez con la semilla dada.
//  - Envolvente y Delaunay se calculan REPETICIONES veces sobre la misma nube
//    (para medir la variacion del tiempo sin cambiar la geometria).
//  - Si se da CARPETA_SALIDA, escribe puntos.csv, envolvente.csv, aristas.csv
//    y resumen.csv en esa carpeta (debe existir).
//  - Imprime por stdout una linea "MED,..." por repeticion (para mediciones.csv).
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_2;
using Delaunay = CGAL::Delaunay_triangulation_2<Kernel>;
using Clock = std::chrono::steady_clock;

static double ms(Clock::time_point a, Clock::time_point b)
{
    return std::chrono::duration<double, std::milli>(b - a).count();
}

struct Conteo {
    long hull, V, E, F;
    int dim;
    bool operator==(const Conteo& o) const {
        return hull == o.hull && V == o.V && E == o.E && F == o.F && dim == o.dim;
    }
};

int main(int argc, char* argv[])
{
    const std::size_t n = (argc > 1) ? std::stoul(argv[1]) : 100;
    const unsigned int seed = (argc > 2) ? static_cast<unsigned>(std::stoul(argv[2])) : 42u;
    const int reps = (argc > 3) ? std::stoi(argv[3]) : 5;
    const std::string dir = (argc > 4) ? argv[4] : "";

    // Nube de puntos (una sola vez)
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> distribution(0.0, 100.0);
    std::vector<Point> points;
    points.reserve(n);
    for (std::size_t i = 0; i < n; ++i)
        points.emplace_back(distribution(generator), distribution(generator));

    Conteo primero{};
    std::vector<Point> hull_guardada;
    Delaunay dt_guardada;
    double t_hull_1 = 0, t_del_1 = 0;
    bool valida_1 = false;

    for (int r = 1; r <= reps; ++r) {
        std::vector<Point> hull;
        auto t0 = Clock::now();
        CGAL::convex_hull_2(points.begin(), points.end(), std::back_inserter(hull));
        auto t1 = Clock::now();

        Delaunay dt;
        auto t2 = Clock::now();
        dt.insert(points.begin(), points.end());
        auto t3 = Clock::now();

        Conteo c;
        c.hull = static_cast<long>(hull.size());
        c.V = static_cast<long>(dt.number_of_vertices());
        c.E = static_cast<long>(std::distance(dt.finite_edges_begin(), dt.finite_edges_end()));
        c.F = static_cast<long>(dt.number_of_faces());
        c.dim = dt.dimension();

        const bool valida = dt.is_valid();
        const bool euler = (c.V - c.E + c.F == 1);
        if (r == 1) {
            primero = c;
            hull_guardada = hull;
            dt_guardada = dt;
            t_hull_1 = ms(t0, t1);
            t_del_1 = ms(t2, t3);
            valida_1 = valida;
        }
        const bool reproducible = (c == primero);

        // n,semilla,dimension,vertices_hull,vertices,aristas_finitas,caras_finitas,
        // hull_ms,delaunay_ms,valida,euler,repeticion,geometria_reproducible
        std::cout << "MED," << n << ',' << seed << ',' << c.dim << ',' << c.hull << ','
                  << c.V << ',' << c.E << ',' << c.F << ',' << ms(t0, t1) << ','
                  << ms(t2, t3) << ',' << valida << ',' << euler << ',' << r << ','
                  << reproducible << '\n';
    }

    if (dir.empty()) return 0;

    std::ofstream fp(dir + "/puntos.csv");
    fp << std::setprecision(17) << "x,y\n";
    for (const auto& p : points) fp << p.x() << ',' << p.y() << '\n';

    std::ofstream fh(dir + "/envolvente.csv");
    fh << std::setprecision(17) << "x,y\n";
    for (const auto& p : hull_guardada) fh << p.x() << ',' << p.y() << '\n';

    std::ofstream fe(dir + "/aristas.csv");
    fe << std::setprecision(17) << "x1,y1,x2,y2\n";
    for (auto e = dt_guardada.finite_edges_begin(); e != dt_guardada.finite_edges_end(); ++e) {
        auto face = e->first;
        int i = e->second;
        Point p = face->vertex((i + 1) % 3)->point();
        Point q = face->vertex((i + 2) % 3)->point();
        fe << p.x() << ',' << p.y() << ',' << q.x() << ',' << q.y() << '\n';
    }

    std::ofstream fr(dir + "/resumen.csv");
    fr << "n,semilla,dimension,vertices_hull,vertices,aristas_finitas,caras_finitas,"
          "hull_ms_rep1,delaunay_ms_rep1,valida,euler_V-E+F,aristas_esperadas,caras_esperadas\n";
    fr << n << ',' << seed << ',' << primero.dim << ',' << primero.hull << ',' << primero.V
       << ',' << primero.E << ',' << primero.F << ',' << t_hull_1 << ',' << t_del_1 << ','
       << valida_1 << ',' << (primero.V - primero.E + primero.F) << ','
       << (3 * primero.V - 3 - primero.hull) << ',' << (2 * primero.V - 2 - primero.hull) << '\n';
    return 0;
}
