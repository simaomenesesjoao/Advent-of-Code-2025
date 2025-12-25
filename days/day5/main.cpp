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

struct Range {
    unsigned long long start, end;
};


struct ExclusiveRange {
    // includes start, does not include end
    unsigned long long start, end;
};


struct ParseOutput {
    std::vector<Range> ranges;
    std::vector<unsigned long long> indices;

    void print(){
        std::cout << "Ranges:\n";
        for(const auto& range: ranges){
            std::cout << range.start << "-" << range.end << "\n";
        }

        std::cout << "Food indices:\n";
        for(const auto& index: indices){
            std::cout << index << " ";
        }
        std::cout << "\n";
    }
};

ParseOutput parse(const std::vector<std::string>& lines){


    std::vector<Range> ranges;
    std::vector<unsigned long long> indices;

    for(const auto& line: lines){

        std::size_t index = line.find("-");

        if(index != std::string::npos){
            std::string first = line.substr(0, index);
            std::string second = line.substr(index+1, line.size() - index);
            ranges.push_back({std::stoul(first), std::stoul(second)});
            continue;
        }

        if(line.size() == 0){
            continue;
        }

        indices.push_back(std::stol(line));        
    }

    return {ranges, indices};   

}

unsigned int first_part(const std::vector<Range>& ranges, const std::vector<unsigned long long>& indices){
    unsigned int count = 0;

    for(const auto& index: indices){
        for(const auto& range: ranges){
            if(index >= range.start && index <= range.end){
                count++;
                break;
            }
        }
    }
    return count;
}

L second_part(const std::vector<Range>& ranges){
    L count = 0;

    std::vector<ExclusiveRange> sorted_ranges;
    for(const auto& range: ranges){
        sorted_ranges.push_back({range.start, range.end+1});
    }

    std::sort(sorted_ranges.begin(), sorted_ranges.end(), 
            [](const ExclusiveRange& a, const ExclusiveRange& b){
                return a.start < b.start;
            });

    L start, end;
    start = 0;
    end = 0;

    for(const auto& range: sorted_ranges){
        
        // No intersection, not contiguous
        if(range.start > end){
            count += end - start;
            start = range.start;
        } 

        end = std::max(end, range.end);
    }

    count += end - start;
    return count;
}

int main(){
    auto lines = parse_input("input.txt");
    const auto output = parse(lines);

    unsigned int count = first_part(output.ranges, output.indices);
    std::cout << "Count: " << count << "\n";

    L count2 = second_part(output.ranges);
    std::cout << "Count2: " << count2 << "\n";
}