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

float4 ps_main(vs_out input) : SV_TARGET {
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}