#ifndef LWE_META_REFLECTOR
#define LWE_META_REFLECTOR

#include "../../base/base.h"
#include "../../container/dictionary.hpp"

LWE_BEGIN
namespace meta {

struct Enumerator;
struct Field;

//! @brief T array for reflection (container of class fiedls and enum values list reflector)
//! @note  Relfector<MyClass> == MyClass reflector class
//! @tparam T Field or Enumerator
template<typename T> class Reflector {
    using Table = container::Dictionary<String, Reflector<T>>;

public:
    //! @tparam C constructor: type info to create
    template<class C> static const Reflector<T>& reflect();

public:
    template<typename C> static const Reflector<T>& find();                 //!< get registred C type data
    template<typename C> static const Reflector<T>& find(const C&);         //!< get registred C type data
    static const Reflector<T>&                      find(const StringView); //!< get registred C type data by name

public:
    Reflector() = default;
    Reflector(const Reflector&);
    Reflector(Reflector&&) noexcept;
    Reflector(const std::initializer_list<T>&);
    ~Reflector();

public:
    Reflector& operator=(const Reflector);
    Reflector& operator=(Reflector&&) noexcept;

public:
    const T& operator[](size_t) const;

public:
    const T* begin() const;
    const T* end() const;
    size_t   size() const;

private:
    template<typename Arg> void push(Arg&&);
    void                        shrink();

private:
    T*     data      = nullptr;
    size_t capacitor = 0;
    size_t count     = 0;

private:
    static Table& map();
};

using Enumeration = Reflector<Enumerator>;
using Structure   = Reflector<Field>;

} // namespace meta
LWE_END
#include "reflector.ipp"
#endif
