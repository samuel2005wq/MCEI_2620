// Experimentos con casos degenerados (pregunta 6 de analisis).
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Delaunay_triangulation_2.h>

#include <iostream>
#include <iterator>
#include <random>
#include <string>
#include <vector>

using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_2;
using Delaunay = CGAL::Delaunay_triangulation_2<Kernel>;

static void reportar(const std::string& nombre, const std::vector<Point>& pts)
{
    std::vector<Point> hull;
    CGAL::convex_hull_2(pts.begin(), pts.end(), std::back_inserter(hull));
    Delaunay dt;
    dt.insert(pts.begin(), pts.end());
    std::cout << "== " << nombre << " ==\n"
              << "  puntos insertados:     " << pts.size() << '\n'
              << "  vertices envolvente:   " << hull.size() << '\n'
              << "  vertices Delaunay:     " << dt.number_of_vertices() << '\n'
              << "  aristas finitas:       " << std::distance(dt.finite_edges_begin(), dt.finite_edges_end()) << '\n'
              << "  caras finitas:         " << dt.number_of_faces() << '\n'
              << "  dimension triangulacion: " << dt.dimension() << "\n\n";
}

int main()
{
    std::mt19937 gen(1);
    std::uniform_real_distribution<double> d(0.0, 100.0);

    // 1. Puntos repetidos: 100 puntos, cada uno insertado dos veces
    std::vector<Point> base, rep;
    for (int i = 0; i < 100; ++i) base.emplace_back(d(gen), d(gen));
    rep = base;
    rep.insert(rep.end(), base.begin(), base.end());
    reportar("1. Sin repetidos (100 puntos)", base);
    reportar("1. Con repetidos (100 puntos x2)", rep);

    // 2. Todos colineales (sobre y = x)
    std::vector<Point> col;
    for (int i = 0; i < 50; ++i) col.emplace_back(i * 2.0, i * 2.0);
    reportar("2. Puntos colineales (y = x)", col);

    // 3. Cocirculares exactos: 12 puntos sobre la circunferencia de radio 5
    std::vector<Point> circ = {
        {5, 0}, {-5, 0}, {0, 5}, {0, -5},
        {3, 4}, {3, -4}, {-3, 4}, {-3, -4},
        {4, 3}, {4, -3}, {-4, 3}, {-4, -3}};
    reportar("3. Cocirculares (12 puntos, radio 5)", circ);

    // 3b. Malla regular 10x10 (muchos grupos de 4 puntos cocirculares)
    std::vector<Point> grid;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j) grid.emplace_back(i, j);
    reportar("3b. Malla regular 10x10", grid);

    // 4. Casi colineales: A=(0,0), B=(1,1), C=(2,2+eps)
    std::cout << "== 4. Casi colineales: A=(0,0), B=(1,1), C=(2,2+eps) ==\n";
    std::cout << "  eps        | determinante double | orientacion CGAL\n";
    for (double eps : {1e-1, 1e-8, 1e-15, 1e-16, 1e-17}) {
        Point A(0, 0), B(1, 1), C(2, 2 + eps);
        double det = (B.x() - A.x()) * (C.y() - A.y())
                   - (B.y() - A.y()) * (C.x() - A.x());
        auto o = CGAL::orientation(A, B, C);
        std::string s = (o == CGAL::COLLINEAR) ? "COLINEALES"
                      : (o == CGAL::LEFT_TURN) ? "giro izquierda"
                                               : "giro derecha";
        std::cout << "  " << eps << "   | " << det << "   | " << s << '\n';
    }
    std::cout << "\n  Nota: 2+1e-17 se redondea a 2.0 en double, por lo que el punto\n"
                 "  realmente almacenado ya es colineal: el error ocurre al\n"
                 "  representar la entrada, no en el predicado.\n\n";

    // 5. Nube casi colineal: puntos sobre y=x con ruido de 1e-9
    std::vector<Point> casi;
    std::uniform_real_distribution<double> ruido(-1e-9, 1e-9);
    for (int i = 0; i < 200; ++i) {
        double t = d(gen);
        casi.emplace_back(t, t + ruido(gen));
    }
    reportar("5. Nube casi colineal (ruido 1e-9)", casi);
    return 0;
}
