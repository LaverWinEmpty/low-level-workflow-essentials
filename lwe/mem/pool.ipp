LWE_BEGIN
namespace mem {

void Pool::Block::initialize(Pool* outer, size_t count) noexcept {
    from = outer;
    next = nullptr;
    prev = nullptr;

    uint8_t* data = reinterpret_cast<uint8_t*>(this) + from->META; // pass header
    uint8_t* meta = data - sizeof(void*);                          // pass pointer

    curr = reinterpret_cast<void*>(data); // save

    // write outer end next
    size_t loop = count - 1;
    for(size_t i = 0; i < loop; ++i) {
        *reinterpret_cast<void**>(meta) = reinterpret_cast<void*>(this);               // outer
        *reinterpret_cast<void**>(data) = reinterpret_cast<void*>(data + from->CHUNK); // next

        meta += from->CHUNK;
        data += from->CHUNK;
    }

    *reinterpret_cast<void**>(meta) = reinterpret_cast<void*>(this);
    *reinterpret_cast<void**>(data) = nullptr;

    used = 0;
}

void* Pool::Block::get() noexcept {
    if(curr == nullptr) {
        return nullptr;
    }

    // get current
    void* out = curr;

    // set next
    curr = *reinterpret_cast<void**>(curr);

    ++used;
    return out;
}

void Pool::Block::set(void* in) noexcept {
    if(!in) return;

    // set next
    *(reinterpret_cast<void**>(in)) = curr;

    --used;
    curr = in;
}

bool Pool::Block::full() const noexcept {
    return used == 0;
}

bool Pool::Block::empty() const noexcept {
    return curr == nullptr;
}

auto Pool::Block::find(void* in) noexcept -> Block* {
    if(in) {
        void* ptr = reinterpret_cast<void**>(in) - 1;
        return *reinterpret_cast<Block**>(ptr);
    }
    return nullptr;
}

Pool::Pool(size_t chunk, size_t alignment, size_t count) noexcept:
    ALIGN{ alignment <= sizeof(void*) ? sizeof(void*) : align(alignment) }, // align min: ptr size (64-bits: 8)
    CHUNK{ align(chunk + sizeof(void*), ALIGN) },                           // with ptr for outer block address
    META{ align(sizeof(Block) + sizeof(void*), ALIGN) },                    // padding for first chunk alignment
    COUNT{ count ? count : ((config::MEMORYPAGE - META) / CHUNK) },         // adjust for mem page size
    SRC{ chunk },                                                           // chunk original size backup
    counter{ 0 } { }

Pool::~Pool() noexcept {
    // assert(counter.chunks == 0);
    for(auto i = all.begin(); i != all.end(); ++i) {
        memfree(*i);
    }
}

auto Pool::count() const noexcept -> Counter {
    return counter;
}

template<typename T, typename... Args> T* Pool::allocate(Args&&... args) noexcept {
    if(COUNT <= 1) {
        void* ptr = core::memalloc(SRC, ALIGN);
        if(!ptr) {
            return nullptr;
        }
        if constexpr(!std::is_void_v<T>) {
            new(ptr) T(std::forward<Args>(args)...);
        }
        return static_cast<T*>(ptr);
    }

    T* ret = nullptr;

    if(usable.head == nullptr) {
        generate(); // set new block
    }

    ret = static_cast<T*>(usable.head->get());
    if constexpr(!std::is_void_v<T>) {
        new(ret) T(std::forward<Args>(args)...); // call constructor
    }

    if(usable.head->empty()) {
        usable.dequeue();
    }

    if(ret) {
        ++counter.chunks; // count
    }
    return ret;
}

template<typename T> bool Pool::deallocate(T* in) noexcept {
    if(COUNT <= 1) {
        if(!in) return false;
        if constexpr(!std::is_void_v<T>) {
            in.~T();
        }
        core::memfree(in);
        return true;
    }

    // call destructor
    if constexpr(!std::is_void_v<T>) {
        in->~T();
    }

    // check
    Block* block = Block::find(in);
    if(!block || block->from != this) {
        return false;
    }

    // empty -> usable
    if(block->empty()) {
        usable.enqueue(block);
    }

    // release
    block->set(in);   // back
    --counter.chunks; // count

    if(block->full()) {
        if(block != usable.head) {
            usable.pop(block);       // remove
            freeable.enqueue(block); // if head: keep
            --counter.blocks;        // count
        }
    }
    return true;
}

bool Pool::generate() noexcept {
    if(COUNT <= 1) return true;

    Block* block;
    if(freeable.head) {
        block = freeable.dequeue(); // move
    }

    // new
    else {
        block = static_cast<Block*>(memalloc(META + (CHUNK * COUNT), ALIGN));
        // check
        if(block) {
            block->initialize(this, COUNT);
            counter.generated += 1;
            all.push(block);
        }
        else return false; // failed
    }
    usable.enqueue(block); // set

    counter.blocks += 1;
    return true;
}

size_t Pool::generate(size_t in) noexcept {
    if(COUNT <= 1) return 0;

    for(size_t i = 0; i < in; ++i) {
        if(!generate()) {
            return i;
        }
    }
    return in;
}

size_t Pool::release() noexcept {
    if(COUNT <= 1) return 0;

    size_t i = 0;
    while(freeable.head != nullptr) {
        Block* ptr = freeable.dequeue();
        all.pop(ptr);
        memfree(ptr);
        counter.generated -= 1;
    }
    return i;
}

void Pool::Queue::enqueue(Block* in) noexcept {
    if(!in) {
        // TODO: 
        // assert(in);
        return;
    }

    if(tail) {
        tail->next = in;
        in->prev   = tail;
        in->next   = nullptr;
        tail       = in;
    }

    else {
        head = in;
        tail = in;
    }
}

auto Pool::Queue::dequeue() noexcept -> Block* {
    Block* out = head;
    pop(out);
    return out;
}

void Pool::Queue::pop(Block* inout) noexcept {
    if(!inout) {
        return;
    }

    if(inout == head) {
        head = inout->next;
    }

    if(inout == tail) {
        tail = inout->prev;
    }

    if(inout->next) {
        inout->next->prev = inout->prev;
    }

    if(inout->prev) {
        inout->prev->next = inout->next;
    }

    inout->next = nullptr;
    inout->prev = nullptr;
}

} // namespace mem
LWE_END
