// shader_texel_pbr.hlsl

cbuffer camera_constants : register(b0) {
    float4x4 view_projection;
    float4x4 view;
    float4x4 projection;
    float4x4 inv_view;
    float4x4 inv_projection;
    float3 camera_pos;
    float2 window_size;
    float2 time;
}

cbuffer light_constants : register(b1) {
    float4x4 light_view_projection;
    float3 light_pos;
}

cbuffer material_constants {
    int type; // 0 - vertex, 1 - fragment, 2 - texel
    float roughness;
    float metallic;
};

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
    float4 world_position : WORLD_POSITION;
    float4 shadow_position : SHADOW_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

float3 calculate_light(float3 l, float3 n, float3 v) {
    
    float3 h = normalize(l + v);
        
    float n_dot_l = saturate(dot(n, l));
    float n_dot_h = saturate(dot(n, h));
        
    float ambient = 0.01f;
    float diffuse = n_dot_l;
    float specular = 5.0f * pow(n_dot_h, 50.0f) * n_dot_l;

    float final_color = ambient + diffuse + specular;
    return final_color;
}

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.world_position = output.position;
    output.shadow_position = mul(output.position, light_view_projection);
    output.position = mul(view_projection, output.position);
    
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    output.texcoord = input.texcoord;
    output.color = input.color;
    
    // per vertex lighting
    if (type == 0) {
        
        float3 l = normalize(light_pos);
        float3 n = normalize(input.normal);
        float3 v = normalize(camera_pos - output.world_position.xyz);
        float3 color = calculate_light(l, n, v);
        
        output.color = float4(color, 1.0f);
    }
    
    return output;
}

Texture2D color_texture : register(t0);
Texture2D shadow_texture : register(t1);
SamplerState texture_sampler : register(s0);

// texel functions

float4 get_texel_size(Texture2D tex) {
    uint width = 0;
    uint height = 0;
    tex.GetDimensions(width, height);
    return float4(1.0f / width, 1.0f / height, width, height);
}

float3 texel_snap(float3 pos, float2 uv, float4 texel_size) {
    
    float2 center_uv = floor(uv * (texel_size.zw)) / texel_size.zw + (texel_size.xy / 2.0f);
    float2 duv = (center_uv - uv);
    
    float2 duv_ds = ddx_fine(uv);
    float2 duv_dt = ddy_fine(uv);
 
    float2x2 dst_duv = float2x2(duv_dt[1], -duv_dt[0], -duv_ds[1], duv_ds[0]) * (1.0f / (duv_ds[0] * duv_dt[1] - duv_dt[0] * duv_ds[1]));
 
    float2 dst = mul(dst_duv, duv);
    
    
    float3 dxyz_ds = ddx_fine(pos);
    float3 dxyz_dt = ddy_fine(pos);
    
    float3 dxyz = dxyz_ds * dst[0] + dxyz_dt * dst[1];
    dxyz = clamp(dxyz, -1.0f, 1.0f);
    
    float3 snapped_pos = pos + dxyz;
    
    return snapped_pos;
    
}


float4 ps_main(vs_out input) : SV_TARGET {
    
    float4 texture_color = color_texture.Sample(texture_sampler, input.texcoord);
    
    if (type == 0) {
        return texture_color * input.color;
    }
    
    // per pixel lighting
    float3 p = input.world_position.xyz;
    float3 n = normalize(input.normal);
    float3 v = normalize(camera_pos - p);
    
    // per texel lighting
    if (type == 2) {
       
        float4 texel_size = get_texel_size(color_texture);
        p = texel_snap(input.world_position.xyz, input.texcoord, texel_size);
        n = texel_snap(input.normal, input.texcoord, texel_size);
        v = normalize(camera_pos - p);
    }
     
    // pbr lighting
    float3 l = normalize(light_pos);
    float3 light_color = calculate_light(l, n, v);
    
    return float4(texture_color.rgb * light_color, 1.0f);
}
