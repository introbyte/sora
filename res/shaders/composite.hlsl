// composite.hlsl

struct vs_in {
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 uv : UV;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    float2 vertex_pos[] = {
        float2(-1.0f, -1.0f),
        float2(-1.0f, 1.0f),
        float2(1.0f, -1.0f),
        float2(1.0f, 1.0f),
    };
    
    float2 vertex_uvs[] = {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f),
    };
    
    // fill output
    output.pos = float4(vertex_pos[input.vertex_id], 0.0f, 1.0f);
    output.uv = vertex_uvs[input.vertex_id];
    
    return output;
}

Texture2D screen_texture : register(t0);
SamplerState nearest_sampler : register(s0);
SamplerState linear_sampler : register(s1);

float3 aces(float3 v) {
    v *= 0.6f;
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v * (a * v + b)) / (v * (c * v + d) + e), 0.0f, 1.0f);
}

float4 ps_main(vs_out input) : SV_TARGET {
    float3 scene_color = screen_texture.Sample(nearest_sampler, input.uv).rgb;
    return float4(scene_color, 1.0f);
}