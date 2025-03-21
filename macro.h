
#ifndef LWE_MACRO_HEADER
#define LWE_MACRO_HEADER

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

#define DECLARE_NO_COPY(Class)                                                                                         \
    Class(const Class&)            = delete;                                                                           \
    Class(Class&&)                 = delete;                                                                           \
    Class& operator=(const Class&) = delete;                                                                           \
    Class& operator=(Class&&)      = delete

#define DECLARE_STATIC_CLASS(Class)                                                                                    \
    Class()  = delete;                                                                                                 \
    ~Class() = delete;                                                                                                 \
    DECLARE_NO_COPY(Class)

#define ERROR_STRING(x)                                                                                                \
    std::string() + "File: " + __FILE__ + "\nLine: " + MACRO(__LINE__) + "\nFunction: " + __func__ + "\nFailed: " + #x

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

#ifdef _MSC_VER
#    define UNINIT_BEGIN __pragma(warning(disable : 26495))
#    define UNINIT_END   __pragma(warning(default : 26495))
#elif(defined(__GNUC__) || defined(__clang__))
#    define UNINIT_BEGIN _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
#    define UNINIT_END   _Pragma("GCC diagnostic pop")
#else
#    define UNINIT_BEGIN
#    define UNINIT_END
#endif

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

/*
 * @brief register metadata inner class
 */
#define CLASS_BODY(TYPE, BASE)                                                                                         \
public:                                                                                                                \
    virtual Class* meta() const override {                                                                             \
        static Class* metacls = Registry<Class>::find(#TYPE);                                                           \
        return metacls;                                                                                                \
    }                                                                                                                  \
    using Base = BASE;                                                                                                 \
private:

#define REGISTER_FIELD_BEGIN(TYPE)                                                                                     \
    struct TYPE##Meta: Class {                                                                                         \
        virtual const char* name() const override {                                                                    \
            return #TYPE;                                                                                              \
        }                                                                                                              \
        virtual size_t size() const override {                                                                         \
            return sizeof(TYPE);                                                                                       \
        }                                                                                                              \
        virtual Class* base() const override {                                                                         \
            return classof<TYPE::Base>();                                                                            \
        }                                                                                                              \
        virtual const Structure& fields() const override;                                                              \
    };                                                                                                                 \
    template<> template<> const Structure& Structure::reflect<TYPE>();                                                 \
    template<> Object* statics<TYPE>() {                                                                               \
        static Object* OBJ = Registry<Object>::find(#TYPE);                                                            \
        return OBJ;                                                                                                    \
    }                                                                                                                  \
    template<> Registered registclass<TYPE>() {                                                                        \
        Structure::reflect<TYPE>();                                                                                    \
        Registry<Object>::add<TYPE>(#TYPE);                                                                            \
        Registry<Class>::add<TYPE##Meta>(#TYPE);                                                                       \
        return Registered::REGISTERED;                                                                                 \
    }                                                                                                                  \
    Registered TYPE##_RGISTERED = registclass<TYPE>();                                                                 \
    const Structure& TYPE##Meta::fields() const {                                                                      \
        static const Structure& REF = Structure::reflect<TYPE>();                                                      \
        return REF;                                                                                                    \
    }                                                                                                                  \
    template<> template<> const Structure& Structure::reflect<TYPE>() {                                                \
        using CLASS = TYPE;                                                                                            \
        const char* NAME = #TYPE;                                                                                      \
        auto result = map.find(NAME);                                                                                  \
        if (result != map.end()) {                                                                                     \
        	return result->second;                                                                                     \
        }                                                                                                              \
        Structure meta; // {
#define REGISTER_FIELD(FIELD)                                                                                          \
        meta.push(                                                                                                     \
                Field {                                                                                                \
                    typeof<decltype(CLASS::FIELD)>(),                                                                  \
                    #FIELD,                                                                                            \
                    sizeof(CLASS::FIELD),                                                                              \
                    offsetof(CLASS, FIELD)                                                                             \
                }                                                                                                      \
            ) // }
#define REGISTER_FIELD_END                                                                                             \
        meta.shrink();                                                                                                 \
        map.insert({ NAME, meta });                                                                                    \
        return map[NAME];                                                                                              \
    }

#define REGISTER_ENUM_BEGIN(TYPE)                                                                                      \
    struct TYPE##Meta;                                                                                                 \
    template<> template<> const Enumerate& Enumerate::reflect<TYPE>();                                                 \
    struct TYPE##Meta: Enum {                                                                                          \
        virtual const char* name() const {                                                                             \
            return #TYPE;                                                                                              \
        }                                                                                                              \
        virtual size_t size() const {                                                                                  \
            return sizeof(TYPE);                                                                                       \
        }                                                                                                              \
        virtual const Enumerate& enums() const {                                                                       \
            static const Enumerate& REF = Enumerate::reflect<TYPE>();                                                  \
            return REF;                                                                                                \
        }                                                                                                              \
    };                                                                                                                 \
    template<> Enum* enumof<TYPE>() {                                                                                \
        static Enum* ENUM = Registry<Enum>::find(#TYPE);                                                               \
        return ENUM;                                                                                                   \
    }                                                                                                                  \
    template<> Registered registenum<TYPE>() {                                                                         \
        Enumerate::reflect<TYPE>();                                                                                    \
        Registry<Enum>::add<TYPE##Meta>(#TYPE);                                                                        \
        return Registered::REGISTERED;                                                                                 \
    }                                                                                                                  \
    Registered TYPE##_REGISTERED = registenum<TYPE>();                                                                 \
    template<> template<> const Enumerate& Enumerate::reflect<TYPE>() {                                                \
        using enum TYPE;                                                                                               \
        const char* NAME = #TYPE;                                                                                      \
        auto result = map.find(NAME);                                                                                  \
        if (result != map.end()) {                                                                                     \
        	return result->second;                                                                                     \
        }                                                                                                              \
        Enumerate meta; // {
#define REGISTER_ENUM(VALUE)                                                                                           \
        meta.push(Enumerator{ static_cast<uint64>(VALUE), #VALUE }) // }
#define REGISTER_ENUM_END                                                                                              \
        meta.shrink();                                                                                                 \
        map.insert({ NAME, meta });                                                                                    \
        return map[NAME];                                                                                              \
    }

/**
 * @brief container enum value register
 */
#define REGISTER_CONTAINER(CONTAINER, ENUM)                                                                            \
    template<typename T> struct ContainerCode<T, std::void_t<typename T::CONTAINER##Element>> {                        \
        using enum EType;                                                                                              \
        static constexpr EType VALUE = ENUM;                                                                           \
    }

/**
 * @brief default container serializer and deserializer override
 */
#define CONTAINER_BODY(CONTAINER, ELEMENT, ...)                                                                        \
    using CONTAINER##Element = ELEMENT;                                                                                \
    virtual void deserialize(const string& in) override {                                                              \
        *this = Container::deserialize<CONTAINER<ELEMENT  __VA_OPT__(,) __VA_ARGS__>>(in);                             \
    }                                                                                                                  \
    virtual std::string serialize() const override {                                                                   \
        return Container::serialize<CONTAINER<ELEMENT __VA_OPT__(,) __VA_ARGS__>>(this);                               \
    }                                                                                                                  \
    using value_type = CONTAINER##Element

// clang-format on

// header guard end
#endif