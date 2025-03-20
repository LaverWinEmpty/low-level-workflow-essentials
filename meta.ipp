#include "meta.hpp"
#ifdef LWE_META_HEADER

// TODO: class, enum name write to Type (const char* -> uintptr)

void Type::push(EType in) {
    size_t next = count + 1;

    // swap
    if(next == STACK) {
        EType buffer[STACK];
        std::memcpy(buffer, stack, STACK);

        capacitor = STACK << 1; // multiple of 2
        heap      = static_cast<EType*>(malloc(sizeof(EType) * capacitor));
        if(!heap) {
            std::memcpy(stack, buffer, STACK); // rollback
            throw std::bad_alloc();            // failed
        }
        std::memcpy(heap, buffer, STACK);

        heap[count] = in;
        count       = next;
    }

    // use heap
    else if(next > STACK) {
        // reallocate
        if(next >= capacitor) {
            capacitor    <<= 1;
            EType* newly   = static_cast<EType*>(realloc(heap, capacitor));
            if(!newly) {
                capacitor >>= 1;
                throw std::bad_alloc();
            }
            heap = newly;
        }
        heap[count] = in;
        count       = next;
    }

    // use stack
    else {
        stack[count] = in;
        count        = next;
    }
}

template<typename T> static const Type& Type::reflect() {
    static Type type;
    if(type.size() == 0) {
        reflect<T>(&type);
        for(auto itr : type) {
            type.hashed = (type.hashed << 5) - static_cast<std::underlying_type_t<EType>>(itr);
        }
    }
    return type;
}

template<typename T> static void Type::reflect(Type* out) {
    if constexpr(std::is_const_v<T>) {
        out->push(EType::CONST);
        reflect<typename std::remove_const_t<T>>(out);
    }

    else if constexpr(isSTL<T>()) {
        out->push(ContainerCode<T>::VALUE);
        reflect<typename T::value_type>(out);
    }

    else if constexpr(std::is_pointer_v<T>) {
        // POINTER CONST TYPENAME: const typeanme*
        // CONST POINTER TYPENAME: typename* const
        out->push(EType::POINTER);
        reflect<typename std::remove_pointer_t<T>>(out); // dereference
    }

    else if constexpr(std::is_reference_v<T>) {
        using Temp = typename std::remove_reference_t<T>;
        // CONST REFERENCE TYPENAME
        if(std::is_const_v<Temp>) {
            out->push(EType::CONST);
            out->push(EType::REFERENCE);
            reflect<typename std::remove_const_t<Temp>>(out);
        } else {
            out->push(EType::REFERENCE);
            reflect<Temp>(out); // dereference
        }
    } else out->push(typecode<T>()); // primitive
}

Type::Type(const Type& in): count(in.count), hashed(in.hashed) {
    if(in.count < STACK) {
        std::memcpy(stack, in.stack, sizeof(EType) * count);
    } else {
        heap = static_cast<EType*>(malloc(sizeof(EType) * in.capacitor));
        if(!heap) {
            count = 0;
            throw std::bad_alloc();
        }
        capacitor = in.capacitor;
        std::memcpy(heap, in.heap, sizeof(EType) * count);
    }
}

Type::Type(Type&& in) noexcept: count(in.count), hashed(in.hashed) {
    if(in.count < STACK) {
        std::memcpy(stack, in.stack, count); // copy
    } else {
        heap      = in.heap;
        capacitor = in.capacitor;
        hashed    = in.hashed;

        in.heap      = nullptr;
        in.capacitor = 0;
        in.count     = 0;
        in.hashed    = 0;
    }
}

Type::Type(EType in): count(1), hashed(hash()) {
    stack[0] = in;
}

Type::~Type() {
    if(count >= STACK) {
        free(heap);
    }
}

Type& Type::operator=(const Type in) {
    EType* dest = count < STACK ? stack : heap; // allocated: use heap
    if(in.count < STACK) {
        std::memcpy(dest, in.stack, in.count); // copy stack
    } else {
        // reallocation required
        if(in.count > capacitor) {
            dest = static_cast<EType*>(malloc(sizeof(EType) * in.count));
            if(dest) {
                if(count > STACK) free(heap); // counter is before moving
            } else throw std::bad_alloc();    // error
        }
        std::memcpy(dest, in.heap, in.count);
        heap      = dest;
        capacitor = in.capacitor;
    }
    count  = in.count;
    hashed = in.hashed;
    return *this;
}

