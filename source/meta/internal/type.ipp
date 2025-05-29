#ifdef LWE_META_TYPE

#include "../../util/hash.hpp"
#include "feature.hpp"

LWE_BEGIN
namespace meta {

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

void Type::shrink() {
    if(count > STACK) {
        EType* newly = static_cast<EType*>(realloc(heap, sizeof(EType) * count));
        if(!newly) {
            throw std::bad_alloc();
        }
        heap      = newly;
        capacitor = count;
    }
}

template<typename T> static const Type& Type::reflect() {
    static Type buffer;
    if(buffer.size() == 0) {
        reflect<T>(&buffer);
        buffer.shrink();
        buffer.hashed = util::Hash(buffer.begin(), buffer.size());
    }
    return buffer;
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

    else if constexpr(std::is_class_v<T> || std::is_enum_v<T>) {
        // get type info
        const char* str = nullptr;
        if constexpr(std::is_class_v<T>) {
            out->push(EType::CLASS);
            Class* meta = classof<T>();
            if (!meta) {
                registclass<T>(); // Try register
                meta = classof<T>();
            }
            if(meta) str = meta->name();
        }
        else if constexpr(std::is_enum_v<T>) {
            out->push(EType::ENUM);
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
            out->push(static_cast<EType>(*ptr)); // size
            ++ptr;
        }
        // get name
        for(int i = 0; i < len; ++i) {
            out->push(static_cast<EType>(*str)); // name
            ++str;
        }
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

Type& Type::operator=(const Type& in) {
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

const char* Type::operator*() const {
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
        // class or enum
        if(in[idx] == EType::CLASS || in[idx] == EType::ENUM) {
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

}
LWE_END
#endif