#ifndef _ez_functions_hpp_
#define _ez_functions_hpp_

#include <cstdlib>
#include <ctime>

namespace ez {

int random(int start, int stop) {
    return start + std::rand() % (stop - start);
}

void set_random_seed(int seed) {
    srand(seed);
}

int integer_from_string(const std::string& str) {
    return atoi(str.c_str());
}

int get_timestamp() {
    return time(NULL);
}

}

#endif

