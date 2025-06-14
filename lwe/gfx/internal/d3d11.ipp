#ifdef LWE_gfx_D3D11

#include <d3dcompiler.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib") 

LWE_BEGIN
namespace gfx {

D3D11::~D3D11() {
    shutdown();
}

diag::Alert D3D11::setup(HWND in, const gfx::Screen& param) {
    hwnd = in;
    info = param; // set

    diag::Alert result;

    result = createDevice();
    if(result != S_OK) {
        return result;
    }

    result = createSwapChain();
    if(result != S_OK) {
        return result;
    }

    result = createDepthStencilBuffer();
    if(result != S_OK) {
        return result;
    }

    result = createInputLayout();
    if (result != S_OK) {
        return result;
    }

    D3D11_RASTERIZER_DESC rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;   // 솔리드 채우기
    rastDesc.CullMode = D3D11_CULL_NONE;    // 컬링 없음 (테스트용)
    rastDesc.FrontCounterClockwise = FALSE; // 시계방향이 앞면
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0.0f;
    rastDesc.SlopeScaledDepthBias = 0.0f;
    rastDesc.DepthClipEnable = TRUE;
    rastDesc.ScissorEnable = FALSE;
    rastDesc.MultisampleEnable = FALSE;
    rastDesc.AntialiasedLineEnable = FALSE;

    result = device->CreateRasterizerState(&rastDesc, rasterizerState.GetAddressOf());
    if (FAILED(result)) {
        return diag::Alert(result);
    }

    return createRenderTargetView();
}

diag::Alert D3D11::reset(const gfx::Screen& param) {
    // check main options
    if(info.width      == param.width    &&
    info.height     == param.height   &&
    info.hdr        == param.hdr      &&
    info.windowed   == param.windowed &&
    info.msaa.level == param.msaa.level
    ) return S_OK;

    Screen old         = info;               // store
    info               = param;              // set
    DXGI_FORMAT format = setupColorFormat(); // update

    // reset
    rtv.Reset();
    dsv.Reset();
    swapchain.Reset();

    HRESULT result = createSwapChain();
    if(result != S_OK) {
        return result;
    }
    result = createDepthStencilBuffer();
    if (result != S_OK) {
        return result;
    }
    return createRenderTargetView();
}

void D3D11::shutdown() {
    rtv.Reset();
    dsv.Reset();
    swapchain.Reset();
    device.Reset();
}

diag::Alert D3D11::begin(const std::vector<Vertex>& vertexList, const std::vector<uint>& indexList) {
    // check
    if (!indexList.empty()) {
        uint maxIndex = *std::max_element(indexList.begin(), indexList.end());
        if (maxIndex >= vertexList.size()) {
            return diag::Alert("INDEX OUT OF RANGE");
        }
    }

    // check vertex buffer size
    vertex.count = 0;
    if (vertex.size < vertexList.size()) {
        do {
            vertex.size <<= 1;
        } while (vertex.size < vertexList.size());

        vertices.Reset();
        diag::Alert alert = createVertexBuffer();
        if (FAILED(alert)) {
            return alert;
        }
    }
    vertex.count = vertexList.size();

    // check index buffer size
    index.count = 0;
    if (index.size < indexList.size()) {
        do {
            index.size <<= 1;
        } while (index.size < indexList.size());

        indexes.Reset();

        diag::Alert alert = createIndexBuffer();
        if (FAILED(alert)) {
            return alert;
        }
    }
    index.count = indexList.size();

    // memcpy
    context->UpdateSubresource(vertices.Get(), 0, nullptr, vertexList.data(), 0, 0);
    context->UpdateSubresource(indexes.Get(), 0, nullptr, indexList.data(), 0, 0);

    return S_OK;
}

diag::Alert D3D11::end() {
    // TODO: blue (temp)
    float fill[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

    // resteriszer state
    context->RSSetState(rasterizerState.Get());

    // output manager set render target
    context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    context->RSSetViewports(1, &viewport);
    context->ClearRenderTargetView(rtv.Get(), fill);
    context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

    // set shader and input layout
    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);
    context->IASetInputLayout(layout.Get());
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    uint stride = sizeof(Vertex);
    uint offset = 0;

    // input assembler buffer binding
    context->IASetVertexBuffers(0, 1, vertices.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(indexes.Get(), DXGI_FORMAT_R32_UINT, 0); // index uint32_t

    // draw
    context->DrawIndexed(index.count, 0, 0);

    // swap chain
    return swapchain->Present(info.vsync, 0);
}

ID3D11Device* D3D11::handle() const {
    return device.Get();
}

ID3D11DeviceContext* D3D11::command() const {
    return context.Get();
}

IDXGISwapChain* D3D11::surface() const {
    return swapchain.Get();
}


diag::Alert D3D11::createDevice() {
    UINT flag
#if !NDEBUG
        = D3D11_CREATE_DEVICE_DEBUG;
#else
        = 0;
#endif

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1, // use DirectX 11.1 WDDM 1.2 
        D3D_FEATURE_LEVEL_11_0, // use DirectX 11.0 WDDM 1.1
        D3D_FEATURE_LEVEL_10_1, // use DirectX 10.1 WDDM 1.0
        D3D_FEATURE_LEVEL_10_0, // use DriectX 10.0 WDDM 1.0 
        D3D_FEATURE_LEVEL_9_3,  // use DirectX 11.0 WDDM 1.0 (DirextX 9 emulate)
        D3D_FEATURE_LEVEL_9_2,  // use DirectX 11.0 WDDM 1.0 (DirextX 9 emulate)
        D3D_FEATURE_LEVEL_9_1,  // use DirectX 11.0 WDDM 1.0 (DirextX 9 emulate)
    };

    D3D_FEATURE_LEVEL level = *levels;

    // create device
    HRESULT result = D3D11CreateDevice(
        nullptr,                           // use default adapter
        D3D_DRIVER_TYPE_HARDWARE,          // hardware acceleration
        nullptr,                           // software rasterizer null (not use cpu)
        flag,                              // (debug) flag
        levels,                            // feauter level list
        sizeof(levels) / sizeof(*levels),  // feauter level count
        D3D11_SDK_VERSION,                 // SDK version
        device.GetAddressOf(),             // out: device
        &level,                            // out: feauter level
        context.GetAddressOf()             // out: device context
    );

    if(FAILED(result)) {
        return diag::Alert(result);
    }
    return diag::Alert(S_OK);
}

diag::Alert D3D11::createSwapChain() {
    DXGI_SWAP_CHAIN_DESC param = { };
    DXGI_FORMAT          format = setupColorFormat();

    // set buffer info
    param.BufferDesc.Width                   = info.width;
    param.BufferDesc.Height                  = info.height;
    param.BufferDesc.RefreshRate.Numerator   = info.fps;
    param.BufferDesc.RefreshRate.Denominator = 1;
    param.BufferDesc.Format                  = info.hdr ?
        DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;

    // set anti-aliasing
    param.SampleDesc.Count   = info.msaa.level; // mass level
    param.SampleDesc.Quality = info.msaa.quality;

    // set buffer
    param.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT; // buffer for render
    param.BufferCount  = 1;                               // buffer count
    param.OutputWindow = hwnd;                            // window handle
    param.Windowed     = info.windowed;
    param.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
    param.Flags        = 0;

    // dxgi factory
    Com<IDXGIDevice>  dxgiDevice  = nullptr;
    Com<IDXGIAdapter> dxgiAdapter = nullptr;
    Com<IDXGIFactory> dxgiFactory = nullptr;

    HRESULT result;

    // get device from d3d device (via QueryInterface to check compatibility)
    result = device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
    if(FAILED(result)) {
        return diag::Alert(result);
    }

    // get parent adapter from device
    result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
    if(FAILED(result)) {
        dxgiDevice->Release(); // free
        return diag::Alert(result);
    }

    // get parent factory from adapter
    result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    if(FAILED(result)) {
        dxgiAdapter->Release();
        dxgiDevice->Release();
        return diag::Alert(result);
    }

    result = dxgiFactory->CreateSwapChain(device.Get(), &param, &swapchain);
    if(FAILED(result)) {
        info.hdr                = false;                                         // hdr -> sdr
        param.BufferDesc.Format = setupColorFormat();                            // reset
        result = dxgiFactory->CreateSwapChain(device.Get(), &param, &swapchain); // try
    }

    if(FAILED(result)) {
        return diag::Alert(result);
    }
    return diag::Alert(S_OK);
}

diag::Alert D3D11::createDepthStencilBuffer() {
    // set
    D3D11_TEXTURE2D_DESC texture = { };
    texture.Width              = info.width;
    texture.Height             = info.height;
    texture.MipLevels          = 1;                             // no mip map
    texture.ArraySize          = 1;                             // single texture
    texture.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT; // 24 bits depth + 8 bits stencil
    texture.SampleDesc.Count   = info.msaa.level;               // pre-set level
    texture.SampleDesc.Quality = info.msaa.quality;             // pre-set quality
    texture.Usage              = D3D11_USAGE_DEFAULT;           // gpu read/write, no cpu access
    texture.BindFlags          = D3D11_BIND_DEPTH_STENCIL;      // bind as depth/stencil buffer
    texture.CPUAccessFlags     = 0;
    texture.MiscFlags          = 0;

    // create depth/stencil buffer texture (image)
    HRESULT result = device->CreateTexture2D(&texture, nullptr, &zbuffer);
    if(FAILED(result)) {
        return diag::Alert(result);
    }

    // create depth/stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC view = { };
    view.Format             = texture.Format;                // using texture setting
    view.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D; // 2d view
    view.Texture2D.MipSlice = 0;                             // no mip map

    // dcreate depth/stencil buffer
    result = device->CreateDepthStencilView(zbuffer.Get(), &view, dsv.GetAddressOf());
    if(FAILED(result)) {
        return diag::Alert(result);
    }
    return diag::Alert(S_OK);
}

diag::Alert D3D11::createRenderTargetView() {
    ID3D11Texture2D* back = nullptr;

    // get back buffer swap chain
    HRESULT result = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back);
    if(FAILED(result)) {
        return diag::Alert(result);
    }

