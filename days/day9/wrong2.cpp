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

    // static L distance_sq(const Point& P, const Point& Q){
    //     int dx = P.x - Q.x;
    //     int dy = P.y - Q.y;
    //     return L(dx)*dx + L(dy)*dy + L(dz)*dz;
    // }

    static L rectangle_area(const Point& P, const Point& Q){
        int dx = std::abs(P.x - Q.x) + 1;
        int dy = std::abs(P.y - Q.y) + 1;
        return L(dx)*dy;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Point& P) {
        stream << "(" << P.x << ", " << P.y << ")";
        return stream;
    }
};


void print_points(const std::vector<Point>& points){
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
    std::vector<Point> points;
    
    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {

        stream << "--- Points: ---\n";
        for(const auto& point: output.points){
            stream << point << "\n";
        }
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){

    std::vector<Point> points;
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

L first_part(const std::vector<Point>& points) {

    L area = 0;
    for(const auto& P: points){
        for(const auto& Q: points){
            area = std::max(area, Point::rectangle_area(Q, P));
        }
    }

    return area;
}




struct Edge {
    int x0, x1, y0, y1;

    enum class Direction {
        Horizontal, 
        Vertical
    };
    
    Direction direction;

    friend std::ostream& operator<<(std::ostream& stream, const Edge& edge){
        stream << "(" << edge.x0 << "," << edge.x1 << "," << edge.y0 << "," << edge.y1;
        if(edge.direction == Direction::Horizontal){
            stream << ", H";
        } else {
            stream << ", V";
        }
        stream << ")";
        return stream;
    }
};

L second_part(const std::vector<Point>& points) {

    std::vector<Edge> edges;

    Point const * prev_point = &points.back();
    for(const auto& point: points){

        const auto direction = (prev_point->y == point.y) ? Edge::Direction::Horizontal : Edge::Direction::Vertical;

        edges.push_back({prev_point->x, point.x, prev_point->y, point.y, direction});
        prev_point = &point;
    }

    std::cout << edges << "\n";


    // For each point, I need to extend a line horizontally (vertically) and
    // keep a list of where it is intersected by vertical (horizontal) edges
    std::map<int, std::vector<int>> horizontal_edges_intersecting, vertical_edges_intersecting;
    for(const auto& point: points){
        horizontal_edges_intersecting[point.x] = {};
        vertical_edges_intersecting[point.y] = {};
    }

    for(auto& [y, val]: vertical_edges_intersecting){
        for(const auto& edge: edges){

            if(edge.direction == Edge::Direction::Vertical){
                int ymax = std::max(edge.y0, edge.y1);
                int ymin = std::min(edge.y0, edge.y1);

                if(y > ymin && y < ymax){
                    val.push_back(edge.x0);
                }
            }

        }
    }


    for(auto& [x, val]: horizontal_edges_intersecting){
        for(const auto& edge: edges){

            if(edge.direction == Edge::Direction::Horizontal){
                int xmax = std::max(edge.x0, edge.x1);
                int xmin = std::min(edge.x0, edge.x1);

                if(x > xmin && x < xmax){
                    val.push_back(edge.y0);
                }
            }

        }
    }


    std::cout << "Horizontal edges intersecting:\n";
    for(const auto& [x, vals]: horizontal_edges_intersecting){
        std::cout << "x=" << x << ": ";
        for(const auto& val: vals){
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    std::cout << "Vertical edges intersecting:\n";
    for(const auto& [y, vals]: vertical_edges_intersecting){
        std::cout << "y=" << y << ": ";
        for(const auto& val: vals){
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

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


    auto amount_numbers_in_range = [](const std::vector<int> numbers, int x, int y){
        // Limits are not included deliberately
        int count = 0;
        for(const auto& number: numbers){
            if(number > x && number < y){
                count++;
            }
        }

        return count;
    };
    
    
    L max_area = 0;
    for(int i=0; i<points.size(); i++){
        const auto& P = points.at(i);
        for(int j=i+1; j<points.size(); j++){
            const auto& Q = points.at(j);
            
            int xi = std::min(P.x, Q.x);
            int xf = std::max(P.x, Q.x);
            int yi = std::min(P.y, Q.y);
            int yf = std::max(P.y, Q.y);

            std::cout << "considering pair: " << i << "," << j << ": " << P << "->" << Q << ": ";
            
            // Check if there are any intersections at y = yi from x = xi to x = xf
            if(has_numbers_in_range(vertical_edges_intersecting[yi], xi, xf) ||
               has_numbers_in_range(vertical_edges_intersecting[yf], xi, xf) ||
               has_numbers_in_range(horizontal_edges_intersecting[xi], yi, yf) ||
               has_numbers_in_range(horizontal_edges_intersecting[xf], yi, yf)
            ) {
                std::cout << "nope\n";
                continue;
            }

            // Check if the corners are inside the polygon
            std::vector<Point> corners{{xi,yi}, {xi,yf}, {xf,yi}, {xf,yf}};
            for(const auto& corner: corners){
                std::cout << "checking corner: " << corner << " ";
                int x = corner.x;
                int y = corner.y;
                std::cout << amount_numbers_in_range(vertical_edges_intersecting[y], -1, x) << " ";
                if(amount_numbers_in_range(vertical_edges_intersecting[y], -1, x) %2 != 0){
                    std::cout << "not inside\n";
                    continue;
                }
                std::cout << "inside\n";

            }

            L area = L(xf - xi + 1)*L(yf - yi + 1);
            max_area = std::max(max_area, area);
            std::cout << "yup. Area " << area << "\n";
        }
    }



    return max_area;
}

int main(){
    auto lines = utils::parse_input("input_small.txt");
    const auto output = parse(lines);
    //std::cout << output << "\n";

    // L count = first_part(output.points); // 4777409595
    // std::cout << "Count: " << count << "\n";

    print_points(output.points);

    L count2 = second_part(output.points);
    std::cout << "Count: " << count2 << "\n";

    // 2344682574 too high
    // 1471191363 too low
    // 1473551379 ?
}