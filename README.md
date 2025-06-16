# Low-level Workflow Essentials
**Low-level Workflow Essentials** (LWE) is a lightweight, modular C++ utility library focused on compile-time reflection, essential utilities, and performance.
> Project by [LaverWinEmpty](https://github.com/LaverWinEmpty)

---

## 🔍 Overview
LWE is a comprehensive utility library providing RTTI-free reflection, smart pointers, containers, and other essential C++ utilities.  
The reflection system is implemented using compile-time macros, achieving near-complete functionality without runtime type information overhead.  
This library aims to be minimal, efficient, and practical for modern C++ development.

---

## 🛠️ Tech Stack

- **Language:** C++17

---

## 💡 Platform

> Tested exclusively on **Windows** with **MSVC** (`/Zc:preprocessor`).
> 
> **Note:** However, this project targets cross-platform support.

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

⚠️ Examples are work-in-progress and may change as the library evolves.

---


## 📋 Development Status

**Core Features**
- ✅ RTTI-free Reflection system
  - ✅ Type reflection and serialization
  - ✅ Enum reflection and serialization
  - ✅ Object reflection and serialization 
  - ✅ Method reflection and invocation
- ✅ Utility Components
  - ✅ Custom Any type (RTTI-free)
  - ✅ Smart pointers
  - ✅ ID/UUID system
  - ✅ Memory pool
- ⚠️ Container system
  - ✅ Container reflection framework
  - ✅ Deque implementation (reference)
  - ❌ Other container types (planned)
- ⚠️ Smart pointer systme
  - ✅ Custom smart pointer with unique/weak pointer semantics
  - ❌ Smart pointer serialization (planned)
  - ❌ Raw pointer handling (not supported by design)

---
