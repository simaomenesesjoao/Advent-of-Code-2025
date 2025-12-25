#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

std::vector<std::string> split(const std::string& input, char delimiter){

    std::stringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while(std::getline(ss, token, delimiter)){
        tokens.push_back(token);
    }

    return tokens;
}

struct Range {
    long long int first, second;
};


std::vector<Range> parse_input(const std::string& filename){

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return {};
    }

    std::vector<Range> range_ints;
    std::string line;

    while(std::getline(file, line)) {

        if(line.empty()){
            continue;
        }

        std::vector<std::string> ranges = split(line, ',');

        for(const auto& range: ranges){
            const auto split_range = split(range, '-');

            std::string first = split_range.at(0);
            std::string second = split_range.at(1);

            range_ints.push_back({std::stol(first), std::stol(second)});
        }
    }

    return range_ints;
}


std::map<unsigned int, std::vector<unsigned int>> divisors{
    {1,  {}}, 
    {2,  {1}}, 
    {3,  {1}}, 
    {4,  {1,2}}, 
    {5,  {1}}, 
    {6,  {1,2,3}},
    {7,  {1}}, 
    {8,  {1,2,4}},
    {9,  {1,3}}, 
    {10, {1,2,5}}, 
    {11, {1}},
    {12, {1,2,3,4,6}}, 
    {13, {1}}, 
    {14, {1,7}}, 
    {15, {1,3,5}}
};

bool has_two_repeated(long long int i){

    const std::string s = std::to_string(i);
    int n = s.size();
    if(n % 2 != 0){
        return false;
    }
    
    const auto first = s.substr(0, n/2);
    const auto second = s.substr(n/2, n/2);
    return first == second;
}

bool has_n_repetition(const std::string& s, unsigned int n){
    //std::cout << "Has n repetition " << s << " " << n << "\n";

    unsigned N = s.size();
    assert(N % n == 0);

    std::string first = s.substr(0, n);

    for(unsigned int i=0; i<N; i += n){
        //std::cout << "i, n:  " << i << " " << n << " ";
        std::string other = s.substr(i, n);
        //std::cout << "comparing " << first << " to " << other << "\n";

        if(first != other){
            return false;
        }
        
    }

    return true;
}

bool has_repetition(const std::string& s){
    unsigned n = s.size();
    for(int div: divisors[n]){
        
        if(has_n_repetition(s, div)){
            //std::cout << "Has repetition\n";
            return true;
        }    
        
    }
    //std::cout << "Has no repetition\n";
    return false;
}

int main(){

    auto ranges = parse_input("input.txt");
    long long int sum = 0;
    for(const auto& [first, second]: ranges){
        std::cout << "Range: " << first << "-" << second << "\n";
        for(long long int i=first; i <= second; i++){
            if(has_repetition(std::to_string(i))){
                std::cout << i << " ";
                sum += i;
            }
        }
        std::cout << "\n";        
    }
    std::cout << "sum: " << sum << "\n";
}