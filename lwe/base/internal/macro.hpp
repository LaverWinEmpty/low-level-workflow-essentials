#ifndef LWE_CORE_MACRO
#define LWE_CORE_MACRO

// namespace
#define LWE       lwe
#define LWE_BEGIN namespace LWE {
#define LWE_END   }

#ifndef STRING
#    define STRING(x) #x
#endif

#ifndef MACRO
#    define MACRO(x) STRING(x)
#endif

#ifndef ASSERT
#    ifdef NDEBUG
#        define ASSERT(x)                                                                                              \
            do {                                                                                                       \
            } while(false)
#    else
#        define ASSERT(x)                                                                                              \
            do {                                                                                                       \
                if(!(x)) {                                                                                             \
                    ERROR_PRINT(ERROR_STRING(x));                                                                      \
                    std::terminate();                                                                                  \
                }                                                                                                      \
            } while(false)
#    endif
#endif

//#ifdef _MSC_VER
//#    define UNINIT_BEGIN __pragma(warning(disable : 26495))
//#    define UNINIT_END   __pragma(warning(default : 26495))
//#elif(defined(__GNUC__) || defined(__clang__))
//#    define UNINIT_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
//#    define UNINIT_END   _Pragma("GCC diagnostic pop")
//#else
//#    define UNINIT_BEGIN
//#    define UNINIT_END
//#endif

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
#        define __cplusplus 199711L
#    endif
#endif

#define WINDOWS 0x0001
#define LINUX   0x0002
#define UNIX    0x0003
#define BSD     0x0004
#define APPLE   0x0005
#define ANDROID 0x0006

#define MSVC  0x0100
#define GCC   0x0200
#define CLANG 0x0300

#ifndef OS
#    if defined(_WIN32)
#        define OS WINDOWS
#    elif defined(__linux__)
#        define OS LINUX
#    elif defined(__unix___
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

#define CPP98 1998
#define CPP03 2003
#define CPP11 2011
#define CPP14 2014
#define CPP17 2017
#define CPP20 2020
#define CPP23 2023

#ifndef CPP_VERSION
#    if __cplusplus >= 202302L
#        define CPP_VERSION CPP23
#    elif __cplusplus >= 202002L
#        define CPP_VERSION CPP20
#    elif __cplusplus >= 201703L
#        define CPP_VERSION CPP17
#    elif __cplusplus >= 201402L
#        define CPP_VERSION CPP14
#    elif __cplusplus >= 201103L
#        define CPP_VERSION CPP11
#    elif __cplusplus >= 199711L
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
#define REGISTER_FIELD_BEGIN(...) GET_MACRO_2(__VA_ARGS__,\
        Macro__register_field_begin_scoped,\
        Macro__register_field_begin_global \
    )(__VA_ARGS__)
#define Macro__register_field_begin_global(TYPE)         Macro__register_field_begin_detail(TYPE, ::)
#define Macro__register_field_begin_scoped(TYPE, SCOPE ) Macro__register_field_begin_detail(TYPE, SCOPE::)
#define Macro__register_field_begin_detail(TYPE, SCOPE)                                                                \
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
        LWE::meta::Structure info; // {
#define REGISTER_FIELD(FIELD)                                                                                          \
        info.push(                                                                                                     \
                LWE::meta::Field {                                                                                     \
                    typeof<decltype(CLASS::FIELD)>(),                                                                  \
                    #FIELD,                                                                                            \
                    sizeof(CLASS::FIELD),                                                                              \
                    offsetof(CLASS, FIELD)                                                                             \
                }                                                                                                      \
            ) // }
#define REGISTER_FIELD_END                                                                                             \
        info.shrink();                                                                                                 \
        map().insert({ NAME, info });                                                                                  \
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
        LWE::meta::Enumeration meta; // {
#define REGISTER_ENUM(VALUE)                                                                                           \
            meta.push(LWE::meta::Enumerator{ static_cast<uint64_t>(ENUM_ALIAS::VALUE), #VALUE }) // }
#define REGISTER_ENUM_END                                                                                              \
        meta.shrink();                                                                                                 \
        map().insert({ NAME, meta });                                                                                  \
        return map()[NAME];                                                                                            \
    }

/**
 * @brief container enum value register
 */
#define REGISTER_CONTAINER(CONTAINER, ENUM)                                                                            \
    template<typename T> struct LWE::meta::ContainerCode<T, std::void_t<typename T::CONTAINER##Element>> {             \
        using ENUM_ALIAS = LWE::meta::Keyword;                                                                         \
        static constexpr meta::Keyword VALUE = ENUM_ALIAS::ENUM;                                                       \
    }

/**
 * @brief default container serializer and deserializer override
 */
#define CONTAINER_BODY(ELEMENT, CONTAINER, ...)                                                                        \
public:                                                                                                                \
	friend LWE::meta::Container;                                                                                       \
    using CONTAINER##Element = ELEMENT;                                                                                \
    virtual std::string serialize() const override {                                                                   \
        const LWE::meta::Container* ptr = static_cast<const Container*>(this);                                         \
        return LWE::meta::serialize<CONTAINER<__VA_ARGS__>>(ptr);                                                      \
    }                                                                                                                  \
    virtual void deserialize(const string& in) override {                                                              \
        const LWE::meta::Container* ptr = const_cast<Container*>(static_cast<const Container*>(this));                 \
        LWE::meta::deserialize<CONTAINER<__VA_ARGS__>>(this, in);                                                      \
    }                                                                                                                  \
    using value_type = CONTAINER##Element

// clang-format on

// header guard end
#endif