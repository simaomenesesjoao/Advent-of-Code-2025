#include <cassert>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <set>
#include <queue>

#include "utils/utils.h"

struct Point2D {
    int x, y;

    // static L distance_sq(const Point2D& P, const Point2D& Q){
    //     int dx = P.x - Q.x;
    //     int dy = P.y - Q.y;
    //     return L(dx)*dx + L(dy)*dy + L(dz)*dz;
    // }

    static L rectangle_area(const Point2D& P, const Point2D& Q){
        int dx = std::abs(P.x - Q.x) + 1;
        int dy = std::abs(P.y - Q.y) + 1;
        return L(dx)*dy;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Point2D& P) {
        stream << "(" << P.x << ", " << P.y << ")";
        return stream;
    }

    friend Point2D operator-(const Point2D& P, const Point2D& Q){
        return {P.x - Q.x, P.y - Q.y};
    }

    friend Point2D operator+(const Point2D& P, const Point2D& Q){
        return {P.x + Q.x, P.y + Q.y};
    }

    friend bool operator==(const Point2D& P, const Point2D& Q){
        return P.x == Q.x && P.y == Q.y;
    }

};


void print_points(const std::vector<Point2D>& points){
    int max_x = 0;
    int max_y = 0;
    for(const auto& point: points){
        max_x = std::max(max_x, point.x);
        max_y = std::max(max_y, point.y);
    }

    std::vector<std::vector<char>> matrix(max_y+1, std::vector<char>(max_x+1, '.'));
    
    for(const auto& point: points){
        matrix[point.y][point.x] = '1';
    }

    for(const auto& row: matrix){
        for(const auto& cell: row){
            std::cout << cell;
        }
        std::cout << "\n";
    }


}


struct ParseOutput {
    std::vector<Point2D> points;
    
    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {

        stream << "--- Points: ---\n";
        for(const auto& point: output.points){
            stream << point << "\n";
        }
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){

    std::vector<Point2D> points;
    for(const auto& line: lines){
        const auto coords_str = utils::split(line, ',');

        if(coords_str.size() != 2){
            throw;
        }

        int x = std::stoi(coords_str.at(0));
        int y = std::stoi(coords_str.at(1));

        points.push_back({x, y});
    }
    
    return {points};
}

L first_part(const std::vector<Point2D>& points) {

    L area = 0;
    for(const auto& P: points){
        for(const auto& Q: points){
            area = std::max(area, Point2D::rectangle_area(Q, P));
        }
    }

    return area;
}




struct Edge {
    // int x0, x1, y0, y1;

    Point2D from, to, direction;
    // enum class Direction {
    //     Horizontal, 
    //     Vertical
    // };
    
    // Direction direction;

