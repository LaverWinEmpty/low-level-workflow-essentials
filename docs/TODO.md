# TODO / Development Notes

## ✅ Reflection System
- [x] RTTI-free `Type` class
- [x] Type registration macro
- [x] Serialize / Deserialize registered types
- [x] `Object` base class for reflection
- [x] Serializable container base + STL-style `Deque`
- [x] Memory pool for `Object`
- [x] Smart pointer
- [ ] Smart pointer inherited pointer for `Object` and serialization
- [ ] Support default constructor in macro (e.g., `Constructor(const MyInitializer&)`)
- [ ] Hash table containers (`Set`, `Map`)
- [ ] O(1) removal array (`Collection`, swap-and-delete, for iteration/traversal)
- [ ] Sort strategy (Merge + Block Merge hybrid)

---

## ✅ Log System
- [x] Custom exception class for logging
- [x] Thread pool using message queue
- [ ] Timer (for timestamps)
- [ ] File system abstraction layer
- [ ] Logging system interface
- [ ] External log viewer / visualization tool

---

## ✅ Render
- [ ] DirectX 11

---

## ✅ Network
- [ ] TCP/UDP socket
- [ ] High-performance I/O (Enhanced `Select` Model (O(1) performance), or Windows IOCP)