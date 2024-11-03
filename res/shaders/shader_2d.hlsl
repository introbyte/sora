// shader_2d.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 bbox : BBOX;
    float4 uv : UV;
    float2 p0 : POS0;
    float2 p1 : POS1;
    float2 p2 : POS2;
    float2 p3 : POS3;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float4 col3 : COL3;
    float4 radii : RAD;
    float3 style : STY;
    int type : TYPE; // 0 - quad, 1 - line, 2 - radial, 3 - tri
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 uv : UV;
    int type : TYPE;
    float2 c_coord : CRD;
    float2 p0 : POS0;
    float2 p1 : POS1;
    float2 p2 : POS2;
    float2 p3 : POS3;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float4 col3 : COL3;
    float4 radius : RAD;
    float thickness : THC;
    float softness : SFT;
    float omit_texture : TEX;
    
};

vs_out vs_main(vs_in input) {
    
    vs_out output = (vs_out) 0;
    
    // unpack bounding box
    float2 quad_p0 = { input.bbox.xy };
    float2 quad_p1 = { input.bbox.zw };
    float2 uv_p0 = { input.uv.xy };
    float2 uv_p1 = { input.uv.zw };
    float2 bbox_size = abs(quad_p1 - quad_p0);
    
    // unpack style params
    float thickness = input.style.x;
    float softness = input.style.y;
    float omit_texture = input.style.z;
    
    // per-vertex arrays
    float2 vertex_pos[] = {
        float2(quad_p0.x, quad_p1.y),
        float2(quad_p1.x, quad_p1.y),
        float2(quad_p0.x, quad_p0.y),
        float2(quad_p1.x, quad_p0.y),
    };
    
    float2 vertex_uvs[] = {
        float2(uv_p0.x, uv_p1.y),
        float2(uv_p1.x, uv_p1.y),
        float2(uv_p0.x, uv_p0.y),
        float2(uv_p1.x, uv_p0.y),
    };
    
    float2 vertex_cols[] = {
        float2(0.0f, 1.0f),
        float2(1.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 0.0f),
    };
    
    // fill output
    output.pos = float4(vertex_pos[input.vertex_id] * 2.0f / window_size - 1.0f, 0.0f, 1.0f);
    output.pos.y = -output.pos.y;
    output.half_size = bbox_size * 0.5f;
    output.uv = vertex_uvs[input.vertex_id];
    output.type = input.type;
    output.c_coord = vertex_cols[input.vertex_id];
    output.p0 = input.p0;
    output.p1 = input.p1;
    output.p2 = input.p2;
    output.p3 = input.p3;
    output.col0 = input.col0;
    output.col1 = input.col1;
    output.col2 = input.col2;
    output.col3 = input.col3;
    output.radius = input.radii;
    output.thickness = thickness;
    output.softness = softness;
    output.omit_texture = omit_texture;
    
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

#define PI 3.14159265359
#define TWO_PI 6.28318530718

#define draw_shape_rect 1
#define draw_shape_quad 2
#define draw_shape_line 3
#define draw_shape_circle 4
#define draw_shape_arc 5
#define draw_shape_tri 6


// sdf functions

float sdf_quad(float2 sample_pos, float2 rect_half_size, float4 r) {
    r.xy = (sample_pos.x > 0.0f) ? r.xy : r.zw;
    r.x = (sample_pos.y > 0.0f) ? r.x : r.y;
    float2 q = abs(sample_pos) - rect_half_size + r.x;
    return min(max(q.x, q.y), 0.0f) + length(max(q, 0.0f)) - r.x;
}

float sdf_line(float2 p, float2 a, float2 b) {
    float2 ba = b - a;
    float2 pa = p - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - h * ba);
}

