// ui_line.hlsl

cbuffer constants : register(b0) {
    float2 window_size;
}

struct vs_in {
    float4 pos : POS;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 points : PNT;
    float4 style : STY; // (thickness, softness, unused, unused)
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 uv : UV;
    float4 col0 : COL0;
    float4 col1 : COL1;
    float4 points : PNT;
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
    output.points = input.points;
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
        -3.3072168827f,  2.6093323231f, -0.7034763098f,
         0.2307590544f, -0.3411344290f,  1.7068625689f);
                    

    float3 lmsA = pow(abs(mul(kCONEtoLMS, colA.rgb)), float3(0.3333f, 0.3333f, 0.3333f));
    float3 lmsB = pow(abs(mul(kCONEtoLMS, colB.rgb)), float3(0.3333f, 0.3333f, 0.3333f));

    float3 lms = lerp(lmsA, lmsB, h);
    float alpha = lerp(colA.a, colB.a, h);
    
    return float4(mul(kLMStoCONE, (lms * lms * lms)), alpha);
}

float sdf_line(float2 p, float2 a, float2 b) {
    float2 ba = b - a;
    float2 pa = p - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0f, 1.0f);
    return length(pa - h * ba);
}

float4 ps_main(vs_out input) : SV_TARGET {
    
    // sample texture
    float4 tex_color = color_texture.Sample(texture_sampler, input.uv);
    
    // sdf sample
    float2 sample_pos = (2.0f * input.uv - 1.0f) * input.half_size;
    
    float line_s = sdf_line(sample_pos, input.points.xy, input.points.zw) - (input.thickness * 0.25f);
    float line_t = 1.0f - smoothstep(0.0f, 2.0f * input.softness, line_s);
    
    float t = length(sample_pos - input.points.zw) / length(input.half_size * 2.0f);
    float4 color = lerp(input.col0, input.col1, t);
    
    float4 final_color = tex_color;
    final_color *= color;
    final_color.a *= line_t;
    
    return final_color;
}
