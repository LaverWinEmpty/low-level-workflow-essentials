#ifndef LWE_CORE_MACRO
#define LWE_CORE_MACRO

// namespace
#define LWE       lwe
#define LWE_BEGIN namespace LWE {
#define LWE_END   }

//! string macro
//! STRING(ARG) -> "ARG"
#ifndef STRING
#    define STRING(x) #x
#endif

//! macro string
//! #define ARG "string"
//! MACRO(ARG) -> "string"
#ifndef MACRO
#    define MACRO(x) STRING(x)
#endif

//! macro param
//! #define FUNCION(VALUE) VALUE
//! MACRO(WRAP(1, 2)) -> 1, 2
#ifndef TUPLE
#    define WRAP(...) __VA_ARGS__
#endif

#ifndef ASSERT
#    ifdef NDEBUG
#        define ASSERT(x) \
            do { }        \
            while(false)
#    else
#        define ASSERT(x)                         \
            do {                                  \
                if(!(x)) {                        \
                    ERROR_PRINT(ERROR_STRING(x)); \
                    std::terminate();             \
                }                                 \
            }                                     \
            while(false)
#    endif
#endif

// #ifdef _MSC_VER
// #    define UNINIT_BEGIN __pragma(warning(disable : 26495))
// #    define UNINIT_END   __pragma(warning(default : 26495))
// #elif(defined(__GNUC__) || defined(__clang__))
// #    define UNINIT_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
// #    define UNINIT_END   _Pragma("GCC diagnostic pop")
// #else
// #    define UNINIT_BEGIN
// #    define UNINIT_END
// #endif

#ifndef _T
#    ifdef UNICODE
#        define _T(x) L##x
#    else
#        define _T(x) x
#    endif
#endif

#ifndef TEXT
#    define TEXT _T
#endif

#ifndef PROP
#    define PROP(...) __declspec(property(__VA_ARGS__))
#endif

#define LWE_API __declspec(dllexport)

#define LWE_NODISCARD [[nodiscard]]

#ifndef NDEBUG
#    ifndef DEBUG
#        define DEBUG
#    endif
#endif

#ifndef __cplusplus
#    ifdef _MSC_VER
#        define __cplusplus 199'711L
#    endif
#endif

#define WINDOWS 0x00'01
#define LINUX   0x00'02
#define UNIX    0x00'03
#define BSD     0x00'04
#define APPLE   0x00'05
#define ANDROID 0x00'06

#define MSVC  0x01'00
#define GCC   0x02'00
#define CLANG 0x03'00

#ifndef OS
#    if defined(_WIN32)
#        define OS WINDOWS
#    elif defined(__linux__)
#        define OS LINUX
#    elif defined(__unix__)
#        define OS UNIX
#    elif defined(__FreeBSD__)
#        define OS BSD
#    elif defined(__APPLE__)
#        define OS APPLE
#    elif defined(__ANDROID__)
#        define OS ANDROID
#    endif
#endif

#ifndef COMPILER
#    if defined(_MSC_VER)
#        define COMPILER MSVC
#    elif defined(__GNUC__)
#        define COMPILER GCC
#    elif defined(__clang__)
#        define COMPILER CLANG
#    endif
#endif

#define CPP98 1'998
#define CPP03 2'003
#define CPP11 2'011
#define CPP14 2'014
#define CPP17 2'017
#define CPP20 2'020
#define CPP23 2'023

#ifndef CPP_VERSION
#    if __cplusplus >= 202'302L
#        define CPP_VERSION CPP23
#    elif __cplusplus >= 202'002L
#        define CPP_VERSION CPP20
#    elif __cplusplus >= 201'703L
#        define CPP_VERSION CPP17
#    elif __cplusplus >= 201'402L
#        define CPP_VERSION CPP14
#    elif __cplusplus >= 201'103L
#        define CPP_VERSION CPP11
#    elif __cplusplus >= 199'711L
#        define CPP_VERSION CPP03
#    else
#        define CPP_VERSION 0
#    endif
#endif

