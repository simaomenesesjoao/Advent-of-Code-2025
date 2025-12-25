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

using L = unsigned long long;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& s) {
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


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        os << vec[i];
        if (i + 1 < vec.size())
            os << ", ";
    }
    os << "]";
    return os;
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

struct ParseOutput {
    unsigned int start;
    std::vector<std::vector<unsigned int>> carret_rows;

    void print() const {

        std::cout << "Start index: " << start << "\n";
        std::cout << "Carret rows;\n";
        for(const auto& row: carret_rows){
            for(const auto& num: row){
                std::cout << num << " ";
            }
            std::cout << "\n";
        }
    }
};

ParseOutput parse(const std::vector<std::string>& lines){
    // First find the position of the S
    unsigned int index = lines.front().find('S');    

    std::vector<std::vector<unsigned int>> carret_rows;
    for(unsigned int i=2; i<lines.size(); i+= 2){

        std::vector<unsigned int> carrets;
        const auto line = lines.at(i);
        for(unsigned int j=0; j<line.size(); j++){
            if(line.at(j) == '^'){
                carrets.push_back(j);
            }
        }
        carret_rows.push_back(carrets);
    }

    return {index, carret_rows};
}

L first_part(const std::vector<std::vector<unsigned int>>& carret_rows, unsigned int start_index){

    std::set<unsigned int> beams{start_index};

    unsigned int count = 0;
    for(const auto& carret_row: carret_rows){

        // If any of the beams hits any of the carrets, it gets split
        std::set<unsigned int> activated_carrets;
        std::set_intersection(carret_row.begin(), carret_row.end(), 
                              beams.begin(), beams.end(), 
                              std::inserter(activated_carrets, activated_carrets.end()));

        count += activated_carrets.size();

        std::set<unsigned int> new_beams;
        for(const auto& carret: activated_carrets){
            new_beams.insert(carret+1);
            new_beams.insert(carret-1);
        }

        // If any of the beams hits no carret, it continues
        std::set<unsigned int> free_beams;
        std::set_difference(beams.begin(), beams.end(), 
                            carret_row.begin(), carret_row.end(), 
                            std::inserter(free_beams, free_beams.end()));



        beams = new_beams;
        beams.insert(free_beams.begin(), free_beams.end());
    }
    return count;
}

struct Node {
    Node *left_child, *right_child;
    L count;
    L i, j;
};

template <typename T>
bool is_in(const T& container, unsigned int value){
    auto it = std::find(container.begin(), container.end(), value);
    return it != container.end();
}

L num_paths(Node* node){
    if(!node){
        return 1;
    }
    if(node->count > 0){
        return node->count;
    }

    node->count = num_paths(node->left_child) + num_paths(node->right_child);
    return node->count;
}

L second_part(const std::vector<std::vector<unsigned int>>& carret_rows, unsigned int start_index){

    unsigned int N = carret_rows.size();

    // Create all the DAG nodes
    std::map<std::pair<L, L>, Node> node_map;
    for(unsigned int i=0; i<N; i++){
        const auto& carret_row = carret_rows[i];
        for(const auto& j: carret_row){
            node_map[{i,j}] = Node{nullptr, nullptr, 0, i, j};
        }        
    }

    // For each node, iterate over all the lower carret rows to check if any
    // is in the adjacent columns
    for( auto& [coordinates, node]: node_map){
        const auto& [row_index, col_index] = coordinates;
        
        // Try to find the left child
        for(unsigned int i=row_index+1; i<N; i++){
            const auto& carret_row = carret_rows[i];

            
            if(is_in(carret_row, col_index+1)){
                
                node.left_child = &node_map[{i, col_index+1}];
                break;
            }
        }

        // Try to find the right child
        for(unsigned int i=row_index+1; i<N; i++){
            const auto& carret_row = carret_rows[i];

            if(is_in(carret_row, col_index-1)){
                node.right_child = &node_map[{i, col_index-1}];
                break;
            }
        }
    }

    // Find the root node:
    Node* root = &node_map[{0,start_index}];


    return num_paths(root);
}

int main(){
    auto lines = parse_input("input.txt");
    const auto output = parse(lines);

    L count = first_part(output.carret_rows, output.start);
    std::cout << "Count: " << count << "\n";


    L count2 = second_part(output.carret_rows, output.start);
    std::cout << "Count: " << count2 << "\n";

}