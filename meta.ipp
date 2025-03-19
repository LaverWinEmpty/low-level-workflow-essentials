#include "meta.hpp"
#ifdef LWE_META_HEADER

template<typename T> MetaClass* MetaClass::get() {
    if(std::is_base_of_v<Object, T>) {
        static T dummy;
        return dummy.metaclass();
    }
    return nullptr;
}

template<typename T> MetaClass* MetaClass::get(const T&) {
    return get<T>();
}

MetaClass* MetaClass::get(const char* in) {
    if(registry.find(in) == registry.end()) {
        return nullptr;
    }
    return registry[in];
}

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
        std::memcpy(stack, in.stack, count);
    } else {
        heap = static_cast<EType*>(malloc(sizeof(EType) * in.capacitor));
        if(!heap) {
            count = 0;
            throw std::bad_alloc();
        }
        capacitor = in.capacitor;
        std::memcpy(heap, in.heap, count);
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
        return *stack;
    }
    return *heap;
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

template<typename T> template<typename C> const Reflect<T>& Reflect<T>::get() {
    static Reflect statics = reflect<C>();
    return statics;
}

template<typename T> template<typename C> const Reflect<T>& Reflect<T>::get(const C&) {
    return get<C>();
}

template<typename T> const Reflect<T>& Reflect<T>::get(const char* in) {
    return registry[in];
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
// get type enum
template<typename T> constexpr EType typecode() {
    if constexpr(std::is_base_of_v<LWE::stl::Container, T>) return ContainerCode<T>::VALUE;
    if constexpr(std::is_enum_v<T>)      return EType::ENUM;
    if constexpr(std::is_pointer_v<T>)   return EType::POINTER;
    if constexpr(std::is_reference_v<T>) return EType::REFERENCE;
    if constexpr(std::is_union_v<T>)     return EType::UNION;
    if constexpr(std::is_class_v<T>)     return EType::CLASS;
    return EType::UNREGISTERED;
}
template<> constexpr EType typecode<void>()               { return EType::VOID; }
template<> constexpr EType typecode<signed int>()         { return EType::SIGNED_INT; }
template<> constexpr EType typecode<signed char>()        { return EType::SIGNED_CHAR; }
template<> constexpr EType typecode<signed short>()       { return EType::SIGNED_SHORT; }
template<> constexpr EType typecode<signed long>()        { return EType::SIGNED_LONG; }
template<> constexpr EType typecode<signed long long>()   { return EType::SIGNED_LONG_LONG; }
template<> constexpr EType typecode<unsigned int>()       { return EType::UNSIGNED_INT; }
template<> constexpr EType typecode<unsigned char>()      { return EType::UNSIGNED_CHAR; }
template<> constexpr EType typecode<unsigned short>()     { return EType::UNSIGNED_SHORT; }
template<> constexpr EType typecode<unsigned long>()      { return EType::UNSIGNED_LONG; }
template<> constexpr EType typecode<unsigned long long>() { return EType::UNSIGNED_LONG_LONG; }
template<> constexpr EType typecode<bool>()               { return EType::BOOL; }
template<> constexpr EType typecode<char>()               { return EType::CHAR; }
template<> constexpr EType typecode<wchar_t>()            { return EType::WCHAR_T; }
template<> constexpr EType typecode<float>()              { return EType::FLOAT; }
template<> constexpr EType typecode<double>()             { return EType::DOUBLE; }
template<> constexpr EType typecode<long double>()        { return EType::LONG_DOUBLE; }
template<> constexpr EType typecode<string>()             { return EType::STD_STRING; }
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

template<typename T> MetaEnum* metaenum() {
    return nullptr;
}

template<typename T> MetaEnum* metaenum(const T&) {
    return metaenum<T>();
}

// DOTO: 이거 string을 T로 인식함 특수화 해야됨
MetaEnum* metaclass(const char* in) {
    if(MetaEnum::registry.find(in) == MetaEnum::registry.end()) {
        return nullptr;
    }
    return MetaEnum::registry[in];
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

    // error
    return "";
}

// clang-format on
#endif