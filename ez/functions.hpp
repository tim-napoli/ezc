#ifndef _ez_functions_hpp_
#define _ez_functions_hpp_

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <cmath>

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

double sqrt(double v) {
    return std::sqrt(v);
}


typedef std::FILE* File;

void open_file(const std::string& path,
               const std::string& accessor,
               File& file)
{
    file = fopen(path.c_str(), accessor.c_str());
}

bool is_file_open(File file) {
    return file != NULL;
}

void close_file(File& file) {
    if (is_file_open(file)) {
        fclose(file);
        file = NULL;
    }
}

std::string read_line_from_file(File file) {
    static char buf[4096];
    fgets(buf, sizeof(buf) - 1, file);
    return std::string(buf);
}

bool is_file_over(File file) {
    return feof(file);
}

}

#endif

