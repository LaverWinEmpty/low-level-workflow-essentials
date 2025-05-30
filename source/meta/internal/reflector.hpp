#ifndef LWE_META_REFLECTOR
#define LWE_META_REFLECTOR

#include "type.hpp"

LWE_BEGIN
namespace meta {

struct Enumerator;
struct Field;

//! @brief T array for reflection (container of class fiedls and enum values list reflector)
//! @note  Relfector<MyClass> == MyClass reflector class
//! @tparam T Field or Enumerator
template<typename T> class Reflector {
public:
    //! @tparam C constructor: type info to create
    template<class C> static const Reflector<T>& reflect();

public:
    template<typename C> static const Reflector<T>& find();              //!< get registred C type data
    template<typename C> static const Reflector<T>& find(const C&);      //!< get registred C type data
    static const Reflector<T>&                      find(const char*);   //!< get registred C type data by name
    static const Reflector<T>&                      find(const string&); //!< get registred C type data by name

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
    inline static std::unordered_map<string, Reflector<T>>& map();
};

using Enumerate = Reflector<Enumerator>;
using Structure = Reflector<Field>;

}
LWE_END
#include "reflector.ipp"
#endif