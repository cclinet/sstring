#include "sstring.h"
#include <iostream>

int main() {
    ss::string ss{"abc"};
    std::cout << ss << std::endl;
    ss::string ms{"abcdefghijklmnopqrstuvwxy"};
    std::cout << ms << std::endl;
    ss::string ls{"abcdefghijklmnopqrstuvwxyz"};
    std::cout << ls << std::endl;
    return 0;
}
