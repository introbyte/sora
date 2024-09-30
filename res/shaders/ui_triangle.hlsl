// ui_triangle.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 pos : POS;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float2 p0 : PNT0;
    float2 p1 : PNT1;
    float2 p2 : PNT2;
    float2 style : STY; // (thickess, softness)
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 uv : UV;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float2 p0 : PNT0;
    float2 p1 : PNT1;
    float2 p2 : PNT2;
    float thickness : THC;
    float softness : SFT;
};

vs_out vs_main(vs_in input) {
    
    vs_out output = (vs_out) 0;
    
    // unpack rectangle vertices
    float2 quad_p0 = { input.pos.xy };
    float2 quad_p1 = { input.pos.zw };
    float2 rect_size = abs(quad_p1 - quad_p0);
    
    // unpack style params
    float thickness = input.style.x;
    float softness = input.style.y;
    
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
    output.col0 = input.col0;
    output.col1 = input.col1;
    output.col2 = input.col2;
    
    output.p0 = input.p0;
    output.p1 = input.p1;
    output.p2 = input.p2;
    
    output.thickness = thickness;
    output.softness = softness;
  
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

float3 barycentric(float2 p , float2 a, float2 b, float2 c) {
    float2 v0 = b - a;
    float2 v1 = c - a;
    float2 v2 = p - a;
    
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0f - v - w;

    return float3(u, v, w);
}

float sdf_triangle(float2 p, float2 p0, float2 p1, float2 p2) {
    
    float2 e0 = p1 - p0;
    float2 e1 = p2 - p1;
    float2 e2 = p0 - p2;

    float2 v0 = p - p0;
    float2 v1 = p - p1;
    float2 v2 = p - p2;

    float2 pq0 = v0 - e0 * clamp(dot(v0, e0) / dot(e0, e0), 0.0, 1.0);
    float2 pq1 = v1 - e1 * clamp(dot(v1, e1) / dot(e1, e1), 0.0, 1.0);
    float2 pq2 = v2 - e2 * clamp(dot(v2, e2) / dot(e2, e2), 0.0, 1.0);
    
    float s = e0.x * e2.y - e0.y * e2.x;
    float2 d = min(min(float2(dot(pq0, pq0), s * (v0.x * e0.y - v0.y * e0.x)),
                       float2(dot(pq1, pq1), s * (v1.x * e1.y - v1.y * e1.x))),
                       float2(dot(pq2, pq2), s * (v2.x * e2.y - v2.y * e2.x)));

    return -sqrt(d.x) * sign(d.y);
}

float4 ps_main(vs_out input) : SV_TARGET {

    // sample texture
    float4 tex_color = color_texture.Sample(texture_sampler, input.uv);
    
    // sdf sample
    float2 sample_pos = (2.0f * input.uv - 1.0f) * input.half_size;
    
    float tri_s = sdf_triangle(sample_pos, input.p0, input.p1, input.p2) - (input.softness * 2.0f);
    float tri_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, tri_s);
    
    float border_s = sdf_triangle(sample_pos, input.p0, input.p1, input.p2) - (input.softness * 2.0f - input.thickness);
    float border_t = smoothstep(0.0f, 2.0f * input.softness, border_s);
    
    if (input.thickness == 0.0f) {
        border_t = 1.0f;
    }
    
    // blend color
    float3 weights = barycentric(sample_pos, input.p0, input.p1, input.p2);
    float4 color = (weights.x * input.col0) + (weights.y * input.col1) + (weights.z * input.col2);
    
    float4 final_color = tex_color;
    final_color = color;
    final_color.a *= tri_t;
    final_color.a *= border_t;
    
    return final_color;
}
