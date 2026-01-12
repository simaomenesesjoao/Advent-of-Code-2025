#include <vector>
#include <ostream>
#include <string>
#include <optional>
#include <unordered_map>
#include <map>

using L = unsigned long long;

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s) {
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
std::ostream& operator<<(std::ostream& stream, const std::optional<T>& opt){
    if(opt){
        stream << *opt;
    } else {
        stream << "null";
    }
    return stream;
}


template <typename T, typename U>
std::ostream& operator<<(std::ostream& stream, const std::unordered_map<T, U>& map){
    for(const auto& [key, val]: map){
        stream << key << ": " << val << "\n";
    }
    return stream;
}



template <typename T, typename U>
std::ostream& operator<<(std::ostream& stream, const std::map<T, U>& map){
    for(const auto& [key, val]: map){
        stream << key << ": " << val << "\n";
    }
    return stream;
}


namespace utils {
    std::vector<std::string> split(const std::string& input, char delimiter);
    std::vector<std::string> parse_input(const std::string& filename);
}
