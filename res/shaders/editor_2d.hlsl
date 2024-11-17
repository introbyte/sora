// editor_2d.hlsl

cbuffer globals : register(b0) {
    float2 window_size;
}

struct vs_in {
    float2 pos0 : POS0;
    float2 pos1 : POS1;
    float2 pos2 : POS2;
    float2 pos3 : POS3;
    float4 uv : UV;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float4 col3 : COL3;
    int omit_texture : TEX;
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 uv : UV;
    float2 c_coord : CRD;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 col2 : COL2;
    float4 col3 : COL3;
    int omit_texture : TEX;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    float2 p0 = input.pos0;
    float2 p1 = input.pos1;
    float2 p2 = input.pos2;
    float2 p3 = input.pos3;
    
    // per-vertex arrays
    float2 vertex_pos[] = {
        p0, p1, p2, p3
    };
    
    float2 vertex_uvs[] = {
        float2(input.uv.x, input.uv.w),
        float2(input.uv.z, input.uv.w),
        float2(input.uv.x, input.uv.y),
        float2(input.uv.z, input.uv.y),
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
    output.uv = vertex_uvs[input.vertex_id];
    output.c_coord = vertex_cols[input.vertex_id];
    output.col0 = input.col0;
    output.col1 = input.col1;
    output.col2 = input.col2;
    output.col3 = input.col3;
    output.omit_texture = input.omit_texture;
    
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

float4 ps_main(vs_out input) : SV_TARGET {
    
    float4 color0 = lerp(input.col0, input.col2, input.c_coord.x);
    float4 color1 = lerp(input.col1, input.col3, input.c_coord.x);
    float4 color = lerp(color0, color1, input.c_coord.y);
    
    float4 tex_color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (input.omit_texture != 1) {
        tex_color = color_texture.Sample(texture_sampler, input.uv);
    }
    
    return tex_color * color;
}