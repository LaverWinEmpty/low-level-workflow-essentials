# LiteWindowsEngine

**LiteWindowsEngine** (LWE) is a lightweight, modular C++ engine focused on performance, low-level reflection, and future scalability.

> Project by [LaverWinEmpty](https://github.com/LaverWinEmpty)

---

## 🔍 Overview

LiteWindowsEngine is a personal engine project aiming to be minimal, efficient, and practical.  
Reflection is already implemented at around 80% without relying on RTTI.  
The current goal is to build a realistic engine structure that is **thread-safe** and **performance-oriented**, even if it's mainly for learning.

---

## 🛠️ Tech Stack

- **Language:** C++20 (only)
- **Core Features:**
  - RTTI-free Reflection system
  - Planned features include a custom `select(fd_set)` implementation or IOCP-based async system, and a D3D11-based rendering API

---

## 💡 Platform

> **Note:** Cross-platform support is under consideration.  
> However, currently, **only Windows** is supported and tested.  
> Hence the name **LiteWindowsEngine**.

---

## 📁 Project Structure

```
root/
├─ namespace/               # Mirrors C++ namespaces
│  ├─ internal/             # Internal details (not recommended to include directly)
│  ├─ module.hpp            # Main public interface
│  ├─ module.ipp            # Inline implementation (acts like .cpp)
│  └─ namespace.h           # Aggregated public headers
└─ ...
```

- To use only reflection:
  ```cpp
  #include "root/meta/meta.h"
  ```
- General module usage:
  ```cpp
  #include "module.hpp"
  ```

---

## 🧪 Usage Examples

> Usage examples are available in the test/ directory.

- Each example is defined in a header file:
  ```
  test/
  └─ example_name.hpp
  ```

- To run an example, simply include the header and call:
  ```
  test::example_name();
  ```

⚠️ Examples are work-in-progress and may change as the engine evolves.

---

## 🚧 Status

- Reflection: ~80% implemented  
- IOCP / Networking / System-level components: not yet implemented  
- This is a **personal learning project**, but built to a high standard of safety and performance.  

---
