#ifndef LWE_OBJECT_PTR
#define LWE_OBJECT_PTR

#include "reflect.hpp"

// TODO:

LWE_BEGIN

class ObjectPtr {
    struct Handler {
        size_t                     owner;
        Object*                    ptr;
        std::unordered_set<size_t> ref;
    };

public:
    ObjectPtr() { id = sid++; }

public:
    template<typename T> ObjectPtr(T* in) {
        handler        = new Handler();
        id             = sid++;
        handler->owner = id;
        handler->ptr   = in;
        handler->ref.insert(id);
    }

    ObjectPtr(const ObjectPtr& in) {
        id      = sid++;         // new
        handler = in.handler;    // reference
        handler->ref.insert(id); // add reference table
    }

    ObjectPtr(ObjectPtr&& in) {
        id         = in.id;      // move
        handler    = in.handler; // move
        in.handler = nullptr;    // lost
        in.id      = 0;          // lost
    }

public:
    operator const Object*() const {
        return handler->ptr; // get const
    }

    /// @brief copy on write
    Object* operator->() {
        if(handler->ref.size() != 1) {
            handler->ref.erase(id);
            // change owner
            if(handler->owner == id) {
                auto itr = handler->ref.begin();
                if(*itr == id) {
                    ++itr; // begin == this -> next
                }
                handler->owner = *itr; // new owner (random)
            }

            Class*  cls  = handler->ptr->meta();         // get meta
            Object* temp = create(handler->ptr->meta()); // new

            handler      = new Handler(); // new
            handler->ptr = temp;          // new
            handler->ref.insert(id);      // add
            handler->owner = id;          // set owner

            return handler->ptr;
        } else return handler->ptr; // owner is this only
    }

    /// @brief copy on write
    Object& operator*() { return *operator->(); }

public:
    ~ObjectPtr() {
        handler->ref.erase(id);
        if(handler->ref.size() == 0) {
            destroy(handler->ptr);
        }
    }


private:
    Handler* handler = nullptr;
    size_t   id      = -1; // plan to change to be unique

private:
    inline static size_t sid = 0; // plan to change to be lock-free manager
};

LWE_END
#endif