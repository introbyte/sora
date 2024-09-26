// ui_text.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 pos : POS;
    float4 uv : UV;
    float4 col : COL;
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 uv : UV;
    float2 c_coord : CRD;
    float4 col : COL;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    // unpack rectangle vertices
    float2 quad_p0 = { input.pos.xy };
    float2 quad_p1 = { input.pos.zw };
    float2 uv_p0 = { input.uv.xy };
    float2 uv_p1 = { input.uv.zw };
    
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
    output.uv = vertex_uvs[input.vertex_id];
    output.c_coord = vertex_cols[input.vertex_id];
    output.col = input.col;
  
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

float4 ps_main(vs_out input) : SV_TARGET {
    float4 tex_color = color_texture.Sample(texture_sampler, input.uv);
    return tex_color * input.col;
}
