#include <cassert>
#include <list>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>

using L = unsigned long long;

std::vector<std::string> split(const std::string& input, char delimiter){

    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while(std::getline(ss, token, delimiter)){
        if(token.empty()){
            continue;
        }
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

enum class Operation: int {
    Mult = 0, 
    Add = 1
};

struct ParseOutput {
    std::vector<std::vector<int>> numbers;
    std::vector<Operation> operations;
};

ParseOutput parse(const std::vector<std::string>& lines){

    std::vector<std::vector<int>> numbers;

    for(unsigned int i=0; i<lines.size()-1; i++){
        const auto& line = lines.at(i);

        std::vector<int> row;
        const auto list = split(line, ' ');
        for(const auto& l: list){
            row.push_back(std::stoi(l));
        }
        numbers.push_back(row);
    }

    const auto& last_line = lines.back();
    std::vector<Operation> operations;
    const auto list = split(last_line, ' ');
    for(const auto& l: list){
        if(l == "*"){
            operations.push_back(Operation::Mult);
        } else if (l == "+") {
            operations.push_back(Operation::Add);
        }
    }
    
    return {numbers, operations};
}

L first_part(const std::vector<std::vector<int>>& numbers, 
             const std::vector<Operation>& operations){

    unsigned int N = operations.size();
    L count = 0;

    for(unsigned int i=0; i<N; i++){
        

        if(operations.at(i) == Operation::Add){

            L sum = 0;
            for(const auto& row: numbers){
                sum += row.at(i);
            }

            count += sum;

        } else {

            L prod = 1;
            for(const auto& row: numbers){
                prod *= row.at(i);
            }

            count += prod;

        }

    }

    return count;
}


L second_part(const std::vector<std::vector<int>>& numbers, 
             const std::vector<Operation>& operations){

    unsigned int N = operations.size();
    L count = 0;

    for(unsigned int i=0; i<N; i++){
        

        if(operations.at(i) == Operation::Add){

            L sum = 0;
            for(const auto& num: numbers.at(i)){
                sum += num;
            }

            count += sum;

        } else {

            L prod = 1;
            for(const auto& num: numbers.at(i)){
                prod *= num;
            }

            count += prod;

        }

    }

    return count;
}


ParseOutput parse2(const std::vector<std::string>& lines){

    unsigned long max_string_size = 0;
    for(const auto& line: lines){
        std::cout << line.size() << " " << line << "\n";
        max_string_size = std::max(max_string_size, line.size());
    }


    // Operation op;
    std::vector<std::vector<int>> numbers;
    std::vector<int> number_buffer;

    for(unsigned int i=0; i<max_string_size; i++){

        // Find the number
        std::string num;
        for(unsigned int j=0; j<lines.size()-1; j++){
            const auto& line = lines.at(j);
            char c = line.at(i);
            if(i >= line.size() || c == ' '){
            } else {
                num += c;
            }
        }

        std::cout << num << " " << "\n";
        if(num == ""){
            std::cout << "clear buffer\n";
            numbers.push_back(number_buffer);
            number_buffer.clear();
        } else {
            number_buffer.push_back(std::stoi(num));
        }
    }
    if(!number_buffer.empty()){

        numbers.push_back(number_buffer);
        number_buffer.clear();

    }

    for(const auto& row: numbers){
        for(const auto& num: row){
            std::cout << num << " ";
        }
        std::cout << "\n";
    }

    const auto& last_line = lines.back();
    std::vector<Operation> operations;
    const auto list = split(last_line, ' ');
    for(const auto& l: list){
        if(l == "*"){
            operations.push_back(Operation::Mult);
        } else if (l == "+") {
            operations.push_back(Operation::Add);
        }
    }
    
    return {numbers, operations};
}

int main(){
    auto lines = parse_input("input.txt");
    const auto output = parse(lines);

    L count = first_part(output.numbers, output.operations);
    std::cout << "Count: " << count << "\n";

    const auto output2 = parse2(lines);


    std::cout << "numbers:\n";
    for(const auto& row: output2.numbers){
        for(const auto& num: row){
            std::cout << num << " ";
        }
        std::cout << "\n";
    }

    std::cout << "op:\n";
    for(const auto& op: output2.operations){
        std::cout << (op == Operation::Add) << " ";
    }
    std::cout << "\n";

    L count2 = second_part(output2.numbers, output2.operations);
    std::cout << "Count: " << count2 << "\n";


}