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

// template <template <typename T> class Container, typename T>
// std::ostream& operator<<(std::ostream& os, const Container<T>& s) {
//     os << "{";
//     auto it = s.begin();
//     while (it != s.end()) {
//         os << *it;
//         if (++it != s.end())
//             os << ", ";
//     }
//     os << "}";
//     return os;
// }

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
    int x, y, z;

    static L distance_sq(const Point& P, const Point& Q){
        int dx = P.x - Q.x;
        int dy = P.y - Q.y;
        int dz = P.z - Q.z;
        return L(dx)*dx + L(dy)*dy + L(dz)*dz;
    }

    friend std::ostream& operator<<(std::ostream& stream, const Point& P) {
        stream << "(" << P.x << ", " << P.y << ", " << P.z << ")";
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

        if(coords_str.size() != 3){
            throw;
        }

        int x = std::stoi(coords_str.at(0));
        int y = std::stoi(coords_str.at(1));
        int z = std::stoi(coords_str.at(2));

        points.push_back({x, y, z});
    }
    
    return {points};
}

struct Pair {
    unsigned int i, j;
    L distance;
};


class DisjointSets {
public:
    DisjointSets(unsigned int N): 
        parents{std::vector<int>(N, 0)}, 
        num_sets{N} {

        for(unsigned int i=0; i<N; i++){
            parents[i] = i;
        }

    }

    int find(int n) const {
        if(parents[n] == n){
            return n;
        } else {
            return find(parents[n]);
        }
    }

    void join(int n, int m){
        int fn = find(n);
        int fm = find(m);
        if(fn != fm){
            num_sets--;
        }
        parents[fn] = fm;
    }


    using Map = std::unordered_map<int, std::vector<int>>;

    Map get_sets() const {

        Map map;
        
        for(unsigned int i=0; i<parents.size(); i++){
            map[find(i)].push_back(i);
        }

        return map;        
    }

    unsigned int get_num_sets(){
        return num_sets;
    }

    friend std::ostream& operator<<(std::ostream& stream, const DisjointSets& sets){

        stream << "Parents: ";
        for(int parent: sets.parents){
            stream << parent << " ";
        }
        stream << "\n";

        const auto map = sets.get_sets();
        for(const auto& [set_representative, set_elements]: map){
            stream << set_representative << ": ";
            for(const auto& element: set_elements){
                stream << element << " ";
            }
            stream << "\n";
        }
        return stream;
            
    }

private:
    std::vector<int> parents;
    unsigned int num_sets;
};


L first_part(const std::vector<Point>& points, unsigned int num_connections) {
    
    const auto cmp = [](const Pair& a, const Pair& b){
        return a.distance > b.distance;
    };

    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pairs(cmp);

    for(unsigned int i=0; i<points.size(); i++){

        const auto& point = points.at(i);
        for(unsigned int j=i+1; j<points.size(); j++){

            const auto& neigh = points.at(j);
            L dist = Point::distance_sq(point, neigh);
            pairs.push({i, j, dist});
        }
    }

    DisjointSets sets(points.size());

    unsigned int counter = 0;
    while (!pairs.empty() && counter++ < num_connections) {
        const auto& pair = pairs.top();
        sets.join(pair.i, pair.j);
        pairs.pop();
    }

    const auto map = sets.get_sets();
    std::vector<int> set_sizes;
    for(const auto& [set_representative, set_elements]: map){
        set_sizes.push_back(set_elements.size());
    }
    std::sort(set_sizes.rbegin(), set_sizes.rend());

    L count = set_sizes[0]*set_sizes[1]*set_sizes[2];


    
    return count;
}


L second_part(const std::vector<Point>& points) {
    
    const auto cmp = [](const Pair& a, const Pair& b){
        return a.distance > b.distance;
    };

    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pairs(cmp);

    for(unsigned int i=0; i<points.size(); i++){

        const auto& point = points.at(i);
        for(unsigned int j=i+1; j<points.size(); j++){

            const auto& neigh = points.at(j);
            L dist = Point::distance_sq(point, neigh);
            pairs.push({i, j, dist});
        }
    }

    DisjointSets sets(points.size());

    while (!pairs.empty()) {
        const auto& pair = pairs.top();
        unsigned int i = pair.i;
        unsigned int j = pair.j;
        sets.join(i, j);
        pairs.pop();
        if(sets.get_num_sets() == 1){
            return L(points[i].x)*points[j].x;
        }
    }



    
    return 0;
}

int main(){
    auto lines = parse_input("input.txt");
    const auto output = parse(lines);
    // std::cout << output << "\n";

    L count = first_part(output.points, 1000);
    std::cout << "Count: " << count << "\n";

    L count2 = second_part(output.points);
    std::cout << "Count: " << count2 << "\n";

}