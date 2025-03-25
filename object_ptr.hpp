#ifndef LWE_OBJECT_PTR
#define LWE_OBJECT_PTR

#include "reflect.hpp"

// TODO:

LWE_BEGIN

// TODO: thread-safty
// TODO: new -> use pool
template<typename T> class Ptr {
    struct Node {
        Node* next = nullptr;
        Node* prev = nullptr;
    };
    class Handler {
    public:
        T*     instance = nullptr;
        Node*  head     = nullptr;
        Node*  owner    = nullptr;
        size_t count    = 0;

    public:
        void push(Node* in) {
            if(count == 0) {
                head  = in;
                owner = in;
            } else {
                in->next   = head; // set next
                head->prev = in;   // set previous
                head       = in;   // push front
            }
            ++count;
        }

        void pop(Node* out) {
            if(out->prev) out->prev->next = out->next; // pop front
            if(out->next) out->next->prev = out->prev; // pop front
            if(owner == out) {
                owner = head; // new owner
            }
            --count;
        }
    };

public:
    Ptr() = default;

    Ptr(T* in) {
        group = new Handler(); // new
        id    = new Node();    // set
        group->push(id);       // add
        group->instance = in;  // create
    }

    Ptr(const Ptr& in): group(in.group) {
        id = new Node(); // set
        group->push(id); // add
    }

    Ptr(Ptr&& in) noexcept: id(in.id), group(in.group) {
        in.id    = 0;
        in.group = nullptr;
    }

    Ptr& operator=(T* in) {
        Handler* newly = new Handler(); // new
        if(group != nullptr) {
            group->pop(id);
            id->prev = nullptr;
            id->next = nullptr;
        } else {
            id = new Node(); // new
        }
        newly->push(id);      // add
        newly->instance = in; // create

        group = newly;
        return this;
    }

    Ptr& operator=(const Ptr& in) {
        group = in.group;   // ref
        id    = new Node(); // set
        group->push(id);    // add
        return *this;
    }

    Ptr& operator=(Ptr&& in) noexcept {
        if(this != &in) {
            id       = in.id;    // move
            group    = in.group; // move
            in.id    = nullptr;  // reset
            in.group = nullptr;  // reset
        }
        return *this;
    }

    ~Ptr() {
        if(group != nullptr) {
            if(group->count == 1) {
                delete group->instance; // delete
                delete group;           // delete
            } else {
                group->pop(id); // unlink
            }
            delete id; // delete
        }
    }

    const T* operator->() const { return group->instance; }

    const T& operator*() const { return *group->instance; }

    T* operator->() { return group->instance; }

    T& operator*() { return *operator->(); }

    /// @brief clone and unshare for fork
    void detach() {
        if(group->count <= 1) {
            return;
        }

        Handler* newly  = new Handler();           // new
        newly->instance = new T(*group->instance); // copy

        id->next = nullptr; // unlink
        id->prev = nullptr; // unlink
        newly->push(id);    // move
        group->pop(id);     // move
        group = newly;      // set
    }

private:
    Handler* group = nullptr;
    Node*    id    = nullptr;
};

LWE_END
#endif