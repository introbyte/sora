// shader_2d.hlsl

cbuffer globals : register(b0) {
    float2 window_size;
}

struct vs_in {
    float2 position : POSITION;
    float2 texcoord : TEXCOORD;
};

struct vs_out {
    float4 position : SV_POSITION;
    float2 texcoord : UV;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
    // fill output
    output.position = float4(input.position * 2.0f / window_size - 1.0f, 0.0f, 1.0f);
    output.position.y = -output.position.y;
    output.texcoord = input.texcoord;
    
    return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
    return float4(1.0f, 0.0f, 1.0f, 1.0f);
}