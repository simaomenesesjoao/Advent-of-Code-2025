#include <optional>
//#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "utils/utils.h"
// #include "utils/lp.h"
// #include <Eigen/Dense>

struct Tree {
    unsigned int width, length;
    std::vector<unsigned int> quantity_of_presents;
    
    friend std::ostream& operator<<(std::ostream& stream, const Tree& tree) {
        stream << tree.length << "x" << tree.width << ": " << tree.quantity_of_presents;
        return stream;
    }

};

struct Present {
    std::vector<std::vector<bool>> bitmask;
    unsigned int area;

    friend std::ostream& operator<<(std::ostream& stream, const Present& present) {
        stream << present.bitmask;
        return stream;
    }

};

struct ParseOutput {
    
    std::vector<Tree> trees;
    std::map<unsigned int, Present> presents;

    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {
        stream << "Trees:\n" << output.trees << "\n";
        stream << "Presents:\n" << output.presents << "\n";
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){

    ParseOutput output;
    for(unsigned int i=0; i<lines.size(); i++){
        const auto line = lines.at(i);

        // Line with quantity of presents
        if(line.find('x') != std::string::npos){
            Tree tree;
            
            const auto left_right = utils::split(line, ':');
            const auto left = left_right.at(0);
            const auto right = left_right.at(1);

            const auto width_length = utils::split(left, 'x');
            tree.width = std::stoi(width_length.at(0));
            tree.length = std::stoi(width_length.at(1));

            auto quantities = utils::split(right, ' ');
            quantities.erase(quantities.begin());
            std::vector<unsigned int> quantity_of_presents;
            for(const auto& quantity: quantities){
                quantity_of_presents.push_back(std::stoi(quantity));
            }
            tree.quantity_of_presents = quantity_of_presents;

            // std::cout << "tree: " << tree << "\n";
            output.trees.push_back(tree);

        } else if(line.find(':') != std::string::npos){

            const auto number = std::stoi(utils::split(line, ':').at(0));
            // std::cout << line << " " << number << "\n";
            // std::cout << "good pattern\n";
            const auto line1 = lines.at(i+1);
            const auto line2 = lines.at(i+2);
            const auto line3 = lines.at(i+3);

            std::vector<std::string> gift_lines{line1, line2, line3};
            
            Present present;
            unsigned int area = 0;

            for(const auto& gift_line: gift_lines){
                std::vector<bool> bitline;
                for(const auto c: gift_line){
                    if(c == '#'){
                        bitline.push_back(true);
                        area++;
                    } else {
                        bitline.push_back(false);
                    }
                }
                present.bitmask.push_back(bitline);
                present.area = area;
            }
            // std::cout << "bitmask: " << present.bitmask << "\n";
            output.presents[number] = present;
        }        
        
    }

    return output;
}

L first_part(const std::vector<Tree>& trees, const std::map<unsigned int, Present>& presents) {
    L count = 0;
    for(const auto& tree: trees){
        int area = tree.width*tree.length;
        int present_bbox = 0;
        int present_area = 0;
        int usable_area = (tree.width - tree.width%3)*(tree.length - tree.length%3);

        for(unsigned int i=0; i<tree.quantity_of_presents.size(); i++){
            const auto& gift = tree.quantity_of_presents.at(i);
            present_bbox += gift*9;
            present_area += gift*presents.at(i).area;
        }
        
        std::cout << area << " " << usable_area << " " << present_area << " " << present_bbox;
        if(present_area > area){
            std::cout << " presents have too much area.\n";
        } else if(present_bbox <= usable_area) {
            std::cout << " presents fit without intersection.\n";
            count++;
        } else {
            std::cout << "\n";            
        }

        
    }
    return count;
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Not enough arguments. Exiting.\n";
        return 1;
    }

    auto lines = utils::parse_input(argv[1]);
    const auto output = parse(lines);
    std::cout << output << "\n";

    L count = first_part(output.trees, output.presents);
    std::cout << "Count: " << count << "\n";
}