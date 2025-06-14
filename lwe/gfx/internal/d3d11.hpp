#ifndef LWE_gfx_D3D11
#define LWE_gfx_D3D11

#include <d3d11.h>
#include <dxgi.h>
#include <wrl/client.h>

#include "gal.hpp"
#include "../../diag/alert.hpp"

LWE_BEGIN
namespace gfx {
    
class D3D11 {
    template<typename T> using Com = Microsoft::WRL::ComPtr<T>;

public:
    D3D11() = default;
    ~D3D11();

public:
    diag::Alert setup(HWND, const gfx::Screen&);
    diag::Alert reset(const gfx::Screen&);
    
public:
    void shutdown();
    diag::Alert begin(const std::vector<Vertex>&, const std::vector<uint>&);
    diag::Alert end();

public:
    ID3D11Device*        handle() const;
    ID3D11DeviceContext* command() const;
    IDXGISwapChain*      surface() const;

private:
    diag::Alert createDevice();
    diag::Alert createSwapChain();
    diag::Alert createDepthStencilBuffer();
    diag::Alert createRenderTargetView();
    diag::Alert createInputLayout();
    diag::Alert createVertexBuffer();
    diag::Alert createIndexBuffer();

private:
    DXGI_FORMAT setupColorFormat();

private:
    HWND                        hwnd;              //!< window
    Com<ID3D11Device>           device;            //!< logical device
    Com<ID3D11DeviceContext>    context;           //!< device command manager
    Com<IDXGISwapChain>         swapchain;         //!< buffer swap chain
    Com<ID3D11RenderTargetView> rtv;               //!< render target view
    Com<ID3D11DepthStencilView> dsv;               //!< depth and stencil view
    Com<ID3D11Texture2D>        zbuffer;           //!< depth stencil buffer
    Com<ID3D11Buffer>           vertices;          //!< vertex buffer
    Com<ID3D11Buffer>           indexes;           //!< index buffer
    Com<ID3D11InputLayout>      layout;            //!< input layout
    Com<ID3D11VertexShader>     vs;                //!< vertex shader
    Com<ID3D11PixelShader>      ps;                //!< vertex shader
    D3D11_VIEWPORT              viewport = { 0 };  //!< viewport information

public:
    Com<ID3D11RasterizerState> rasterizerState;

private:
    gfx::Screen info;
    struct {
        size_t size  = 1; //!< buffer size
        size_t count = 0; //!< set when begin(), and use when end()
    } index, vertex;
};

}
LWE_END
#include "d3d11.ipp"
#endif LWE_gfx_D3D11