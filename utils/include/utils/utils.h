#include <vector>
#include <ostream>
#include <string>

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

namespace utils {
    std::vector<std::string> split(const std::string& input, char delimiter);
    std::vector<std::string> parse_input(const std::string& filename);
}
