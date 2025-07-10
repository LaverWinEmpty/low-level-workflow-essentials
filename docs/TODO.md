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
