// shader_3d.hlsl

cbuffer camera_constants : register(b0) {
    float4x4 view_projection;
    float4x4 view;
    float4x4 projection;
    float4x4 inv_view;
    float4x4 inv_projection;
    float2 window_size;
    float2 time;
}

cbuffer light_constants : register(b1) {
    float4x4 light_view_projection;
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
    float4 light_position : LIGHT_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.light_position = mul(output.position, light_view_projection);
    output.position = mul(output.position, view_projection);
    
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    output.texcoord = input.texcoord;
    
    output.color = input.color;
    
    return output;
}

Texture2D color_texture : register(t0);
Texture2D shadow_texture : register(t1);
SamplerState texture_sampler : register(s0);

float sample_shadow_map(float2 uv) {
    return shadow_texture.Sample(texture_sampler, uv).r;
}

float shadow_pcf(float4 light_position, float n_dot_l) {
    
    float2 shadow_uv = light_position.xy / light_position.w * 0.5f + 0.5f;
    float shadow_depth = light_position.z / light_position.w;
    shadow_uv.y = 1.0f - shadow_uv.y;
    
    float shadow = 0.0f;
    float bias = 0.00001 * tan(acos(n_dot_l));
    bias = clamp(bias, 0, 0.001);
    float filter_radius = 1.0f / 4096.0f;
    
    [unroll]
    for (int x = -2; x <= 2; ++x) {
        [unroll]
        for (int y = -2; y <= 2; ++y) {
            float2 offset = filter_radius * float2(x, y);
            float shadow_sample = sample_shadow_map(shadow_uv + offset);
            shadow += (shadow_depth - bias) > shadow_sample ? 1.0f : 0.0f;
        }
    }
    
    return clamp(1.0f - (shadow / 25.0f), 0.0f, 1.0f);
}

float4 ps_main(vs_out input) : SV_TARGET {
    
    // basic lighting
    float3 l = normalize(light_pos);
    float3 n = normalize(input.normal);
    float n_dot_l = max(dot(n, l), 0.0f);
    float3 color = color_texture.Sample(texture_sampler, input.texcoord).rgb;
    
    // shadow
    float shadow_factor = shadow_pcf(input.light_position, n_dot_l);
    
    float diffuse = clamp(n_dot_l * shadow_factor, 0.2f, 1.0f);
    
    return float4(color * diffuse, 1.0f);
}
