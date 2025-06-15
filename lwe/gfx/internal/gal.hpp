#ifndef LWE_GFX_GAL
#define LWE_GFX_GAL

#include "../../diag/diag.h"

LWE_BEGIN
namespace gfx {

//! @brief vertex
struct Vertex {
    float x, y, z;
    float r, g, b, a;
    float u, v;
};

////! @brief constants / uniform buffer
//struct Uniform {
//    float matrix[4][4]; // matrix
//    float color[4];     // color (r, g, b, a)
//    float time[4];      // time with padding
//};

//! @brief screen information structure
struct Screen {
    /// @brief like a enum, uint32 (1, 2, 4, 8, 16)
    struct MSAA {
        enum class Level : uint {
            OFF = 1,
            X2  = 2,
            X4  = 4,
            X8  = 8,
            X16 = 16,
        };

    public:
        static constexpr Level OFF = Level::OFF; //!< using enum
        static constexpr Level X2  = Level::X2;  //!< using enum
        static constexpr Level X4  = Level::X4;  //!< using enum
        static constexpr Level X8  = Level::X8;  //!< using enum
        static constexpr Level X16 = Level::X16; //!< using enum

    public:
        MSAA(Level in) noexcept;                      // setter
        constexpr bool operator==(const MSAA&) const; // compare
        constexpr bool operator!=(const MSAA&) const; // compare
        constexpr operator uint() const;          // getter

    private:
        Level level;
    };

    uint width  = 1280; // screen x
    uint height = 720;  // screen y
    uint fps    = 60;   // frame per sec
    struct {
        MSAA level   = MSAA::OFF;
        uint quality = 0;
    } msaa;

    bool windowed = true; //!< false, full screen
    bool vsync    = false; //!< false, not use
    bool hdr      = false; //!< false, use sdr
};

//! @brief graphics abstraction layer
template<typename T, typename U> class GAL {
public:
    diag::Alert setup(U, const Screen&);
    diag::Alert reset(const Screen&);

public:
    void shutdown();
    void begin();
    void end();
    void present();

public:
    T* instance() const;

private:
    T renderer;
};

}
LWE_END
#include "gal.ipp"
#endif