#if defined(_MSC_VER)
#    define ALLOC_API __declspec(allocator)
#elif defined(__clang__) || defined(__GNUC__)
#    define ALLOC_API __attribute__((malloc))
#else
#    define ALLOC_API
#endif

// use #prgma MESSAGE
#define MESSAGE(x) message(__FILE__ " [" MACRO(__LINE__) "] " #x)

#define LOCKGUARD(lock) if(std::lock_guard<decltype(lock)> MACRO_lock_guard(lock); true)

// clang-format off

#define GET_MACRO_1(a, NAME, ...) NAME                         //! for macro overloading, param count 0 ~ 1
#define GET_MACRO_2(a, b, NAME, ...) NAME                      //! for macro overloading, param count 0 ~ 2
#define GET_MACRO_3(a, b, c, NAME, ...) NAME                   //! for macro overloading, param count 0 ~ 3
#define GET_MACRO_4(a, b, c, d, NAME, ...) NAME                //! for macro overloading, param count 0 ~ 4
#define GET_MACRO_5(a, b, c, d, e, NAME, ...) NAME             //! for macro overloading, param count 0 ~ 5
#define GET_MACRO_6(a, b, c, d, e, f, NAME, ...) NAME          //! for macro overloading, param count 0 ~ 6
#define GET_MACRO_7(a, b, c, d, e, f, g, NAME, ...) NAME       //! for macro overloading, param count 0 ~ 7
#define GET_MACRO_8(a, b, c, d, e, f, g, h, NAME, ...) NAME    //! for macro overloading, param count 0 ~ 8
#define GET_MACRO_9(a, b, c, d, e, f, g, h, i, NAME, ...) NAME //! for macro overloading, param count 0 ~ 9

/*
 * @brief class body for register
 */
#define CLASS_BODY(TYPE, BASE)                                                                                         \
public:                                                                                                                \
	friend LWE::meta::Structure;                                                                                       \
    friend struct TYPE##Meta;                                                                                          \
    template<typename T> friend LWE::meta::Registered LWE::meta::registmethod();                                       \
    virtual LWE::meta::Class* meta() const override;                                                                   \
    using Base = BASE

 /*
  * @breif register class field (Type Name, Scope Name)
  */
#define REGISTER_CLASS_BEGIN(...) GET_MACRO_2(__VA_ARGS__,\
        Macro__register_class_begin_scoped,\
        Macro__register_class_begin_global \
    )(__VA_ARGS__)
