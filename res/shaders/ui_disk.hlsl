// ui_disk.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 pos : POS;
    float4 col00 : COL0;
    float4 col01 : COL1;
    float4 col10 : COL2;
    float4 col11 : COL3;
    float2 angles : ANG;
    float2 style : STY; // (thickess, softness)
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 uv : UV;
    float4 col00 : COL0;
    float4 col01 : COL1;
    float4 col10 : COL2;
    float4 col11 : COL3;
    float2 angles : ANG;
    float thickness : THC;
    float softness : SFT;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    // unpack rectangle vertices
    float2 quad_p0 = { input.pos.xy };
    float2 quad_p1 = { input.pos.zw };
    float2 rect_size = abs(quad_p1 - quad_p0);
    
    // per-vertex arrays
    float2 vertex_pos[] = {
        float2(quad_p0.x, quad_p1.y),
        float2(quad_p1.x, quad_p1.y),
        float2(quad_p0.x, quad_p0.y),
        float2(quad_p1.x, quad_p0.y),
    };
    
    float2 vertex_uvs[] = {
        float2(0.0f, 1.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
    };
    
    // fill output
    output.pos = float4(vertex_pos[input.vertex_id] * 2.0f / window_size - 1.0f, 0.0f, 1.0f);
    output.pos.y = -output.pos.y;
    output.half_size = rect_size * 0.5f;
    output.uv = vertex_uvs[input.vertex_id];
    output.col00 = input.col00;
    output.col01 = input.col01;
    output.col11 = input.col11;
    output.col10 = input.col10;
    output.angles = input.angles;
    output.thickness = input.style.x;
    output.softness = input.style.y;
  
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

#define PI 3.14159265359
#define TWO_PI 6.28318530718

float2x2 uv_rotate(float angle) {
    return float2x2(float2(cos(angle), -sin(angle)), float2(sin(angle), cos(angle)));
}

float sdf_circle(float2 p, float r) {
    return (length(p) - r);
}

float2 polar(float2 p) {
    return float2(length(p), atan2(p.y, p.x) / TWO_PI);
}

float4 ps_main(vs_out input) : SV_TARGET {
    
    // sample texture
    float4 tex_color = color_texture.Sample(texture_sampler, input.uv);
    
    // sdf sample
    float2 sample_pos = (2.0f * input.uv - 1.0f) * input.half_size.x;
    
    float disk_s = sdf_circle(sample_pos, input.half_size.x - input.softness * 2.0f);
    float disk_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, disk_s);  
    
    float border_s = sdf_circle(sample_pos, input.half_size.x - input.softness * 2.0f - input.thickness);
    float border_t = smoothstep(0.0f, 2.0f * input.softness, border_s);
    
    float angle_diff = (input.angles.y - input.angles.x); 
    float2 uv = mul(((input.uv - 0.5f) * 2.0f), uv_rotate(input.angles.x));
    float a = atan2(uv.y, uv.x);
    float theta = (a < 0.0) ? (a + TWO_PI) / TWO_PI : a / TWO_PI;
    float bar = step(theta, (angle_diff) / TWO_PI);
    
    // blend color
    float2 polar_uv = frac(polar(uv));
    float2 sample_uv = float2(polar_uv.x, polar_uv.y * (TWO_PI / angle_diff));
    
    float4 top_color = lerp(input.col00, input.col10, sample_uv.x);
    float4 bottom_color = lerp(input.col01, input.col11, sample_uv.x);
    float4 color = lerp(top_color, bottom_color, sample_uv.y);
    
    if (input.thickness == 0.0f) {
        border_t = 1.0f;
    }
    
    float4 final_color = tex_color;
    final_color *= color;
    final_color.a *= disk_t;
    final_color.a *= border_t;
    final_color.a *= bar;
    
    return final_color;
}