    // get backbuffer size
    D3D11_TEXTURE2D_DESC param;
    back->GetDesc(&param);

    // create
    result = device->CreateRenderTargetView(back, nullptr, rtv.GetAddressOf());
    back->Release(); // ref counter -=1
    if(FAILED(result)) {
        return diag::Alert(result);
    }

    // set viewport
    viewport.Width    = float(param.Width);
    viewport.Height   = float(param.Height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    context->RSSetViewports(1, &viewport);

    return S_OK;
}

diag::Alert D3D11::createInputLayout() {
    Com<ID3DBlob> vsBlob  = nullptr;
    Com<ID3DBlob> vsError = nullptr;
    Com<ID3DBlob> psBlob  = nullptr;
    Com<ID3DBlob> psError = nullptr;

    UINT flag =
#if NDEBUG
        0;
#else
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    HRESULT result;

    LPCWSTR filename = L"C:/Git/working/lwe/gfx/internal/temp.hlsl";
    
    // vertex shader compile
    result = D3DCompileFromFile(
        filename, nullptr, nullptr, "VSMain", "vs_5_0",
        flag, 0, &vsBlob, &vsError);
    if(FAILED(result)) {
        return result;
    }

    // pixel shader compile
    result = D3DCompileFromFile(
        filename, nullptr, nullptr, "PSMain", "ps_5_0",
        flag, 0, &psBlob, &psError
    );
    if(FAILED(result)) {
        return result;
    }

    // create vertex shader
    result = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        nullptr, vs.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }

