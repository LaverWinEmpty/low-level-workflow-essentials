# Low-level Workflow Essentials

**Low-level Workflow Essentials** (LWE) is **header only** utility library for **C++17** without **RTTI**

## 🎯 Purpose

### Why was this created?

Originally developed as foundational infrastructure for a personal game engine project. While initially focused on my specific engine needs, the project has evolved to serve broader engine development scenarios.

### What does it provide?

This experimental project implements RTTI-free reflection with the primary goal of data serialization and supporting utilities.

**Components:**

-   **Utilities**: Essential features for reflection system and basic engine functionality.
-   **Memory Pool**: Originally planned as separate library, integrated as foundational infrastructure
-   **Containers**: Initially built for serialization purposes, refactored to adapter pattern during development. Now maintained as high-performance alternatives to STL.

## 🛠️ How to use

### Environment

Tested exclusively on **Windows** and **MSVC** with _/Zc:preprocessor_

> **NOTE**: However, this project targets cross-platform support.

### Build

This is a header-only library, so simply include the files you need.

**File Structure Guidelines:**

-   `.hpp` files: Individual modules - include these
-   `.ipp` files: Source implementation - do NOT include directly
-   `.h` files: Aggregated headers with helper APIs for integrated module usage

```
Reference: structure

root/
├─ namespace/     # Mirrors C++ namespaces
│  ├─ internal/   # Internal details (not recommended to include directly)
│  ├─ module.hpp  # Main public interface
│  ├─ module.ipp  # Inline implementation (acts like .cpp)
│  └─ namespace.h # Aggregated public headers
└─ ...
```

**Usage Examples:**

```cpp
#include "dir/module.hpp" // Individual module
#include "dir/dir.h"      // Integrated namespace usage
```

### Quick Start

```cpp
#include "meta/meta.h"  // Adjust path based on your project structure

using namespace LWE;

// sample class
class MyClass: public Object {
    CLASS_BODY(MyClass, Object);
public:
    void set(int in) {
        std::cout << meta()->name() << "::" << "set(" << in << ")\n";
        std::cout << meta()->fields()[0].name << " = " << in << "\n";
        value = in;
    }
private:
    int value = 0;
};

// register class and field (REQUIRED)
REGISTER_FIELD_BEGIN(MyClass) {
    REGISTER_FIELD(value);
}
REGISTER_FIELD_END

// register class (method)
REGISTER_METHOD_BEGIN(MyClass) {
    REGISTER_METHOD(set);
}
REGISTER_METHOD_END;

// sample code
int main() {
    RC<MyClass> rc;
    auto        fn = methodof<MyClass>("set");
    if(fn) {
        fn->invoke(rc.get(), {100});
    }
    return 0;
}
```

**output**

```
MyClass::set(100)
value = 100
```

**Additional Resources:**

-   Executable examples in `test/example/` (`.hpp` files - just include and call)
-   Performance benchmarks in `test/bench/` (results in `docs/benchmark/`)

## 📋 Other

### development state

**Core Features** ✅

-   RTTI-free Reflection system
-   Utility Components (Custom Any, Smart pointers, Memory pool, etc...)
-   Container system with adapter pattern

**Work in Progress** ⚠️

-   Smart pointer register and serialization
-   Enum serialization (structure complete, needs refactoring)
-   Logging system (serializable Object logging)

### License

MIT - see LICENSE file

### Contributing

Issues and pull requests are welcome.
