#ifndef _ez_optional_hpp_
#define _ez_optional_hpp_

#include <cstdlib>
#include <cstdio>

namespace ez {

template <typename T>
class optional {
  private:
    int* _refcount;
    T* _value;

  public:
    optional() {
        _refcount = new int;
        *_refcount = 0;
        _value = NULL;
    }

    optional(const T& value) {
        _refcount = new int;
        *_refcount = 0;
        set(value);
    }

    optional(const optional<T>& opt) {
        _refcount = opt._refcount;
        (*_refcount)++;
        _value = opt._value;
    }

    ~optional() {
        if (*_refcount == 0 && _value) {
            delete _value;
            delete _refcount;
        } else {
            (*_refcount)--;
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

