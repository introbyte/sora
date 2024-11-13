// shader_3d.hlsl

cbuffer globals : register(b0) {
    float4x4 view_projection;
    float4x4 projection;
    float4x4 view;
}

struct vs_in {
    float3 position : POSITION;
};

struct vs_out {
    float4 position : SV_POSITION;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    output.position = float4(input.position, 1.0f);
    output.position = mul(view_projection, output.position);
       
    return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
