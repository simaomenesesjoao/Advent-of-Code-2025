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

using L = unsigned long long;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
    os << "{";
    auto it = s.begin();
    while (it != s.end()) {
        os << *it;
        if (++it != s.end())
            os << ", ";
    }
    os << "}";
    return os;
}

std::vector<std::string> split(const std::string& input, char delimiter){

    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while(std::getline(ss, token, delimiter)){
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> parse_input(const std::string& filename){

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return {};
    }

    std::string line;
    std::vector<std::string> lines;

    while(std::getline(file, line)) {
        lines.push_back(line);    
    }
    return lines;
}

struct Point {
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
        const auto coords_str = split(line, ',');

        if(coords_str.size() != 2){
            throw;
        }

        int x = std::stoi(coords_str.at(0));
        int y = std::stoi(coords_str.at(1));

        points.push_back({x, y});
    }
    
    return {points};
}




struct VerticalEdge {
    int x, bot_y, top_y;

    enum class Direction {
        Down, 
        Up        
    };
    
    Direction direction;

    bool is_valid() const {
        return bot_y < top_y;
    }


    friend std::ostream& operator<<(std::ostream& stream, const VerticalEdge& edge){
        stream << "(" << edge.x << "," << edge.bot_y << "," << edge.top_y;
        if(edge.direction == Direction::Up){
            stream << ", Up)";
        } else {
            stream << ", Down)";
        }
        return stream;
    }
};

std::vector<VerticalEdge> intersect(const VerticalEdge& rec_edge, const VerticalEdge& edge){
    std::vector<VerticalEdge> edges;

    // No intersection. Ex: [0,2[ + [2,3[ -> [0,2[
    if(edge.top_y <= rec_edge.bot_y || edge.bot_y >= rec_edge.top_y){
        std::cout << "No intersection\n";
        edges.push_back(rec_edge);

    // Intersection
    } else if(rec_edge.bot_y < edge.bot_y || rec_edge.top_y > edge.top_y) {
        VerticalEdge bot_edge{rec_edge.x, rec_edge.bot_y, edge.bot_y};
        VerticalEdge top_edge{rec_edge.x, edge.top_y, rec_edge.top_y};


        if(bot_edge.is_valid()){
            std::cout << "bot is valid: " << bot_edge << "\n";
            edges.push_back(bot_edge);
        }


        if(top_edge.is_valid()){
            std::cout << "top is valid: " << top_edge << "\n";
            edges.push_back(top_edge);
        }

    }

    return edges;
}

L first_part(const std::vector<Point>& points) {

    // Identify the vertical edges which go down and up
    std::vector<VerticalEdge> edges;

    Point const * prev_point = &points.back();
    for(const auto& point: points){
        if(point.x == prev_point->x){
            if(point.y > prev_point->y){
                edges.push_back({point.x, prev_point->y, point.y, VerticalEdge::Direction::Up});
            } else {
                edges.push_back({point.x, point.y, prev_point->y, VerticalEdge::Direction::Down});
            }
        }
        prev_point = &point;
    }

    std::cout << "Edges:\n" << edges << "\n";

    auto comparator = [](const VerticalEdge& e1, const VerticalEdge& e2){
        if(e1.x == e2.x){
            return e1.bot_y < e2.bot_y;
        }
        return e1.x < e2.x;
    };

    std::sort(edges.begin(), edges.end(), comparator);

    std::vector<VerticalEdge> rectangle_edges;

    L area = 0;
    for(const auto& edge: edges){
        std::cout << "---- Checking edge: " << edge << "\n";
        
        std::vector<VerticalEdge> new_rectangle_edges;
        // If the edge goes down, it starts new rectangles by extending existing edges
        if(edge.direction == VerticalEdge::Direction::Down){

            bool found = false;
            VerticalEdge new_edge{edge.x, edge.bot_y, edge.top_y};

            for(const auto& rectangle_edge: rectangle_edges){
                // std::cout << "Checking rectangle edge: " << rectangle_edge << "\n";
                new_rectangle_edges.push_back(rectangle_edge);

                if(edge.top_y == rectangle_edge.bot_y){
                    // std::cout << "Extending up\n";
                    new_edge.top_y = std::max(new_edge.top_y, rectangle_edge.top_y);
                }

                if(edge.bot_y == rectangle_edge.top_y){
                    // std::cout << "Extending down\n";
                    new_edge.bot_y = std::min(new_edge.bot_y, rectangle_edge.bot_y);
                }
            }
            new_rectangle_edges.push_back(new_edge);



        // If the edge goes up, it ends existing rectangles
        } else {

            for(const auto& rectangle_edge: rectangle_edges){
                std::cout << "Checking rectangle edge: " << rectangle_edge << "\n";

                // If it intersects the new edge, it can disappear, be clipped or split
                const auto split_edges = intersect(rectangle_edge, edge);
                new_rectangle_edges.insert(new_rectangle_edges.end(), split_edges.begin(), split_edges.end());


                L new_area = L(rectangle_edge.top_y - rectangle_edge.bot_y + 1)*(edge.x - rectangle_edge.x + 1);

                //L area = (edge.x - rectangle_edge.x + 1);
                std::cout << "area: " << new_area << "\n";
                area = std::max(area, new_area);

            }
        }

        std::cout << new_rectangle_edges << "\n";
        rectangle_edges = new_rectangle_edges;

    }
    return area;
}

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


int main(){
    auto lines = parse_input("input_small.txt");
    const auto output = parse(lines);
    std::cout << output << "\n";

    //print_points(output.points);
    

    L count = first_part(output.points);
    std::cout << "Count: " << count << "\n";

    // L count2 = second_part(output.points);
    // std::cout << "Count: " << count2 << "\n";

    // 2344682574 too high
    // 1471191363 too low

}