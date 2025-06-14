// vertex shader main
float4 VSMain(float3 pos : POSITION) : SV_POSITION {
    return float4(pos.x, pos.y, pos.z, 1.0);
}

// pixel shader main
float4 PSMain() : SV_TARGET {
    return float4(1.0, 0.0, 0.0, 1.0); // red
}