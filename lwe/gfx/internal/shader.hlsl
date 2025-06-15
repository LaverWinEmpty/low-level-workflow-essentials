//! vertex
struct Vertex {
    float3 pos   : POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD0;
};

//! vertex output
struct Pixel {
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
    float2 uv    : TEXCOORD0;
};

// vertex shader main
Pixel VSMain(Vertex input) {
    Pixel output;

    output.pos   = float4(input.pos, 1.0);
    output.color = input.color;
    output.uv    = input.uv;

    return output;
}

// pixel shader main
float4 PSMain(Pixel input) : SV_TARGET {
    return input.color;
}

cbuffer Transform : register(b0) {
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};

cbuffer Material : register(b1) {
    float4 diffuse;
    float4 specular;
};

cbuffer Frame : register(b2) {
    float4 time;
};