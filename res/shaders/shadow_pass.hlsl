// shadow_pass.hlsl

cbuffer light_constants : register(b0) {
    float4x4 view_projection;
    float3 light_pos;
}

struct vs_in {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct vs_out {
    float4 position : SV_POSITION;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, view_projection);
    
    return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
