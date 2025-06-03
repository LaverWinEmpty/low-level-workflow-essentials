#ifdef LWE_META_TYPE

#include "../util/hash.hpp"

LWE_BEGIN
namespace meta {

void Type::push(Keyword in) {
    size_t next = counter + 1;

    // swap
    if(next == STACK) {
        Keyword buffer[STACK];
        std::memcpy(buffer, stack, STACK);

        capacitor = STACK << 1; // multiple of 2
        heap      = static_cast<Keyword*>(malloc(sizeof(Keyword) * capacitor));
        if(!heap) {
            std::memcpy(stack, buffer, STACK); // rollback
            throw std::bad_alloc();            // failed
        }
        std::memcpy(heap, buffer, STACK);

        heap[counter] = in;
        counter       = next;
    }

    // use heap
    else if(next > STACK) {
        // reallocate
        if(next >= capacitor) {
            capacitor    <<= 1;
            Keyword* newly   = static_cast<Keyword*>(realloc(heap, capacitor));
            if(!newly) {
                capacitor >>= 1;
                throw std::bad_alloc();
            }
            heap = newly;
        }
        heap[counter] = in;
        counter       = next;
    }

    // use stack
    else {
        stack[counter] = in;
        counter        = next;
    }
}

void Type::shrink() {
    if(counter > STACK) {
        Keyword* newly = static_cast<Keyword*>(realloc(heap, sizeof(Keyword) * counter));
        if(!newly) {
            throw std::bad_alloc();
        }
        heap      = newly;
        capacitor = counter;
    }
}

template<typename T> static const Type& Type::reflect() {
    static Registered flag = Registered::UNREGISTERED;
    static Type       buf;
    static string     str;
    static std::mutex mtx;

    if(flag == Registered::UNREGISTERED) {
        LOCKGUARD(mtx) {
            // double- checked
            if(flag == Registered::UNREGISTERED) {
                // generate and caching

                reflect<T>(&buf);
                buf.shrink();
                buf.hashed = util::Hash(buf.begin(), buf.count());
                
                // to string and caching
                stringify(&str, buf, 0);
                buf.str = str.c_str();

                // set on complete
                flag = Registered::REGISTERED;
            }
        }
    }
    return buf;
}

template<typename T> static void Type::reflect(Type* out) {
    if constexpr(std::is_const_v<T>) {
        out->push(Keyword::CONST);
        reflect<typename std::remove_const_t<T>>(out);
    }

    else if constexpr(isSTL<T>()) {
        out->push(ContainerCode<T>::VALUE);
        reflect<typename T::value_type>(out);
    }

    else if constexpr(std::is_pointer_v<T>) {
        // POINTER CONST TYPENAME: const typeanme*
        // CONST POINTER TYPENAME: typename* const
        out->push(Keyword::POINTER);
        reflect<typename std::remove_pointer_t<T>>(out); // dereference
    }

    /**************************************************************************
     * NOTE: EXCEPTION CLASSES
     **************************************************************************/

    // string
    else if constexpr(std::is_same_v<T, std::string>) {
        out->push(Keyword::STD_STRING);
    }

    /**************************************************************************
     * OTHER CLASS
     **************************************************************************/
    else if constexpr(std::is_class_v<T> || std::is_enum_v<T>) {
        // get type info
        const char* str = nullptr;
        if constexpr(std::is_class_v<T>) {
            out->push(Keyword::CLASS);
            Class* info = classof<T>();
            if (!info) {
                registclass<T>(); // Try register
                info = classof<T>();
            }
            if(info) str = info->name();
        }
        else if constexpr(std::is_enum_v<T>) {
            out->push(Keyword::ENUM);
            Enum* info = enumof<T>();
            if(!info) {
                registenum<T>(); // try register
                info = enumof<T>();
            }
            if (info) str = info->name();
        }

        // calculate size
        uint64_t len = 0;
        if(str) {
            len = strlen(str);
        }
        // store
        const char* ptr = reinterpret_cast<const char*>(&len);
        for(int i = 0; i < sizeof(len); ++i) {
            out->push(static_cast<Keyword>(*ptr)); // size
            ++ptr;
        }
        // get name
        for(int i = 0; i < len; ++i) {
            out->push(static_cast<Keyword>(*str)); // name
            ++str;
        }
    }

    else if constexpr(std::is_reference_v<T>) {
        using Temp = typename std::remove_reference_t<T>;
        // CONST REFERENCE TYPENAME
        if(std::is_const_v<Temp>) {
            out->push(Keyword::CONST);
            out->push(Keyword::REFERENCE);
            reflect<typename std::remove_const_t<Temp>>(out);
        } else {
            out->push(Keyword::REFERENCE);
            reflect<Temp>(out); // dereference
        }
    }
    else out->push(typecode<T>()); // primitive
}

Type::Type(const Type& in): counter(in.counter), hashed(in.hashed), str(in.str) {
    if(in.counter < STACK) {
        std::memcpy(stack, in.stack, sizeof(Keyword) * counter);
    } else {
        heap = static_cast<Keyword*>(malloc(sizeof(Keyword) * in.capacitor));
        if(!heap) {
            counter = 0;
            throw std::bad_alloc();
        }
        capacitor = in.capacitor;
        std::memcpy(heap, in.heap, sizeof(Keyword) * counter);
    }
}

Type::Type(Type&& in) noexcept: counter(in.counter), hashed(in.hashed), str(in.str) {
    if(in.counter < STACK) {
        std::memcpy(stack, in.stack, counter); // copy
    } else {
        heap      = in.heap;
        capacitor = in.capacitor;
        hashed    = in.hashed;

        in.heap      = nullptr;
        in.capacitor = 0;
        in.counter   = 0;
        in.hashed    = 0;
    }
    in.str = nullptr; // moved
}

Type::Type(Keyword in): counter(1), hashed(hash()) {
    stack[0] = in;
}

Type::~Type() {
    if(counter >= STACK) {
        free(heap);
    }
}

Type& Type::operator=(const Type& in) {
    Keyword* dest = counter < STACK ? stack : heap; // allocated: use heap
    if(in.counter < STACK) {
        std::memcpy(dest, in.stack, in.counter); // copy stack
    }
    else {
        // reallocation required
        if(in.counter > capacitor) {
            dest = static_cast<Keyword*>(malloc(sizeof(Keyword) * in.counter));
            if(dest) {
                if(counter > STACK) free(heap); // counter is before moving
            } else throw std::bad_alloc();    // error
        }
        std::memcpy(dest, in.heap, in.counter);
        heap      = dest;
        capacitor = in.capacitor;
    }
    counter = in.counter;
    hashed  = in.hashed;
    str     = in.str;
    return *this;
}

Type& Type::operator=(Type&& in) noexcept {
    if(this != &in) {
        if(counter > STACK) {
            free(heap);
        }
        counter  = in.counter;
        hashed = in.hashed;
        if(counter < STACK) {
            std::memcpy(stack, in.stack, counter); // copy
        }
        else {
            heap      = in.heap;
            capacitor = in.capacitor;

            in.heap      = nullptr;
            in.capacitor = 0;
            in.counter     = 0;
        }
        // cached string move
        str = in.str;
        in.str = nullptr;
    }
    return *this;
}

const Keyword& Type::operator[](size_t idx) const {
    if(counter < STACK) {
        return stack[idx];
    } else return heap[idx];
}

Type::operator Keyword() const {
    if(counter == 0) {
        return Keyword::UNREGISTERED;
    }

    if(counter < STACK) {
        if(*stack == Keyword::CONST) {
            return stack[1];
        }
        return *stack;
    }
    if(*heap == Keyword::CONST) {
        return heap[1];
    }
    return *heap;
}

hash_t Type::hash() const {
    return hashed;
}

Keyword Type::code() const {
    if(counter < STACK) {
        return stack[0] == Keyword::CONST ? stack[1] : stack[0];
    }
    return heap[0] == Keyword::CONST ? heap[1] : heap[0];
}

const char* Type::operator*() const {
    return str;
}

bool Type::operator==(const Type& in) const {
    return hash() == in.hash();
}

bool Type::operator!=(const Type& in) const {
    return !operator==(in);
}

bool Type::operator==(Keyword in) const {
    return static_cast<Keyword>(*this) == in;
}

bool Type::operator!=(Keyword in) const {
    return !operator==(in);
}

Type::operator string() const {
    return this->operator*(); // copy
}

const Keyword* Type::begin() const {
    if(counter < STACK) {
        return stack;
    }
    return heap;
}

const Keyword* Type::end() const {
    if(counter < STACK) {
        return stack + counter;
    }
    return heap + counter;
}

size_t Type::count() const {
    return counter;
}

size_t Type::stringify(string* out, const Type& in, size_t idx) {
    if(idx >= in.count()) {
        return idx;
    }
    // pointer or reference
    if(in[idx] == Keyword::POINTER || in[idx] == Keyword::REFERENCE) {
        stringify(out, in, idx + 1); // rec
    }
    // const
    else if(in[idx] == Keyword::CONST) {
        // CONST POINTER ... = ...* const
        if(in[idx + 1] == Keyword::POINTER) {
            size_t last = stringify(out, in, idx + 1);
            out->append(" const");
            return last + 1; // ...* const, terminate
        }
        // CONST ... = const ...
        // CONST REFERENCE ...  = const ...&
        else {
            out->append("const ");
            size_t last = stringify(out, in, idx + 1);
            return last + 1; // const ..., terminate
        }
    }
    // class or enum
    if(in[idx] == Keyword::CLASS || in[idx] == Keyword::ENUM) {
        uint64_t len;
        char*  ptr = reinterpret_cast<char*>(&len);
        for(int i = 0; i < sizeof(len); ++i) {
            ptr[i] = static_cast<uint8_t>(in[idx + 1 + i]); // read size
        }
        if(len) {
            const char* ptr = reinterpret_cast<const char*>(&in[idx + 1 + sizeof(len)]);
            out->append(ptr, len);
            return idx + 1 + sizeof(len) + len;
        }
        out->append(typestring(in[idx])); // size 0 == unregistered type
        return idx + 1 + sizeof(len);
    }
    // primitive
    else {
        out->append(typestring(in[idx]));
    }
    // has template
    if(isSTL(in[idx])) {
        out->append("<");
        stringify(out, in, idx + 1);

        // TODO: map implementation

        out->append(">");
    }

    return idx + 1;
}

const Field& Class::field(const char* name) const {
    return field(string{ name });
}

const Field& Class::field(const string& name) const {
    static const Field failed = {
        .type   = Type{},
        .name   = nullptr,
        .size   = 0,
        .offset = size_t(-1)
    };

    const Structure& temp = fields();
    for(auto& itr : temp) {
        if(itr.name == name) {
            return itr;
        }
    }
    return failed;
}

template<typename E> const char* Enum::serialize(E in) {
    static std::unordered_map<E, const char*> cache;

    auto result = cache.find(in);
    if(result != cache.end()) {
        return result->second;
    }

    const Enumeration& reflected = Enumeration::find<E>();
    for(auto i : reflected) {
        if(i.value == static_cast<uint64_t>(in)) {
            cache[in] = i.name;
            return i.name;
        }
    }
    return "";
}

const char* Enum::serialize(const string& type, uint64_t value) {
    const Enumeration& reflected = Enumeration::find(type);
    for(auto i : reflected) {
        if(i.value == value) {
            return i.name;
        }
    }
    return "";
}

const char* Enum::serialize(const char* type, uint64_t value) {
    return serialize(string{ type }, value);
}

template<typename E> E Enum::deserialize(const char* in) {
    return deserialize(string{ in });
}

template<typename E> E Enum::deserialize(const string& in) {
    std::unordered_map<string, E> cache;

    auto result = cache.find(in);
    if(result != cache.end()) {
        return result->second;
    }

    const Enumeration& reflected = Enumeration::find<E>();
    for(auto i : reflected) {
        if(i.name == in) {
            cache[in] = static_cast<E>(i.value);
            return static_cast<E>(i.value);
        }
    }
    return static_cast<E>(0);
}

uint64_t Enum::deserialize(const string& type, const string& name) {
    const Enumeration& reflected = Enumeration::find(type);
    for(auto i : reflected) {
        if(i.name == name) {
            return i.value;
        }
    }
    return 0;
}

// call by template
template<typename T> const char* typestring() {
    Type statics = typeof<T>();
    return statics.serialize();
}

// call by argument
template<typename T> const char* typestring(const T&) {
    return typestring<T>();
}

const char* typestring(const Type& in) {
    return *in;
}

template<typename T> const Type& typeof() {
    return Type::reflect<T>();
}

template<typename T> const Type& typeof(const T&) {
    return Type::reflect<T>();
}

template<typename T> Class* classof() {
    // default, other class -> template specialization
    if constexpr(!std::is_same_v<T, Object>) {
        return nullptr;
    }

    static Class* meta = Registry<Class>::find("Object");
    return meta;
}

template<typename T> Class* classof(const T&) {
    return classof<T>();
}

Class* classof(const char* in) {
    return classof(string{ in });
}

Class* classof(const string& in) {
    return Registry<Class>::find(in);
}

template<typename T> Enum* enumof() {
    return nullptr;
}

template<typename T> Enum* enumof(const T&) {
    return enumof<T>();
}

Enum* enumof(const char* in) {
    return enumof(string{ in });
}

Enum* enumof(const string& in) {
    return Registry<Enum>::find(in);
}

template<typename T> Object* statics() {
    // default, other class -> specialization
    if constexpr(!std::is_same_v<T, Object>) {
        return nullptr;
    }
    static Object* statics = Registry<Object>::find("Object");
    return statics;
}

template<typename T> Object* statics(const T&) {
    return statics<T>();
}

Object* statics(const char* in) {
    return statics(string{ in });
}

Object* statics(const string& in) {
    return Registry<Object>::find(in);
}

}
LWE_END
#endif