#define Macro__register_class_begin_global(TYPE)         Macro__register_field_begin_detail(TYPE, ::)
#define Macro__register_class_begin_scoped(TYPE, SCOPE ) Macro__register_field_begin_detail(TYPE, SCOPE::)
#define Macro__register_class_begin_detail(TYPE, SCOPE)                                                                \
    template<> LWE::meta::Class* LWE::meta::classof<SCOPE TYPE>() {                                                    \
        static LWE::meta::Class* META = nullptr;                                                                       \
        if(!META) META = LWE::meta::Registry<LWE::meta::Class>::find(#TYPE);                                           \
        return META;                                                                                                   \
    }                                                                                                                  \
    template<> SCOPE TYPE* LWE::meta::statics<SCOPE TYPE>() {                                                          \
        static LWE::meta::Object* OBJ = nullptr;                                                                       \
        if(!OBJ) OBJ = LWE::meta::Registry<LWE::meta::Object>::find(#TYPE);                                            \
        return static_cast<SCOPE TYPE*>(OBJ);                                                                          \
    }                                                                                                                  \
    struct TYPE##Meta: LWE::meta::Class {                                                                              \
        virtual const char* name() const override {                                                                    \
            return #TYPE;                                                                                              \
        }                                                                                                              \
        virtual size_t size() const override {                                                                         \
            return sizeof(SCOPE TYPE);                                                                                 \
        }                                                                                                              \
        virtual LWE::meta::Class* base() const override {                                                              \
            return LWE::meta::classof<SCOPE TYPE::Base>();                                                             \
        }                                                                                                              \
        virtual LWE::meta::Object* statics() const override {                                                          \
            return LWE::meta::statics<SCOPE TYPE>();                                                                   \
        }                                                                                                              \
        virtual LWE::meta::Object* construct(LWE::meta::Object* in) const override {                                   \
            if constexpr (std::is_copy_constructible_v<SCOPE TYPE>) {                                                  \
                new (in) SCOPE TYPE(*LWE::meta::statics<SCOPE TYPE>());                                                \
            }                                                                                                          \
            else new (in) SCOPE TYPE();                                                                                \
            return in;                                                                                                 \
        }                                                                                                              \
        virtual const LWE::meta::Structure& fields() const override;                                                   \
    };                                                                                                                 \
    LWE::meta::Class* SCOPE TYPE::meta() const {                                                                       \
        return LWE::meta::classof<SCOPE TYPE>();                                                                       \
    }                                                                                                                  \
    template<> template<> const LWE::meta::Structure& LWE::meta::Structure::reflect<SCOPE TYPE>();                     \
    template<> LWE::meta::Registered LWE::meta::registclass<SCOPE TYPE>() {                                            \
        LWE::meta::Structure::reflect<SCOPE TYPE>();                                                                   \
        LWE::meta::Registry<LWE::meta::Object>::add<SCOPE TYPE>(#TYPE);                                                \
        LWE::meta::Registry<LWE::meta::Class>::add<TYPE##Meta>(#TYPE);                                                 \
        return LWE::meta::Registered::REGISTERED;                                                                      \
    }                                                                                                                  \
    LWE::meta::Registered TYPE##_FIELD_REGISTERED = LWE::meta::registclass<SCOPE TYPE>();                              \
    const LWE::meta::Structure& TYPE##Meta::fields() const {                                                           \
        static const LWE::meta::Structure& REF = LWE::meta::Structure::reflect<SCOPE TYPE>();                          \
        return REF;                                                                                                    \
    }                                                                                                                  \
    template<> template<> const LWE::meta::Structure& LWE::meta::Structure::reflect<SCOPE TYPE>() {                    \
        using CLASS = SCOPE TYPE;                                                                                      \
        const char* NAME = #TYPE;                                                                                      \
        auto result = map().find(NAME);                                                                                \
        if (result != map().end()) {                                                                                   \
        	return result->second;                                                                                     \
        }                                                                                                              \
        LWE::meta::Structure INFO; // {
#define CLASS_FIELD(FIELD)                                                                                             \
        INFO.push(                                                                                                     \
                LWE::meta::Field {                                                                                     \
                    typeof<decltype(CLASS::FIELD)>(),                                                                  \
                    #FIELD,                                                                                            \
                    sizeof(CLASS::FIELD),                                                                              \
                    offsetof(CLASS, FIELD)                                                                             \
                }                                                                                                      \
            ) // }
#define REGISTER_CLASS_END                                                                                             \
        INFO.shrink();                                                                                                 \
        map().push({ NAME, INFO });                                                                                    \
        return map()[NAME];                                                                                            \
    }

  /*
   * @brief register class method (Type Name, Function Name)
   */
#define REGISTER_METHOD_BEGIN(...) GET_MACRO_2(__VA_ARGS__,                                                            \
        Macro__register_method_begin_scoped,                                                                           \
        Macro__register_method_begin_global                                                                            \
    )(__VA_ARGS__)
