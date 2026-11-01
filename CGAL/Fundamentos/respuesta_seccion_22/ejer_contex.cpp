#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/convex_hull_2.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

// 1. Definicion del Kernel y la estructura de Delaunay con ID de sensor (std::string)
using Kernel = CGAL::Exact_predicates_inexact_constructions_kernel;
using Point = Kernel::Point_2;
using Segment = Kernel::Segment_2;
using Ray = Kernel::Ray_2;

using Vb = CGAL::Triangulation_vertex_base_with_info_2<std::string, Kernel>;
using Fb = CGAL::Triangulation_face_base_2<Kernel>;
using Tds = CGAL::Triangulation_data_structure_2<Vb, Fb>;
using Delaunay = CGAL::Delaunay_triangulation_2<Kernel, Tds>;

int main(int argc, char *argv[])
{
    const std::string filename = (argc > 1) ? argv[1] : "ubicaciones_100_sensores_xy.csv";
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "Error: No se pudo abrir el archivo " << filename << "\n";
        return 1;
    }

    // 2. Lectura del CSV
    std::vector<std::pair<Point, std::string>> sensores;
    std::vector<Point> raw_points;
    std::string line;

    std::getline(file, line); // Saltar cabecera: sensor_id,x_m,y_m
    while (std::getline(file, line))
    {
        if (line.empty())
            continue;
        std::stringstream ss(line);
        std::string id, x_str, y_str;

        std::getline(ss, id, ',');
        std::getline(ss, x_str, ',');
        std::getline(ss, y_str, ',');

        // Limpiar posibles retornos de carro (\r)
        y_str.erase(std::remove(y_str.begin(), y_str.end(), '\r'), y_str.end());

        Point p(std::stod(x_str), std::stod(y_str));
        sensores.emplace_back(p, id);
        raw_points.push_back(p);
    }
    file.close();

    // 3. Construccion de la Envolvente Convexa y la Triangulacion de Delaunay
    std::vector<Point> hull;
    CGAL::convex_hull_2(raw_points.begin(), raw_points.end(), std::back_inserter(hull));

    Delaunay dt;
    dt.insert(sensores.begin(), sensores.end());

    const std::size_t n = dt.number_of_vertices();
    const std::size_t h = hull.size();
    std::size_t E = 0;
    for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge)
    {
        ++E;
    }
    const std::size_t F = dt.number_of_faces();

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "=================================================================\n";
    std::cout << " 1. RESUMEN TOPOLOGICO Y RELACIONES DE EULER (CGAL)\n";
    std::cout << "=================================================================\n";
    std::cout << "Sensores cargados (n):                 " << n << "\n";
    std::cout << "Vertices en la Envolvente Convexa (h): " << h << "\n";
    std::cout << "Aristas finitas en Delaunay (E):       " << E
              << " (Teorico 3n - 3 - h = " << (3 * n - 3 - h) << ")\n";
    std::cout << "Caras finitas en Delaunay (F):         " << F
              << " (Teorico 2n - 2 - h = " << (2 * n - 2 - h) << ")\n";

    // Identificar los IDs de los sensores en el borde (Envolvente Convexa)
    // En CGAL, los vertices adyacentes al vertice infinito (infinite_vertex) son la envolvente convexa
    std::cout << "Sensores perimetrales (Convex Hull):   ";
    auto vc = dt.incident_vertices(dt.infinite_vertex());
    auto done = vc;
    if (vc != nullptr)
    {
        do
        {
            std::cout << vc->info() << " ";
        } while (++vc != done);
    }
    std::cout << "\n\n";

    // 4. Analisis de Relaciones de Proximidad (Recorrido de Aristas de Delaunay)
    double min_dist = std::numeric_limits<double>::max();
    double max_dist = 0.0;
    double sum_dist = 0.0;
    std::string min_s1, min_s2, max_s1, max_s2;

    for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge)
    {
        auto face = edge->first;
        int idx = edge->second;

        auto v1 = face->vertex((idx + 1) % 3);
        auto v2 = face->vertex((idx + 2) % 3);

        double d = std::sqrt(CGAL::squared_distance(v1->point(), v2->point()));
        sum_dist += d;

        if (d < min_dist)
        {
            min_dist = d;
            min_s1 = v1->info();
            min_s2 = v2->info();
        }
        if (d > max_dist)
        {
            max_dist = d;
            max_s1 = v1->info();
            max_s2 = v2->info();
        }
    }

    std::cout << "=================================================================\n";
    std::cout << " 2. RELACIONES DE PROXIMIDAD (VECINOS DE DELAUNAY)\n";
    std::cout << "=================================================================\n";
    std::cout << "Par de sensores mas cercano: " << min_s1 << " - " << min_s2
              << " (" << min_dist << " m)\n";
    std::cout << "Arista mas larga (borde):    " << max_s1 << " - " << max_s2
              << " (" << max_dist << " m)\n";
    std::cout << "Distancia promedio entre vecinos: " << (sum_dist / E) << " m\n\n";

    // 5. Deteccion del Mayor Hueco de Cobertura (Vertices de Voronoi / Circuncentros)
    // Usando dt.dual(face) que entrega directamente el vertice del Diagrama de Voronoi
    double max_radius = 0.0;
    Point best_center(0.0, 0.0);
    std::string h_s1, h_s2, h_s3;

    for (auto face = dt.finite_faces_begin(); face != dt.finite_faces_end(); ++face)
    {
        Point voronoi_vertex = dt.dual(face); // Circuncentro del triangulo

        // Filtrar circuncentros dentro de la region de 100x100 m
        if (voronoi_vertex.x() >= 0.0 && voronoi_vertex.x() <= 100.0 &&
            voronoi_vertex.y() >= 0.0 && voronoi_vertex.y() <= 100.0)
        {

            double r = std::sqrt(CGAL::squared_distance(voronoi_vertex, face->vertex(0)->point()));
            if (r > max_radius)
            {
                max_radius = r;
                best_center = voronoi_vertex;
                h_s1 = face->vertex(0)->info();
                h_s2 = face->vertex(1)->info();
                h_s3 = face->vertex(2)->info();
            }
        }
    }

    std::cout << "=================================================================\n";
    std::cout << " 3. DETECCION DE PUNTOS CIEGOS (DUAL DE VORONOI EN CGAL)\n";
    std::cout << "=================================================================\n";
    std::cout << "Mayor hueco sin cobertura (circuncentro): ("
              << best_center.x() << " m, " << best_center.y() << " m)\n";
    std::cout << "Radio libre de sensores (R_max):          " << max_radius << " m\n";
    std::cout << "Triangulo de Delaunay que lo delimita:    "
              << h_s1 << ", " << h_s2 << ", " << h_s3 << "\n\n";

    // 6. Conteo de Aristas del Diagrama de Voronoi (Segmentos finitos vs Rayos infinitos)
    std::size_t voronoi_segments = 0;
    std::size_t voronoi_rays = 0;

    for (auto edge = dt.finite_edges_begin(); edge != dt.finite_edges_end(); ++edge)
    {
        CGAL::Object obj = dt.dual(*edge);
        Segment seg;
        Ray ray;
        if (CGAL::assign(seg, obj))
        {
            voronoi_segments++;
        }
        else if (CGAL::assign(ray, obj))
        {
            voronoi_rays++;
        }
    }

    // 7. Estimacion de areas de celdas de Voronoi dentro de [0, 100]x[0, 100]
    // usando consulta espacial dt.nearest_vertex(Point) propia de CGAL
    std::map<std::string, int> cell_counts;
    const int grid_res = 500; // Malla de 500x500 (250,000 puntos de control)
    const double step = 100.0 / grid_res;
    const double cell_area_unit = step * step;

    for (int i = 0; i < grid_res; ++i)
    {
        double x = (i + 0.5) * step;
        for (int j = 0; j < grid_res; ++j)
        {
            double y = (j + 0.5) * step;
            auto nearest_v = dt.nearest_vertex(Point(x, y));
            cell_counts[nearest_v->info()]++;
        }
    }

    std::string min_area_id, max_area_id;
    double min_area = std::numeric_limits<double>::max();
    double max_area = 0.0;

    for (const auto &kv : cell_counts)
    {
        double area = kv.second * cell_area_unit;
        if (area < min_area)
        {
            min_area = area;
            min_area_id = kv.first;
        }
        if (area > max_area)
        {
            max_area = area;
            max_area_id = kv.first;
        }
    }

    std::cout << "=================================================================\n";
    std::cout << " 4. DISTRIBUCION ESPACIAL Y CELDAS DE VORONOI (CGAL)\n";
    std::cout << "=================================================================\n";
    std::cout << "Aristas de Voronoi internas (Segment_2):  " << voronoi_segments << "\n";
    std::cout << "Aristas de Voronoi abiertas (Ray_2):      " << voronoi_rays
              << " (coincide con h = " << h << ")\n";
    std::cout << "Sensor con menor celda (redundancia):     " << min_area_id
              << " (~" << std::setprecision(2) << min_area << " m^2)\n";
    std::cout << "Sensor con mayor celda (sobrecargado):    " << max_area_id
              << " (~" << max_area << " m^2)\n";
    std::cout << "=================================================================\n";

    return 0;
}