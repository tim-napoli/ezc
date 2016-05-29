#ifndef _ez_optional_hpp_
#define _ez_optional_hpp_

#include <cstdlib>
#include <cstdio>

namespace ez {

template <typename T>
class optional {
  private:
    int _refcount;
    T* _value;

  public:
    optional() {
        _refcount = 0;
        _value = NULL;
    }

    optional(const optional<T>& opt) {
        _refcount = opt._refcount + 1;
        _value = opt._value;
    }

    ~optional() {
        if (!_refcount && _value) {
            delete _value;
        }
    }

    bool is_set() const {
        return _value != NULL;
    }

    void set(const T& value) {
        _value = new T;
        *_value = value;
    }

    const T& get() const {
        if (!is_set()) {
            fprintf(stderr, "accessing unset optional value\n");
            exit(EXIT_FAILURE);
        }
        return *_value;
    }

    T& get() {
        if (!is_set()) {
            fprintf(stderr, "accessing unset optional value\n");
            exit(EXIT_FAILURE);
        }
        return *_value;
    }
};

}

#endif

