#ifndef LWE_ENUM_HEADER
#define LWE_ENUM_HEADER

#include "hal.hpp"

template<typename E> E evalue(size_t); //!< declare index to enum for template specialization
template<typename E> E evalue(string); //!< declare string to enum for template specialization

// get enum max
template<typename E> size_t emax(E) {
    return static_cast<size_t>(eval<E>(-1));
}

#endif