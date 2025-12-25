#include <cassert>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <map>

std::vector<std::vector<unsigned short>> parse_input(const std::string& filename){

    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        return {};
    }

    std::vector<std::vector<unsigned short>> banks;
    std::string line;

    while(std::getline(file, line)) {
        if(line.empty()){
            continue;
        }
        std::vector<unsigned short> bank;
        for(char battery: line){
            bank.push_back(battery - '0');
        }
        banks.push_back(bank);        
    }



    return banks;
}

//using RevIt = std::vector<unsigned short>::const_reverse_iterator;

template <typename RevIt>
RevIt get_largest_from_to(RevIt it_from, RevIt it_to){

    auto digit_it = it_from;
    unsigned short digit = *digit_it;

    for(auto it = it_from; it != it_to; it++){
        if(*it >= digit){
            digit_it = it;
            digit = *it;
        }
    }

    return digit_it;
}


unsigned long first_part(const std::vector<std::vector<unsigned short>>& banks){

    unsigned long total_joltage = 0;

    for(const auto& bank: banks){

        auto first_digit_it = get_largest_from_to(bank.rbegin() + 1, bank.rend());
        auto second_digit_it = get_largest_from_to(bank.rbegin() + 0, first_digit_it);

        auto first_digit = *first_digit_it;
        auto second_digit = *second_digit_it;

        short joltage = first_digit*10 + second_digit;
        total_joltage += joltage;
    }
    return total_joltage;
}

unsigned long second_part(const std::vector<std::vector<unsigned short>>& banks){

    unsigned long total_joltage = 0;

    for(const auto& bank: banks){

        unsigned long joltage = 0;
        auto it = bank.rend();

        for(short i = 11; i >= 0; i--){
            it = get_largest_from_to(bank.rbegin() + i, it);
            joltage = joltage*10 + *it;            
        }

        total_joltage += joltage;
        
    }
    return total_joltage;

}

int main(){

    auto banks = parse_input("input.txt");
    unsigned int total_joltage = first_part(banks);
    std::cout << "Total joltage: " << total_joltage << "\n";    

    unsigned long total_joltage2 = second_part(banks);
    std::cout << "Total joltage: " << total_joltage2 << "\n";   
    
}