    friend std::ostream& operator<<(std::ostream& stream, const Edge& edge){
        stream << "(" << edge.from << " -> " << edge.to << "} via " << edge.direction;
        return stream;
    }
};

// int cross(const Point2D& P, const Point2D& Q){
//     return P.x*Q.y - P.y*Q.x;
// }

Point2D rotate_90_clockwise(const Point2D& P){
    return {P.y, -P.x};
}

Point2D rotate_90_anticlockwise(const Point2D& P){
    return {-P.y, P.x};
}

std::vector<Edge> get_bounding_edges(const std::vector<Point2D>& points){

    // collect all the edges
    std::vector<Edge> edges;
    Point2D const * prev_point = &points.back();
    for(const auto& point: points){

        Point2D direction;
        direction.x = (prev_point->x == point.x) ? 0 : (point.x - prev_point->x)/std::abs(prev_point->x - point.x);
        direction.y = (prev_point->y == point.y) ? 0 : (point.y - prev_point->y)/std::abs(prev_point->y - point.y);

        const Edge edge{*prev_point, point, direction};

        edges.push_back(edge);
        prev_point = &point;
    }

    // Move the edges out so they bound the squares
    unsigned int N = edges.size();
    for(unsigned int i=0; i<N; i++){
        Edge& edge = edges[i];
        Edge& next_edge = edges[(i+1)%N];
        Edge& prev_edge = edges[(i-1)%N];

        // While traveling along the edges, left is always inside the polygon
        Point2D vector_inward = rotate_90_anticlockwise(edge.direction);
        Point2D next_vector_inward = rotate_90_anticlockwise(next_edge.direction);
        Point2D prev_vector_inward = rotate_90_anticlockwise(prev_edge.direction);
        
        edge.from = edge.from - vector_inward - prev_vector_inward;
        edge.to = edge.to - vector_inward - next_vector_inward;        

    }
    return edges;
}

struct EdgeIntersections {
    std::map<int, std::vector<int>> horizontal_edges_intersecting, vertical_edges_intersecting;

};

EdgeIntersections get_edge_intersections(const std::vector<Point2D>& points, const std::vector<Edge>& edges){

    std::map<int, std::vector<int>> horizontal_edges_intersecting, vertical_edges_intersecting;

    // For each point, I need to extend a line horizontally (vertically) and
    // keep a list of where it is intersected by vertical (horizontal) edges
    for(const auto& point: points){
        horizontal_edges_intersecting[point.x] = {};
        vertical_edges_intersecting[point.y] = {};
    }

    for(auto& [y, val]: vertical_edges_intersecting){
        for(const auto& edge: edges){

            if(edge.direction == Point2D{0,1} || edge.direction == Point2D{0,-1}){
                int ymax = std::max(edge.to.y, edge.from.y);
                int ymin = std::min(edge.to.y, edge.from.y);

                if(y > ymin && y < ymax){
                    val.push_back(edge.to.x);
                }
            }
        }
    }

    for(auto& [x, val]: horizontal_edges_intersecting){
        for(const auto& edge: edges){

            
            if(edge.direction == Point2D{1,0} || edge.direction == Point2D{-1,0}){
                int xmax = std::max(edge.to.x, edge.from.x);
                int xmin = std::min(edge.to.x, edge.from.x);

                if(x > xmin && x < xmax){
                    val.push_back(edge.to.y);
                }
            }

        }
    }

    std::cout << "vertical_edges_intersecting:\n";
    for(const auto& [y, isects]: vertical_edges_intersecting){
        std::cout << "y=" << y << ": ";
        for(const auto& x: isects){
            std::cout << x << " ";
        }
        std::cout << "\n";
    }

    std::cout << "horizontal_edges_intersecting:\n";
    for(const auto& [x, isects]: horizontal_edges_intersecting){
        std::cout << "x=" << x << ": ";
        for(const auto& y: isects){
            std::cout << y << " ";
        }
        std::cout << "\n";
    }

    return {horizontal_edges_intersecting, vertical_edges_intersecting};
}

L second_part(const std::vector<Point2D>& points_orig) {

    // points: x2 + 1
    std::vector<Point2D> points;
    for(const auto& point: points_orig){
        points.push_back({point.x*2+1, point.y*2+1});
    }

    const auto edges = get_bounding_edges(points);

    for(const auto& edge: edges){
        std::cout << "edge: " << edge << "\n"; 
    }

    const auto [horizontal_edges_intersecting, vertical_edges_intersecting] = get_edge_intersections(points, edges);

    auto has_numbers_in_range = [](const std::vector<int> numbers, int x, int y){
        // Checks if the vector has any number in the range [x,y] (inclusive)
        
        // Limits are not included deliberately
        for(const auto& number: numbers){
            if(number > x && number < y){
                return true;
            }
        }

        return false;
    };


    L max_area = 0;
    for(unsigned int i=0; i<points.size(); i++){
        const auto& P = points.at(i);
        for(unsigned int j=i+1; j<points.size(); j++){
            const auto& Q = points.at(j);
            
            int xi = std::min(P.x, Q.x);
            int xf = std::max(P.x, Q.x);
            int yi = std::min(P.y, Q.y);
            int yf = std::max(P.y, Q.y);

            std::cout << "considering pair: " << i << "," << j << ": " << P << "->" << Q << ": ";
            
            // Check if there are any intersections at y = yi from x = xi to x = xf
            if(has_numbers_in_range(vertical_edges_intersecting.at(yi), xi, xf) ||
               has_numbers_in_range(vertical_edges_intersecting.at(yf), xi, xf) ||
               has_numbers_in_range(horizontal_edges_intersecting.at(xi), yi, yf) ||
               has_numbers_in_range(horizontal_edges_intersecting.at(xf), yi, yf)
            ) {
                std::cout << "nope\n";
                continue;
            }

            L area = L(xf - xi + 2)*L(yf - yi + 2)/4;
            max_area = std::max(max_area, area);
            std::cout << "yup. Area " << area << "\n";
        }
    }




    return max_area;
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Not enough arguments. Exiting.\n";
        return 1;
    }
    auto lines = utils::parse_input(argv[1]);
    const auto output = parse(lines);
    //std::cout << output << "\n";

    // L count = first_part(output.points); // 4777409595
    // std::cout << "Count: " << count << "\n";

    // print_points(output.points);

    L count2 = second_part(output.points);
    std::cout << "Count: " << count2 << "\n";

    // 2344682574 too high
    // 1471191363 too low
    // 1473551379
}