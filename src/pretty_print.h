#pragma once

#include <iostream>
#include <vector>

namespace pretty_print {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
    os << "[";
    for (auto& e : vec) {
        os << e << ", ";
    }
    os << "]";
    return os;
}


}