Type& Type::operator=(Type&& in) noexcept {
    if(this != &in) {
        if(count > STACK) {
            free(heap);
        }
        count  = in.count;
        hashed = in.hashed;
        if(count < STACK) {
            std::memcpy(stack, in.stack, count); // copy
        } else {
            heap      = in.heap;
            capacitor = in.capacitor;

            in.heap      = nullptr;
            in.capacitor = 0;
            in.count     = 0;
        }
    }
    return *this;
}

const EType& Type::operator[](size_t idx) const {
    if(count < STACK) {
        return stack[idx];
    } else return heap[idx];
}

Type::operator EType() const {
    if(count == 0) return EType::UNREGISTERED;
    if(count < STACK) {
        if(*stack == EType::CONST) {
            return stack[1];
        }
        return *stack;
    }
    if(*heap == EType::CONST) {
        return heap[1];
    }
    return *heap;
}

hash_t Type::hash() const {
    return hashed;
}

EType Type::type() const {
    if(count < STACK) {
        return stack[0] == EType::CONST ? stack[1] : stack[0];
    }
    return heap[0] == EType::CONST ? heap[1] : heap[0];
}

const char* Type::stringify() const {
    std::function<size_t(string*, const Type&, size_t)> fn = [&fn](string* out, const Type& in, size_t idx) {
        if(idx >= in.size()) {
            return idx;
        }
        // pointer or reference
        if(in[idx] == EType::POINTER || in[idx] == EType::REFERENCE) {
            fn(out, in, idx + 1); // rec
        }
        // const
        else if(in[idx] == EType::CONST) {
            // CONST POINTER ... = ...* const
            if(in[idx + 1] == EType::POINTER) {
                fn(out, in, idx + 1);
                out->append(" const");
            }
            // CONST ... = const ...
            // CONST REFERENCE ...  = const ...&
            else {
                out->append("const ");
                fn(out, in, idx + 1);
            }
        }
        // primitive
        else {
            out->append(typestring(in[idx]));
        }
        // has template
        if(isSTL(in[idx])) {
            out->append("<");
            fn(out, in, idx + 1);

            // TODO: map 처리

            out->append(">");
        }

        return idx + 1;
    };

    static std::unordered_map<size_t, string> map;
    if(map.find(hashed) == map.end()) {
        string temp;
        fn(&temp, *this, 0);
        map.insert({ hashed, temp });
    }
    return map[hashed].c_str();
}

Type::operator string() const {
    return stringify(); // copy
}

const EType* Type::begin() const {
    if(count < STACK) {
        return stack;
    }
    return heap;
}

const EType* Type::end() const {
    if(count < STACK) {
        return stack + count;
    }
    return heap + count;
}

size_t Type::size() const {
    return count;
}

template<typename T> Reflect<T>::Reflect(const Reflect& in): capacitor(in.capacitor), count(in.count) {
    // set
    capacitor = in.capacitor;
    count     = in.count;
    if(count == 0) {
        data = nullptr;
        return;
    }

    // new and copy
    data = static_cast<T*>(malloc(sizeof(T) * capacitor));
    if(!data) {
        capacitor = 0;
        count     = 0;
        throw std::bad_alloc();
    }
    std::memcpy(data, in.data, sizeof(T) * capacitor);
}

template<typename T>
Reflect<T>::Reflect(Reflect&& in) noexcept: data(in.data), count(in.count), capacitor(in.capacitor) {
    in.data      = nullptr;
    in.capacitor = 0;
    in.count     = 0;
}

template<typename T> Reflect<T>::Reflect(const std::initializer_list<T>& in): count(0), capacitor(in.size()) {
    data = static_cast<Field*>(malloc(sizeof(Field) * capacitor));
    if(!data) {
        throw std::bad_alloc();
    }
    for(auto& i : in) {
        data[count++] = i;
    }
}

template<typename T> Reflect<T>::~Reflect() {
    if(data) {
        free(data);
    }
}

template<typename T> Reflect<T>& Reflect<T>::operator=(const Reflect in) {
    if(data) {
        free(data);
    }

    // set
    count     = in.count;
    capacitor = in.capacitor;
    if(count == 0) {
        data = nullptr;
        return *this;
    }

    // new and copy
    data = static_cast<Field*>(malloc(sizeof(Field) * capacitor));
    if(!data) {
        count     = 0;
        capacitor = 0;
        throw std::bad_alloc();
    }
    std::memcpy(data, in.data, sizeof(Field) * capacitor);

    return *this;
}

template<typename T> Reflect<T>& Reflect<T>::operator=(Reflect&& in) noexcept {
    if(this != &in) {
        if(data) {
            free(data);
        }
        data         = in.data;
        count        = in.count;
        capacitor    = in.capacitor;
        in.data      = nullptr;
        in.capacitor = 0;
        in.count     = 0;
    }
    return *this;
}

