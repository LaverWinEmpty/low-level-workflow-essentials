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

⚠️ Examples are work-in-progress and may change as the engine evolves.

---


## 📋 Development Status

**Implemented:**
- RTTI-free Reflection system (~80%)
  - [x] Basic type reflection and serialization
  - [x] Enum type reflection and serialization 
  - [-] Object type rfelection and serialization (Need: default constructor, pointer serialization)
  - [-] Custom container type reflection and serialization (current implementation Deque only)

**Planned:**
- Custom async IO API
- Custom rendering API

---
