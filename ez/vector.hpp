#ifndef _ez_vector_hpp_
#define _ez_vector_hpp_

#include <vector>
#include <functional>

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
        std::vector<T>::erase(std::vector<T>::begin() + n);
    }

    unsigned int size() const {
        return std::vector<T>::size();
    }

    void map(const std::function<void(T&)>& func) {
        for (int i = 0; i < size(); i++) {
            func(at(i));
        }
    }

    T reduce(const std::function<const T&(const T&, const T&)>& func,
             const T& initial_value) const
    {
        T current = initial_value;
        for (unsigned int i = 0; i < size(); i++) {
            current = func(at(i), current);
        }
        return current;
    }

    void filter(const std::function<bool(const T&)>& func) {
        for (unsigned int i = 0; i < size(); i++) {
            if (func(at(i))) {
                remove(i);
                i--;
            }
        }
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
    return os;
}

}

#endif

