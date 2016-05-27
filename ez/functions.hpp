#ifndef _ez_functions_hpp_
#define _ez_functions_hpp_

namespace ez {

int random(int start, int stop) {
    return start + std::rand() % (stop - start);
}

}

#endif

