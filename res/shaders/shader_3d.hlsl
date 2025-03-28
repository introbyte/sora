// shader_3d.hlsl

cbuffer camera_constants : register(b0) {
    float4x4 view_projection;
    //float4x4 view;
    //float4x4 projection;
    //float4x4 inv_view;
    //float4x4 inv_projection;
    //float2 window_size;
    //float2 time;
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
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.position = mul(output.position, view_projection);
    
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    output.texcoord = input.texcoord;
    
    output.color = input.color;
    
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

float4 ps_main(vs_out input) : SV_TARGET {
    
    // basic lighting
    float3 l = normalize(float3(0.75f, 0.25f, 1.0f));
    float3 n = normalize(input.normal);
    float n_dot_l = max(dot(n, l), 0.0f);
    float3 color = color_texture.Sample(texture_sampler, input.texcoord).rgb;
    
    float diffuse = clamp(n_dot_l, 0.0f, 1.0f);
    
    float3 light_color = color * lerp(float3(0.1f, 0.1f, 0.1f), float3(0.9f, 0.9f, 0.9f), diffuse);
    
    return float4(color * light_color, 1.0f);
}
