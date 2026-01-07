# TODO / Development Notes

## ✅ Reflection System

-   [x] RTTI-free `Type` class
-   [x] Type registration macro
-   [x] Serialize / Deserialize registered types
-   [x] `Object` base class for reflection
-   [x] Serializable container base + STL-style `Deque`
-   [x] Memory pool for `Object`
-   [x] Smart pointer
-   [ ] Smart pointer inherited pointer for `Object` and serialization
-   [x] Support default constructor in macro (e.g., `Constructor(const MyInitializer&)`) -> No need
-   [x] Hash table containers (`Set`, `Map`)
-   [ ] O(1) removal array (`Collection`, swap-and-delete, for iteration/traversal)
-   [ ] Sort strategy (Merge + Block Merge hybrid)

---

## ✅ Log System

-   [x] Custom exception class for logging
-   [x] Thread pool using message queue
-   [x] Timer (for timestamps)
-   [x] File system abstraction layer -> No need
-   [x] Logging system interface
-   [ ] External log viewer / visualization tool

---

## Roadmap

### Memory Management

-   [ ] Replace current allocation logic with a memory pool–based approach  
         (reference: https://github.com/LaverWinEmpty/cpp_mem_pool)

### Serialization

-   [ ] Add `serialize()` support to `RC<T>` and `Ptr<T>`

### Language & Undefined Behavior

-   [ ] Remove union-based type punning and unsafe casts  
         to prevent strict-aliasing–related UB

### API & Semantics

-   [ ] Add a generic field access method (based on `Any<T>`)
-   [ ] Clarify pointer-serialization ambiguity by explicitly ignoring pointers

### Extensibility

-   [ ] Improve the design to make it easier to register and support new types