template<typename T> const T& Reflect<T>::operator[](size_t in) const {
    return data[in];
}

template<typename T> const T* Reflect<T>::begin() const {
    return data;
}

template<typename T> const T* Reflect<T>::end() const {
    return data + count;
}

template<typename T> size_t Reflect<T>::size() const {
    return count;
}

template<typename T> void Reflect<T>::shrink() {
    T* newly = static_cast<T*>(realloc(data, sizeof(T) * count));
    if(!newly) {
        throw std::bad_alloc();
    }
    data      = newly;
    capacitor = count;
}

template<typename T> template<class C> const Reflect<T>& Reflect<T>::reflect() {
    // default
    static Structure EMPTY;
    return EMPTY;
}

template<typename T> template<typename C> const Reflect<T>& Reflect<T>::get() {
    static Reflect statics = reflect<C>();
    return statics;
}

template<typename T> template<typename C> const Reflect<T>& Reflect<T>::get(const C&) {
    return get<C>();
}

template<typename T> const Reflect<T>& Reflect<T>::get(const char* in) {
    return get(string{ in });
}

template<typename T> inline const Reflect<T>& Reflect<T>::get(const string& in) {
    return map[in];
}

template<typename T> template<typename Arg> void Reflect<T>::push(Arg&& in) {
    if(count >= capacitor) {
        capacitor += 256;
        T* newly   = static_cast<T*>(realloc(data, sizeof(T) * capacitor));
        if(!newly) {
            capacitor >>= 1;
            throw std::bad_alloc();
        }
        data = newly;
    }
    new(data + count) T(std::forward<Arg>(in));
    ++count;
}

// clang-format off
template<typename T> constexpr EType typecode() {
    if constexpr(std::is_base_of_v<LWE::stl::Container, T>) return ContainerCode<T>::VALUE;
    if constexpr(std::is_enum_v<T>)                         return EType::ENUM;
    if constexpr(std::is_pointer_v<T>)                      return EType::POINTER;
    if constexpr(std::is_reference_v<T>)                    return EType::REFERENCE;
    if constexpr(std::is_union_v<T>)                        return EType::UNION;
    if constexpr(std::is_class_v<T>)                        return EType::CLASS;
    if constexpr(std::is_void_v<T>)                         return EType::VOID;
    if constexpr(std::is_same_v<T, bool>)                   return EType::BOOL;
    if constexpr(std::is_same_v<T, signed char>)            return EType::SIGNED_CHAR;
    if constexpr(std::is_same_v<T, unsigned char>)          return EType::UNSIGNED_CHAR;
    if constexpr(std::is_same_v<T, char>)                   return EType::CHAR;
    if constexpr(std::is_same_v<T, signed short>)           return EType::SIGNED_SHORT;
    if constexpr(std::is_same_v<T, unsigned short>)         return EType::UNSIGNED_SHORT;
    if constexpr(std::is_same_v<T, signed int>)             return EType::SIGNED_INT;
    if constexpr(std::is_same_v<T, unsigned int>)           return EType::UNSIGNED_INT;
    if constexpr(std::is_same_v<T, signed long>)            return EType::SIGNED_LONG;
    if constexpr(std::is_same_v<T, unsigned long>)          return EType::UNSIGNED_LONG;
    if constexpr(std::is_same_v<T, wchar_t>)                return EType::WCHAR_T;
    if constexpr(std::is_same_v<T, float>)                  return EType::FLOAT;
    if constexpr(std::is_same_v<T, double>)                 return EType::DOUBLE;
    if constexpr(std::is_same_v<T, long double>)            return EType::LONG_DOUBLE;
    if constexpr(std::is_same_v<T, std::string>)            return EType::STD_STRING;
    else                                                    return EType::UNREGISTERED;
}
// clang-format on

// call by template
template<typename T> const char* typestring() {
    Type statics = typeof<T>();
    return statics.stringify();
}

// call by argument
template<typename T> const char* typestring(const T&) {
    return typestring<T>();
}

const char* typestring(const Type& in) {
    return in.stringify();
}

template<typename T> const Type& typeof() {
    return Type::reflect<T>();
}

template<typename T> const Type& typeof(const T&) {
    return Type::reflect<T>();
}

template<typename T> constexpr bool isSTL() {
    return std::is_base_of_v<LWE::stl::Container, T> && ContainerCode<T>::VALUE != EType::UNREGISTERED;
};

template<typename T> constexpr bool isSTL(const T&) {
    return isSTL<T>();
}

template<> bool isSTL<EType>(const EType& code) {
    // const char* name = estring(code);
    // if(name[0] == 'S' && name[1] == 'T' && name[2] == 'L' && name[3] == '_') {
    //     return true; // read 4 byte
    // }
    switch(code) {
        case EType::STL_DEQUE:
            return true;
    }
    return false;
}

