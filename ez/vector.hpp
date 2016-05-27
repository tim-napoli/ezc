#ifndef _ez_vector_hpp_
#define _ez_vector_hpp_

#include <vector>

namespace ez {

template <typename T>
class vector : private std::vector<T> {
  public:
    vector() : std::vector<T>() {

    }

    virtual ~vector() {

    }

    T& at(unsigned int n) {
        return std::vector<T>::at(n);
    }

    const T& at(unsigned int n) const {
        return std::vector<T>::at(n);
    }

    void push(const T& v) {
        std::vector<T>::push_back(v);
    }

    void pop() {
        std::vector<T>::pop_back();
    }

    void insert(unsigned int n, const T& v) {
        std::vector<T>::insert(n, v);
    }

    void remove(unsigned int n) {
        std::vector<T>::erase(n);
    }

    unsigned int size() const {
        return std::vector<T>::size();
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const vector<T>& v) {
    os << "[";
    for (unsigned int i = 0; i < v.size(); i++) {
        os << v.at(i);
        if (i + 1 < v.size()) {
            os << ", ";
        }
    }
    os << "]";
}

}

#endif

