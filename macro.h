
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

#if(CPP_VERSION >= CPP17)
#    define IF_CONST if constexpr
#else
#    define IF_CONST if
#endif

#if(CPP_VERSION >= CPP17)
#    define LOCKGUARD(lock) if(std::lock_guard<decltype(lock)> MACRO_lock_guard(lock); true)
#else
#    define LOCKGUARD(lock)                                                                                            \
        for(int i = 0; i < 1;)                                                                                         \
            for(std::lock_guard<decltype(lock)> MACRO_lock_guard(lock); i < 1; ++i)
#endif

// clang-format off

/*
 * @brief register metadata inner class
 */
#define CLASS_BODY(TYPE, BASE)                                                                                         \
    friend struct MetaClass;                                                                                           \
    struct TYPE##Meta: MetaClass {                                                                                     \
        virtual const char* name() const override {                                                                    \
            return #TYPE;                                                                                              \
        }                                                                                                              \
        virtual size_t size() const override {                                                                         \
            return sizeof(TYPE);                                                                                       \
        }                                                                                                              \
        virtual MetaClass* base() const override {                                                                     \
            return MetaClass::get<BASE>();                                                                             \
        }                                                                                                              \
        virtual const FieldInfo& field() const override;                                                               \
    };                                                                                                                 \
public:                                                                                                                \
    virtual MetaClass* metaclass() override {                                                                          \
        static TYPE##Meta meta;                                                                                        \
        return &meta;                                                                                                  \
    }                                                                                                                  \
    using Base = BASE;

//! @brief declare  evalue from string
#define REGISTER_STRING_TO_ENUM_BEGIN(TYPE)                                                                            \
    template<> TYPE evalue<TYPE>(std::string IN) {                                                                     \
        using enum TYPE;                                                                                               \
        std::unordered_map<std::string, TYPE> MAP;                                                                     \
        if(MAP.find(IN) == MAP.end())
#define REGISTER_STRING_TO_ENUM(VAL)                                                                                   \
    if(#VAL == IN) {                                                                                                   \
        MAP.insert({ #VAL, VAL });                                                                                     \
        return VAL;                                                                                                    \
    }
#define REGISTER_STRING_TO_ENUM_END                                                                                    \
    else return MAP[IN]; throw std::out_of_range(IN);                                                                  \
    }

//! @brief register enum to string
#define REGISTER_ENUM_TO_STRING_BEGIN(TYPE)                                                                            \
    constexpr const char* estring(TYPE IN) {                                                                           \
        using enum TYPE;                                                                                               \
        switch(IN) // {
#define REGISTER_ENUM_TO_STRING(VAL)                                                                                   \
        case VAL: return #VAL;
#define REGISTER_ENUM_TO_STRING_END                                                                                    \
    return "";                                                                                                         \
    }

//! @brief delcare evalue from index
#define REGISTER_INDEX_TO_ENUM_BEGIN(TYPE)                                                                             \
    template<> TYPE evalue<TYPE>(size_t IN) {                                                                          \
        using enum TYPE;                                                                                               \
        size_t                           INDEX = 0;                                                                    \
        std::unordered_map<size_t, TYPE> MAP;                                                                          \
        if(MAP.find(IN) == MAP.end()) // {
#define REGISTER_INDEX_TO_ENUM(VAL)                                                                                    \
            if(IN == INDEX) {                                                                                          \
                MAP.insert({ INDEX, VAL });                                                                            \
                return VAL;                                                                                            \
            }                                                                                                          \
            ++INDEX;
#define REGISTER_INDEX_TO_ENUM_END                                                                                     \
        else return MAP[IN]; return static_cast<decltype(MAP.begin()->second)>(INDEX);                                 \
    }

//! @brief register enum to index
#define REGISTER_ENUM_TO_INDEX_BEGIN(TYPE)                                                                             \
    constexpr size_t eindex(TYPE IN) {                                                                                 \
        using enum TYPE;                                                                                               \
        size_t                                  INDEX = 0;                                                             \
        static std::unordered_map<TYPE, size_t> MAP;                                                                   \
        if(MAP.find(IN) == MAP.end()) // {
#define REGISTER_ENUM_TO_INDEX(VAL)                                                                                    \
            if(VAL == IN) {                                                                                            \
                MAP.insert({ VAL, INDEX });                                                                            \
                return INDEX;                                                                                          \
            }                                                                                                          \
            ++INDEX;
#define REGISTER_ENUM_TO_INDEX_END                                                                                     \
        else return MAP[IN];                                                                                           \
        return INDEX;                                                                                                  \
    }

//! @brief register class
#define REGISTER_CLASS(TYPE)\
    template<> MetaClass* MetaClass::get<TYPE>() {                                                                     \
        static TYPE DUMMY;                                                                                             \
        return DUMMY.metaclass();                                                                                      \
    }                                                                                                                  \
    template<> MetaClass* MetaClass::get<TYPE>(const TYPE&) {                                                          \
        return get<TYPE>();                                                                                            \
    }

//! @brief fields begin
#define REGISTER_FIELD_BEGIN(TYPE)                                                                                     \
    const std::vector<MetaField>& TYPE::TYPE##Meta::field() const {                                                    \
        static auto ACCESS_MODIFIER = [](const char* in) -> EAccess {                                               \
            if(!std::strcmp(in, "public")) { return EAccess::PUBLIC; }                                              \
            if(!std::strcmp(in, "private")) { return EAccess::PRIVATE; }                                            \
            if(!std::strcmp(in, "protected")) { return EAccess::PROTECTED; }                                        \
            return EAccess::NONE;                                                                                   \
        };                                                                                                             \
        static std::vector<MetaField> VECTOR = reflect<TYPE>( // {
//! @brief field
#define REGISTER_FIELD(ACCESS, NAME, ...) \
            MetaField{ ACCESS_MODIFIER(#ACCESS), typeof<__VA_ARGS__>(), #NAME, sizeof(__VA_ARGS__)  },
//! @brief fiels end
#define REGISTER_FIELD_END                                                                                             \
        );                                                                                                             \
        return VECTOR;                                                                                                 \
    }

/**
 * @brief container enum value register
 */
#define REGISTER_CONTAINER(CONTAINER, ENUM)                                                                            \
    template<typename T> struct ContainerCode<T, std::void_t<typename T::CONTAINER##Element>> {                        \
        using enum EType;                                                                                           \
        static constexpr EType VALUE = ENUM;                                                                         \
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