float sdf_circle(float2 p, float r) {
    return (length(p) - r);
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

// helper functions

float2x2 uv_rotate(float angle) {
    return float2x2(float2(cos(angle), -sin(angle)), float2(sin(angle), cos(angle)));
}

float2 polar(float2 p) {
    return float2(length(p), atan2(p.y, p.x) / TWO_PI);
}

float3 barycentric(float2 p, float2 a, float2 b, float2 c) {
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

// types

float4 draw_quad(vs_out input) {
    
    // blend color
    float4 top_color = lerp(input.col0, input.col2, input.c_coord.x);
    float4 bottom_color = lerp(input.col1, input.col3, input.c_coord.x);
    float4 color = lerp(top_color, bottom_color, input.c_coord.y);

    // sdf sample
    float2 sample_pos = (2.0f * input.c_coord - 1.0f) * input.half_size;
    
    float quad_s = sdf_quad(sample_pos, input.half_size - (input.softness * 2.0f), input.radius);
    float quad_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, quad_s);
    
    float border_s = sdf_quad(sample_pos, input.half_size - input.softness * 2.0f - input.thickness, max(input.radius - input.thickness, 0.0f));
    float border_t = smoothstep(0.0f, 2.0 * input.softness, border_s);
    
    if (input.thickness == 0.0f) {
        border_t = 1.0f;
    }
    
    float4 final_color = color;
    final_color.a *= quad_t;
    final_color.a *= border_t;
    
    return final_color;
    
}

float4 draw_line(vs_out input) {
    
    // sdf sample
    float2 sample_pos = (2.0f * input.uv - 1.0f) * input.half_size;
    
    float line_s = sdf_line(sample_pos, input.p0.xy, input.p1.xy) - (input.thickness * 0.25f);
    float line_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, line_s);
    
    float t = length(sample_pos - input.p1.xy) / length(input.half_size * 2.0f);
    float4 color = lerp(input.col0, input.col1, t);
    
    float4 final_color = color;
    final_color.a *= line_t;
    
    return final_color;
}

float4 draw_radial(vs_out input) {
    
    // sdf sample
    float2 sample_pos = (2.0f * input.uv - 1.0f) * input.half_size.x;
    
    float disk_s = sdf_circle(sample_pos, input.half_size.x - input.softness * 2.0f);
    float disk_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, disk_s);
    
    float border_s = sdf_circle(sample_pos, input.half_size.x - input.softness * 2.0f - input.thickness);
    float border_t = smoothstep(0.0f, 2.0f * input.softness, border_s);
    
    float angle_diff = (input.p0.y - input.p0.x);
    float2 uv = mul(((input.uv - 0.5f) * 2.0f), uv_rotate(input.p0.x));
    float a = atan2(uv.y, uv.x);
    float theta = (a < 0.0) ? (a + TWO_PI) / TWO_PI : a / TWO_PI;
    float bar = step(theta, (angle_diff) / TWO_PI);
    
    // blend color
    float2 polar_uv = frac(polar(uv));
    float2 sample_uv = float2(polar_uv.x, polar_uv.y * (TWO_PI / angle_diff));
    
    float4 top_color = lerp(input.col0, input.col2, sample_uv.x);
    float4 bottom_color = lerp(input.col1, input.col3, sample_uv.x);
    float4 color = lerp(top_color, bottom_color, sample_uv.y);
    
    if (input.thickness == 0.0f) {
        border_t = 1.0f;
    }
    
    float4 final_color = color;
    final_color.a *= disk_t;
    final_color.a *= border_t;
    final_color.a *= bar;
    
    return final_color;
}

float4 draw_tri(vs_out input) {
    
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
    
    float4 final_color = color;
    final_color.a *= tri_t;
    final_color.a *= border_t;
    
    return final_color;
}


float4 ps_main(vs_out input) : SV_TARGET { 
    
    float4 final_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float4 tex_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    if (input.omit_texture == 0.0f) {
        tex_color = color_texture.Sample(texture_sampler, input.uv);
    }
    
    switch (input.type) {
        case draw_shape_rect:{
            final_color = draw_quad(input);
            break;
        }
        case draw_shape_line:{
            final_color = draw_line(input);
            break;
        }
        case draw_shape_circle:{
            final_color = draw_radial(input);
            break;
        }
        case draw_shape_tri:{
            final_color = draw_tri(input);
            break;
        }
    }
    
    return tex_color * final_color;
}
