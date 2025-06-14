#ifndef LWE_GDI_HEADER
#define LWE_GDI_HEADER

LWE_BEGIN
namespace gfx {

/*
 * GAL
 */

template<typename T, typename U>
diag::Alert GAL<T, U>:: setup(U handle, const Screen& value) {
    return renderer.setup(handle, value);
}

template<typename T, typename U>
diag::Alert GAL<T, U>::reset(const Screen& value) { return renderer.reset(value); }

template<typename T, typename U>
void GAL<T, U>::shutdown() { renderer.shutdown(); }

template<typename T, typename U>
void GAL<T, U>::begin() { renderer.begin(); }

template<typename T, typename U>
void GAL<T, U>::end() { renderer.end(); }

template<typename T, typename U>
void GAL<T, U>::present() { renderer.present(); }

template<typename T, typename U>
T* GAL<T, U>::instance() const { return &renderer; }

/*
 * Screen
 */

Screen::MSAA::MSAA(Level in) noexcept : level(in) {}

constexpr bool Screen::MSAA::operator==(const MSAA& in) const { return level == in.level; }

constexpr bool Screen::MSAA::operator!=(const MSAA& in) const { return level != in.level; }

constexpr Screen::MSAA::operator uint() const { return uint(level); }

}
LWE_END
#endif