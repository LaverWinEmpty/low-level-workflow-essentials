#ifdef LWE_META_TYPE

#include "../../util/hash.hpp"
#include "feature.hpp"

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
    static Type buffer;
    if(buffer.count() == 0) {
        reflect<T>(&buffer);
        buffer.shrink();
        buffer.hashed = util::Hash(buffer.begin(), buffer.count());
    }
    return buffer;
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

    else if constexpr(std::is_class_v<T> || std::is_enum_v<T>) {
        // get type info
        const char* str = nullptr;
        if constexpr(std::is_class_v<T>) {
            out->push(Keyword::CLASS);
            Class* meta = classof<T>();
            if (!meta) {
                registclass<T>(); // Try register
                meta = classof<T>();
            }
            if(meta) str = meta->name();
        }
        else if constexpr(std::is_enum_v<T>) {
            out->push(Keyword::ENUM);
            Enum* meta = enumof<T>();
            if(!meta) {
                registenum<T>(); // try register
                meta = enumof<T>();
            }
            if (meta) str = meta->name();
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
    } else out->push(typecode<T>()); // primitive
}

Type::Type(const Type& in): counter(in.counter), hashed(in.hashed) {
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

Type::Type(Type&& in) noexcept: counter(in.counter), hashed(in.hashed) {
    if(in.counter < STACK) {
        std::memcpy(stack, in.stack, counter); // copy
    } else {
        heap      = in.heap;
        capacitor = in.capacitor;
        hashed    = in.hashed;

        in.heap      = nullptr;
        in.capacitor = 0;
        in.counter     = 0;
        in.hashed    = 0;
    }
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
    } else {
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
        } else {
            heap      = in.heap;
            capacitor = in.capacitor;

            in.heap      = nullptr;
            in.capacitor = 0;
            in.counter     = 0;
        }
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

Keyword Type::type() const {
    if(counter < STACK) {
        return stack[0] == Keyword::CONST ? stack[1] : stack[0];
    }
    return heap[0] == Keyword::CONST ? heap[1] : heap[0];
}

const char* Type::operator*() const {
    std::function<size_t(string*, const Type&, size_t)> fn = [&fn](string* out, const Type& in, size_t idx) {
        if(idx >= in.count()) {
            return idx;
        }
        // pointer or reference
        if(in[idx] == Keyword::POINTER || in[idx] == Keyword::REFERENCE) {
            fn(out, in, idx + 1); // rec
        }
        // const
        else if(in[idx] == Keyword::CONST) {
            // CONST POINTER ... = ...* const
            if(in[idx + 1] == Keyword::POINTER) {
                size_t last = fn(out, in, idx + 1);
                out->append(" const");
                return last + 1; // ...* const, terminate
            }
            // CONST ... = const ...
            // CONST REFERENCE ...  = const ...&
            else {
                out->append("const ");
                size_t last = fn(out, in, idx + 1);
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

}
LWE_END
#endif