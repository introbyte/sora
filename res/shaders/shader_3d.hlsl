// shader_3d.hlsl

cbuffer camera_constants : register(b0) {
    float4x4 view_projection;
    float4x4 shadow_view_projection;
    float4x4 view;
    float4x4 projection;
    float4x4 inv_view;
    float4x4 inv_projection;
    float2 window_size;
    float time;
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
    float4 shadow_position : SHADOW;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.shadow_position = mul(output.position, shadow_view_projection);
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
SamplerState nearest_sampler : register(s0);
SamplerState linear_sampler : register(s1);

float4 ps_main(vs_out input) : SV_TARGET {
    
    float3 shadow_clip_space = input.shadow_position.xyz / input.shadow_position.w;
    
    float shadow = 0;
    float2 shadowmap_sample_pos_center = float2((shadow_clip_space.x + 1) / 2, 1 - (shadow_clip_space.y + 1) / 2);
    float shadow_sum = 0;
    float shadow_count = 0;
    for (int x = -2; x <= 2; x += 1) {
        for (int y = -2; y <= 2; y += 1) {
            float shadowmap_sample = shadow_texture.Sample(linear_sampler, shadowmap_sample_pos_center + float2(x / 4096.0f, y / 4096.0f)).r;
            float difference_from_real_depth = shadow_clip_space.z - shadowmap_sample;
            if (difference_from_real_depth > 0.0001f) {
                shadow_sum += 1.f;
            }
            shadow_count += 1;
        }
    }
    shadow = shadow_sum / shadow_count;

    
    float3 l = normalize(float3(10.0f, 15.0f, 7.0f));
    float3 n = normalize(input.normal);
    
    float n_dot_l = max(dot(n, l), 0.2f);
    
    float3 color = color_texture.Sample(linear_sampler, input.texcoord).rgb;
        
    return float4(color * n_dot_l * (1.0f - shadow), 1.0f);
}
