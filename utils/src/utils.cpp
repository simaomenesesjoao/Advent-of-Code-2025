#include "utils/utils.h"
#include <sstream>
#include <cassert>
#include <fstream>
#include <iostream>

namespace utils {

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

}
