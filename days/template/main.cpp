#include <string>
#include <vector>
#include <iostream>
#include "utils/utils.h"

struct ParseOutput {
    
    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){
    return {};
}

L first_part() {
    return 0;
}


L second_part() {
    return 0;
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Not enough arguments. Exiting.\n";
        return 1;
    }
    auto lines = utils::parse_input(argv[1]);
    const auto output = parse(lines);
    std::cout << output << "\n";

    L count = first_part();
    std::cout << "Count: " << count << "\n";

    L count2 = second_part();
    std::cout << "Count: " << count2 << "\n";

}