template<typename T> T* Register<T>::get(const char* in) {
    return get(string{ in });
}

template<typename T> T* Register<T>::get(const string& in) {
    auto result = registry().find(in);
    if(result != registry().end()) {
        return result->second;
    }
    return nullptr;
}

template<typename T> template<typename U> void Register<T>::set(const char* in) {
    set<U>(string{ in });
}

template<typename T> template<typename U> void Register<T>::set(const string& in) {
    registry().insert({ in, static_cast<T*>(new U()) });
}

template<typename T> Register<T>::~Register() {
    for(auto& it : map) {
        delete it.second;
    }
}

template<typename T> auto Register<T>::registry() -> Map& {
    static Register<T> statics;
    return statics.map;
}

template<typename T> Registered registclass() {
    // default, other class -> template specialization
    Structure::reflect<Object>();
    Register<Object>::set<Object>("Object");
    Register<Class>::set<ObjectMeta>("Object");
    return Registered::REGISTERED;
}

template<typename T> const Object* statics() {
    // default, other class -> specialization
    if constexpr(!std::is_same_v<T, Object>) {
        return nullptr;
    }
    static const Object* statics = Register<Object>::get("Object");
    return statics;
}

template<typename T> const Object* statics(const T&) {
    return statics<T>();
}

const Object* statics(const char* in) {
    return statics(string{ in });
}

const Object* statics(const string& in) {
    if(Object::map.find(in) == Object::map.end()) {
        return nullptr;
    }
    return Object::map[in];
}

template<typename T> Class* metaclass() {
    const Object* ptr = statics<T>();
    if(ptr) {
        return ptr->meta();
    }
    return nullptr;
}

template<typename T> Class* metaclass(const T&) {
    return metaclass<T>();
}

Class* metaclass(const char* in) {
    return metaclass(string{ in });
}

Class* metaclass(const string& in) {
    return Register<Class>::get(in);
}

template<typename T> Enum* metaenum() {
    return nullptr;
}

template<typename T> Enum* metaenum(const T&) {
    return metaenum<T>();
}

Enum* metaenum(const char* in) {
    return metaenum(string{ in });
}

Enum* metaenum(const string& in) {
    return Register<Enum>::get(in);
}

template<typename E> const char* Enum::stringify(E in) {
    static std::unordered_map<E, const char*> cache;

    auto result = cache.find(in);
    if(result != cache.end()) {
        return result->second;
    }

    const Enumerate& reflected = Enumerate::get<E>();
    for(auto i : reflected) {
        if(i.value == static_cast<uint64>(in)) {
            cache[in] = i.name;
            return i.name;
        }
    }
    return "";
}

const char* Enum::stringify(const string& type, uint64 value) {
    const Enumerate& reflected = Enumerate::get(type);
    for(auto i : reflected) {
        if(i.value == value) {
            return i.name;
        }
    }
    return "";
}

const char* Enum::stringify(const char* type, uint64 value) {
    return stringify(string{ type }, value);
}

// clang-format off
constexpr const char* typestring(EType code) {
    switch(code) {
    case EType::UNREGISTERED:       return "";
    case EType::VOID:               return "void";
    case EType::SIGNED_INT:         return "int";
    case EType::SIGNED_CHAR:        return "char";
    case EType::SIGNED_SHORT:       return "short";
    case EType::SIGNED_LONG:        return "long";
    case EType::SIGNED_LONG_LONG:   return "long long";
    case EType::UNSIGNED_SHORT:     return "unsigned short";
    case EType::UNSIGNED_INT:       return "unsigned int";
    case EType::UNSIGNED_CHAR:      return "unsigned char";
    case EType::UNSIGNED_LONG:      return "unsigned long";
    case EType::UNSIGNED_LONG_LONG: return "unsigned long long";
    case EType::BOOL:               return "bool";
    case EType::CHAR:               return "char";
    case EType::WCHAR_T:            return "wchar_t";
    case EType::FLOAT:              return "float";
    case EType::DOUBLE:             return "double";
    case EType::LONG_DOUBLE:        return "long double";
    case EType::CLASS:              return "class";
    case EType::UNION:              return "union";
    case EType::POINTER:            return "*";
    case EType::REFERENCE:          return "&";
    case EType::FUNCTION:           return "function";
    case EType::STD_STRING:         return "string";
    case EType::STL_DEQUE:          return "Deque";
    case EType::CONST:              return "const";
    case EType::ENUM:               return "enum";
    }
    return ""; // error
}

// clang-format on
#endif