#ifdef LWE_CONTAINER

LWE_BEGIN
namespace meta {

template<typename Derived> string Container::stringify(const Derived* in) {
    std::string out;

    // CRTP begin / end
    typename Derived::Iterator curr = in->begin();
    typename Derived::Iterator last = in->end();
    // has data
    if(curr != last) {
        out.append("[");
        // for each
        while(true) {
            serialize(&out, &*curr, typecode<typename Derived::value_type>());
            ++curr;
            if(curr != last) {
                out.append(", ");
            } else break;
        }
        out.append("]");
    } else return "[]";
    return out;
}

template<typename Derived> Derived Container::parse(const string& in) {
    using Element = typename Derived::value_type;
    if(in == "[]") {
        return Derived{}; // empty
    }
    Derived out; // else

    size_t begin = 1;             // "[", ignore 1
    size_t end   = in.size() - 1; // "]", ignore 1
    size_t len   = 0;

    // parsing
    size_t i = begin;
    for(; i < end; ++i, ++len) {
        if constexpr(std::is_same_v<Element, string>) {
            // find <",> but ignore \"
            if(in[i] == '\"' && in[i + 1] == ',', in[i - 1] != '\\') {
                Element data;
                // len + 1: with '\"'
                deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <", >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(isSTL<Element>()) {
            // find <],> but ignore \]
            if(in[i] == ']' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                // len + 1: with ']'
                deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len + 1), typecode<Element>());
                i     += 3; // pass <], >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if constexpr(std::is_same_v<Element, Object> || std::is_base_of_v<Object, Element>) {
            if(in[i] == '}' && in[i + 1] == ',' && in[i - 1] != '\\') {
                Element data;
                deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
                i     += 2; // pass <, >
                begin  = i; // next position
                len    = 0; // next length
                out.push(std::move(data));
            }
        }

        else if(in[i] == ',') {
            Element data;
            deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
            i     += 2; // pass <, >
            begin  = i; // next position
            len    = 0; // next length
            out.push(std::move(data));
        }
    }

    // insert last data
    Element data;
    deserialize(reinterpret_cast<void*>(&data), in.substr(begin, len), typecode<Element>());
    out.push(std::move(data));
    return std::move(out);
}


template<typename T> constexpr bool isSTL() {
    return std::is_base_of_v<LWE::meta::Container, T> && ContainerCode<T>::VALUE != EType::UNREGISTERED;
};

template<typename T> constexpr bool isSTL(const T&) {
    return isSTL<T>();
}

template<> bool isSTL<EType>(const EType& code) {
    switch(code) {
        case EType::STL_DEQUE: return true;
    }
    return false;
}

} // namespace meta
LWE_END
#endif