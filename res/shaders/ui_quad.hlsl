// ui_quad.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 pos : POS;
    float4 uv : UV;
    float4 col00 : COL0;
    float4 col01 : COL1;
    float4 col10 : COL2;
    float4 col11 : COL3;
    float4 radii : RAD;
    float4 style : STY; // (thickess, softness, unused, unused)
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 uv : UV;
    float2 c_coord : CRD;
    float4 col00 : COL0;
    float4 col01 : COL1;
    float4 col10 : COL2;
    float4 col11 : COL3;
    float4 radius : RAD;
    float thickness : THC;
    float softness : SFT;
};

vs_out vs_main(vs_in input) {
    
    vs_out output = (vs_out) 0;
    
    // unpack rectangle vertices
    float2 quad_p0 = { input.pos.xy };
    float2 quad_p1 = { input.pos.zw };
    float2 uv_p0 = { input.uv.xy };
    float2 uv_p1 = { input.uv.zw };
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
    
    //float vertex_radii[] = {
    //    input.radii.x,
    //    input.radii.y,
    //    input.radii.z,
    //    input.radii.w
    //};
    
    // fill output
    output.pos = float4(vertex_pos[input.vertex_id] * 2.0f / window_size - 1.0f, 0.0f, 1.0f);
    output.pos.y = -output.pos.y;
    output.half_size = rect_size * 0.5f;
    output.uv = vertex_uvs[input.vertex_id];
    output.c_coord = vertex_cols[input.vertex_id];
    output.col00 = input.col00;
    output.col01 = input.col01;
    output.col11 = input.col11;
    output.col10 = input.col10;
    //output.radius = vertex_radii[input.vertex_id];
    output.radius = input.radii;
    output.thickness = thickness;
    output.softness = softness;
  
    return output;
}

Texture2D color_texture : register(t0);
SamplerState texture_sampler : register(s0);

float4 oklab_lerp(float4 colA, float4 colB, float h) {
    float3x3 kCONEtoLMS = float3x3(
         0.4121656120f, 0.2118591070f, 0.0883097947f,
         0.5362752080f, 0.6807189584f, 0.2818474174f,
         0.0514575653f, 0.1074065790f, 0.6302613616f);
    float3x3 kLMStoCONE = float3x3(
         4.0767245293f, -1.2681437731f, -0.0041119885f,
        -3.3072168827f, 2.6093323231f, -0.7034763098f,
         0.2307590544f, -0.3411344290f, 1.7068625689f);
                    

    float3 lmsA = pow(abs(mul(kCONEtoLMS, colA.rgb)), float3(0.3333f, 0.3333f, 0.3333f));
    float3 lmsB = pow(abs(mul(kCONEtoLMS, colB.rgb)), float3(0.3333f, 0.3333f, 0.3333f));

    float3 lms = lerp(lmsA, lmsB, h);
    float alpha = lerp(colA.a, colB.a, h);
    
    return float4(mul(kLMStoCONE, (lms * lms * lms)), alpha);
}

float sdf_quad(float2 sample_pos, float2 rect_half_size, float4 r) {
    
    r.xy = (sample_pos.x > 0.0f) ? r.xy : r.zw;
    r.x = (sample_pos.y > 0.0f) ? r.x : r.y;
    float2 q = abs(sample_pos) - rect_half_size + r.x;
    return min(max(q.x, q.y), 0.0f) + length(max(q, 0.0f)) - r.x;
    
    //return length(max(abs(sample_pos) - rect_half_size + r, 0.0f)) - r;
}

float4 ps_main(vs_out input) : SV_TARGET {
    
    // blend color
    float4 top_color = lerp(input.col00, input.col10, input.c_coord.x);
    float4 bottom_color = lerp(input.col01, input.col11, input.c_coord.x);
    float4 color = lerp(top_color, bottom_color, input.c_coord.y);
    
    // sample texture
    float4 tex_color = color_texture.Sample(texture_sampler, input.uv);
    
    // sdf sample
    float2 sample_pos = (2.0f * input.c_coord - 1.0f) * input.half_size;
    
    float quad_s = sdf_quad(sample_pos, input.half_size - (input.softness * 2.0f), input.radius);
    float quad_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, quad_s);
    
    float border_s = sdf_quad(sample_pos, input.half_size - input.softness * 2.0f - input.thickness, max(input.radius - input.thickness, 0.0f));
    float border_t = smoothstep(0.0f, 2.0 * input.softness, border_s);
    
    if (input.thickness == 0.0f) {
        border_t = 1.0f;
    }
    
    float4 final_color = tex_color;
    final_color *= color;
    final_color.a *= quad_t;
    final_color.a *= border_t;
    
    return final_color;
}