#define Macro__register_method_begin_global(TYPE)        Macro__register_method_begin_detail(TYPE, ::)
#define Macro__register_method_begin_scoped(TYPE, SCOPE) Macro__register_method_begin_detail(TYPE, SCOPE::)
#define Macro__register_method_begin_detail(TYPE, SCOPE)                                                               \
    template<> LWE::meta::Method* LWE::meta::methodof<SCOPE TYPE>(const std::string& in) {                             \
        return Registry<LWE::meta::Method>::find(#TYPE, in);                                                           \
    }                                                                                                                  \
    template<> LWE::meta::Registered LWE::meta::registmethod<SCOPE TYPE>();                                            \
    LWE::meta::Registered TYPE##_METHOD_REGISTERED = LWE::meta::registmethod<SCOPE TYPE>();                            \
    template<> LWE::meta::Registered LWE::meta::registmethod<SCOPE TYPE>() {                                           \
        using TYPE_NAME = SCOPE TYPE;                                                                                  \
        static const string CLASS_NAME = { #TYPE }; // {
#define REGISTER_METHOD(NAME)                                                                                          \
            Registry<LWE::meta::Method>::add(CLASS_NAME, #NAME, Method::lambdaize(&TYPE_NAME::NAME)) // ; }
#define REGISTER_METHOD_END                                                                                            \
        return LWE::meta::Registered::REGISTERED;                                                                      \
    }

/*
 * @breif register enum (Type Name, Scope Name)
 */
#define REGISTER_ENUM_BEGIN(...) GET_MACRO_2(__VA_ARGS__,\
        Macro__register_enum_begin_scoped,\
        Macro__register_enum_begin_global \
    )(__VA_ARGS__)
#define Macro__register_enum_begin_global(TYPE)        Macro__register_enum_begin_detail(TYPE, ::)
#define Macro__register_enum_begin_scoped(TYPE, SCOPE) Macro__register_enum_begin_detail(TYPE, SCOPE::)
#define Macro__register_enum_begin_detail(TYPE, SCOPE)                                                                 \
    struct TYPE##Meta;                                                                                                 \
    template<> template<> const LWE::meta::Enumeration& LWE::meta::Enumeration::reflect<SCOPE TYPE>();                 \
    struct TYPE##Meta: LWE::meta::Enum {                                                                               \
        virtual const char* name() const {                                                                             \
            return #TYPE;                                                                                              \
        }                                                                                                              \
        virtual size_t size() const {                                                                                  \
            return sizeof(SCOPE TYPE);                                                                                 \
        }                                                                                                              \
        virtual const LWE::meta::Enumeration& enums() const {                                                          \
            static const LWE::meta::Enumeration& REF = LWE::meta::Enumeration::reflect<SCOPE TYPE>();                  \
            return REF;                                                                                                \
        }                                                                                                              \
    };                                                                                                                 \
    template<> LWE::meta::Enum* LWE::meta::enumof<SCOPE TYPE>() {                                                      \
        static LWE::meta::Enum* ENUM = nullptr;                                                                        \
        if(!ENUM) ENUM = LWE::meta::Registry<LWE::meta::Enum>::find(#TYPE);                                            \
        return ENUM;                                                                                                   \
    }                                                                                                                  \
    template<> LWE::meta::Enum* LWE::meta::enumof<SCOPE TYPE>(const SCOPE TYPE&) {                                     \
        return enumof<SCOPE TYPE>();                                                                                   \
    }                                                                                                                  \
    template<> LWE::meta::Registered LWE::meta::registenum<SCOPE TYPE>() {                                             \
        LWE::meta::Enumeration::reflect<SCOPE TYPE>();                                                                 \
        LWE::meta::Registry<LWE::meta::Enum>::add<TYPE##Meta>(#TYPE);                                                  \
        return LWE::meta::Registered::REGISTERED;                                                                      \
    }                                                                                                                  \
    LWE::meta::Registered TYPE##_REGISTERED = LWE::meta::registenum<SCOPE TYPE>();                                     \
    template<> template<> const LWE::meta::Enumeration& LWE::meta::Enumeration::reflect<SCOPE TYPE>() {                \
        using ENUM_ALIAS = SCOPE TYPE;                                                                                 \
        const char* NAME = #TYPE;                                                                                      \
        auto result = map().find(NAME);                                                                                \
        if (result != map().end()) {                                                                                   \
        	return result->second;                                                                                     \
        }                                                                                                              \
        LWE::meta::Enumeration INFO; // {
#define REGISTER_ENUM(VALUE)                                                                                           \
            INFO.push(LWE::meta::Enumerator{ static_cast<uint64_t>(ENUM_ALIAS::VALUE), #VALUE }) // }
#define REGISTER_ENUM_END                                                                                              \
        INFO.shrink();                                                                                                 \
        map().push({ NAME, INFO });                                                                                    \
        return map()[NAME];                                                                                            \
    }

//! NOTE: need push()
#define DECLARE_CONTAINER(TEMPLATE, CLASS, BASE, ...)                   \
    template<WRAP TEMPLATE> class CLASS: public BASE<__VA_ARGS__>,      \
                                       public LWE::meta::Container {    \
        using Base = BASE<__VA_ARGS__>;                                 \
    public:                                                             \
        using Base::Base;                                               \
        virtual std::string serialize() const override {                \
            return LWE::meta::Codec::encode<CLASS<__VA_ARGS__>>(*this); \
        }                                                               \
        virtual void deserialize(const std::string_view in) override {  \
            lwe::meta::Codec::decode<CLASS<__VA_ARGS__>>(this, in);     \
        }                                                               \
    }

//! register container code
#define REGISTER_CONTAINER(TEMPLATE, CLASS, CODE, ...)                            \
    template<WRAP TEMPLATE> struct LWE::meta::ContainerCode<CLASS<__VA_ARGS__>> { \
        static constexpr meta::Keyword KEYWORD = static_cast<LWE::meta::Keyword>(CODE);   \
    }

/**
 * @brief default container serializer and deserializer override
 */
#define CONTAINER_BODY(CONTAINER, ELEMENT, ...)                                                                        \
    template<Mod MOD> using Iterator = Iterator<MOD, CONTAINER<__VA_ARGS__>>;                                          \
    template<Mod, typename> friend class LWE::container::Iterator;                                                     \
    using iterator               = Iterator<FWD>;                                                                      \
    using const_iterator         = Iterator<FWD | VIEW>;                                                               \
    using reverse_iterator       = Iterator<BWD>;                                                                      \
    using const_reverse_iterator = Iterator<BWD | VIEW>;                                                               \
    using value_type             = ELEMENT

#define ITERATOR_BODY(MOD, CONTAINER, ...)                       \
    using CONTAINER = CONTAINER<__VA_ARGS__>;                    \
    using Reverse   = Iterator<MOD ^ Mod(FWD | BWD), CONTAINER>; \
    using Const     = Iterator<MOD | VIEW, CONTAINER>;           \
    friend class Reverse

#define REGISTER_CONST_ITERATOR(TEMPLATE, MOD, CONTAINER, ...)                         \
    template<WRAP TEMPLATE>                                                            \
    class Iterator<MOD | VIEW, CONTAINER<__VA_ARGS__>>                                 \
        : public Iterator<MOD, CONTAINER<__VA_ARGS__>> {                               \
        using CONTAINER = CONTAINER<__VA_ARGS__>;                                      \
        using Mutable   = Iterator<MOD, CONTAINER>;                                    \
        using Iterator<MOD, CONTAINER>::Iterator;                                      \
    public:                                                                            \
        Iterator(const Iterator<MOD, CONTAINER>& in): Iterator<MOD, CONTAINER>(in) { } \
        const T& operator*() { return Mutable::operator*(); }                          \
        const T* operator->() { return Mutable::operator->(); }                        \
    }

// clang-format on

// header guard end
#endif
