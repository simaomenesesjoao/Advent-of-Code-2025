#include <optional>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include "utils/utils.h"

using Map = std::unordered_map<std::string, std::vector<std::string>>;

struct ParseOutput {
    
    Map map;

    friend std::ostream& operator<<(std::ostream& stream, const ParseOutput& output) {
        stream << output.map << "\n";
        return stream;
    }
};

ParseOutput parse(const std::vector<std::string>& lines){

    ParseOutput output;
    for(const auto& line: lines){
        // line -> aaa: you hhh

        // key_val vector -> { aaa, you hhh }
        const auto key_val = utils::split(line, ':');
        
        const std::string key = key_val.at(0); // aaa
        const std::string val = key_val.at(1); //  you hhh (the extra space is expected)
        
        auto items = utils::split(val, ' ');
        // items -> {, you, hhh}
        items.erase(items.begin()); // remove the empty element 

        output.map[key] = items;
    }
    output.map["out"] = {};

    return output;
}

using PathMap = std::unordered_map<std::string, std::optional<unsigned int>>;

L num_paths(const std::string& key, PathMap& num_paths_to_out, const Map& map){
    // std::cout << "Entering num_paths for key " << key << "\n";

    const auto maybe_num_paths = num_paths_to_out.at(key);
    if(maybe_num_paths){
        return *maybe_num_paths;
    } 

    const auto& children = map.at(key); 
    L path_sum = 0;
    for(const auto& child: children){
        path_sum += num_paths(child, num_paths_to_out, map);
    }
    num_paths_to_out[key] = path_sum;
    return path_sum;    
}

L num_paths_from_to(const std::string& from, const std::string& to, const Map& map){

    PathMap paths_to_final;
    for(const auto& [node_str, outputs]: map){
        paths_to_final[node_str] = std::nullopt;
    }
    paths_to_final[to] = 1;

    return num_paths(from, paths_to_final, map);
}

L first_part(const Map& map) {
    return num_paths_from_to("you", "out", map);
}

L second_part(const Map& map) {
    // We need to find all the paths that go from svr to out, passing by fft and dac
    // Since this is a DAG, the order must be svr -> fft -> dac -> out

    return num_paths_from_to("svr", "fft", map)*
            num_paths_from_to("fft", "dac", map)*
            num_paths_from_to("dac", "out", map);
}

int main(int argc, char** argv){
    if(argc != 2){
        std::cerr << "Not enough arguments. Exiting.\n";
        return 1;
    }

    auto lines = utils::parse_input(argv[1]);
    const auto output = parse(lines);
    std::cout << output << "\n";

    L count = first_part(output.map);
    std::cout << "Count: " << count << "\n";

    L count2 = second_part(output.map);
    std::cout << "Count: " << count2 << "\n"; 

}