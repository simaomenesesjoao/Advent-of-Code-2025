#include <fstream>
#include <string>
#include <vector>
#include <iostream>

std::vector<int> parse_input(const std::string& filename){

    std::ifstream file(filename);
    std::string line;
    std::vector<int> output;

    while(std::getline(file, line)) {
        if(line.empty()){
            continue;
        }
        char first_char = line[0];
        int value = std::stoi(line.substr(1));

        switch(first_char){
            case 'L':
                output.push_back(-value);
                break;
            case 'R':
                output.push_back(value);
                break;
            default:
                std::cout << "Should not get here!\n";
        }
    }
    return output;
}

int main(){

    // auto input = parse_input("input_small.txt");
    auto input = parse_input("input.txt");
    {
        int start = 50;
        unsigned int counter = 0;
        for(const auto& v: input){
            start = (start + v)%100;
            if(start == 0){
                counter++;
            }
        }
        std::cout << "Total count: " << counter << "\n";
    }

    {
        int start = 50;
        unsigned int counter = 0;
        bool prev_was_zero = false;
        for(int v: input){

            int before = start;
            start += v;
            std::cout << counter << " " << before << " " << v << " " << start << " ";


            if(start > 0){

                while(start > 99){
                    std::cout << "Large ";
                    counter++;
                    start -= 100;
                }

            } else if(start < 0){

                while(start < 0){
                    std::cout << "Negative ";
                    counter++;
                    start += 100;
                }

                if(start == 0){
                    counter++;
                }
                if(prev_was_zero){
                    counter--;
                }

            } else {
                counter++;v
                std::cout << "Zero ";
            }

            if(start == 0){
                prev_was_zero = true;
            } else {
                prev_was_zero = false;
            }


            std::cout << "\n";
        }
        std::cout << "Total count: " << counter << "\n";
    }

}