    // create pixel shder
    result = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(),
        nullptr, ps.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }

    // vertex info
    D3D11_INPUT_ELEMENT_DESC param[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    // { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }

    // create input layout
    result = device->CreateInputLayout(
        param,
        sizeof(param) / sizeof(*param),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        layout.GetAddressOf()
    );
    if (FAILED(result)) {
        return result;
    }
    return S_OK;
}

diag::Alert D3D11::createVertexBuffer() {
    D3D11_BUFFER_DESC param = { };
    param.ByteWidth      = sizeof(Vertex) * vertex.size; // size
    param.BindFlags      = D3D11_BIND_VERTEX_BUFFER;     // set vertex buffer
    param.Usage          = D3D11_USAGE_DEFAULT;          // dynamic
    param.CPUAccessFlags = 0;                            // pooling

    return device->CreateBuffer(&param, nullptr, &vertices); // not init
}

diag::Alert D3D11::createIndexBuffer() {
    D3D11_BUFFER_DESC param = { };
    param.ByteWidth      = sizeof(uint) * index.size; // size
    param.BindFlags      = D3D11_BIND_INDEX_BUFFER;   // set vertex buffer
    param.Usage          = D3D11_USAGE_DEFAULT;       // dynamic
    param.CPUAccessFlags = 0;                         // pooling

    return device->CreateBuffer(&param, nullptr, &indexes); // not init
}


DXGI_FORMAT D3D11::setupColorFormat() {
    DXGI_FORMAT format = info.hdr ? DXGI_FORMAT_R10G10B10A2_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
    if(!device) {
        return format;
    }

    if(info.msaa.level != Screen::MSAA::OFF) {
        UINT quality;
        // get max mass level
        HRESULT result = device->CheckMultisampleQualityLevels(format, info.msaa.level, &quality);
        if(result == S_OK) {
            info.msaa.quality = quality - 1;
        }
        else info.msaa.quality = 0;
    }
    else info.msaa.quality = 0;

    return format;
}

}
LWE_END
#endif