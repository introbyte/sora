// shader_3d.hlsl


struct vs_in {
    float4 bbox : BBOX;
    float4 texcoords : TEX;
    float2 point0 : PNT0;
    float2 point1 : PNT1;
    float2 point2 : PNT2;
    float2 point3 : PNT3;
    float4 color0 : COL0;
    float4 color1 : COL1;
    float4 color2 : COL2;
    float4 color3 : COL3;
    float4 radii : RAD;
    float2 style : STY;
    uint indices : TEST;
    uint vertex_id : SV_VertexID;
};

struct vs_out {
    float4 pos : SV_POSITION;
    float2 half_size : HSIZE;
    float2 texcoord : TEX;
    float2 col_coord : COLC;
    float2 point0 : PNT0;
    float2 point1 : PNT1;
    float2 point2 : PNT2;
    float2 point3 : PNT3;
    float4 color0 : COL0;
    float4 color1 : COL1;
    float4 color2 : COL2;
    float4 color3 : COL3;
    float4 radii : RAD;
    float2 style : STY;
    uint shape : SHP;
    uint texture_index : TID;
    uint clip_index : CLP;
};

vs_out vs_main(vs_in input) {
    vs_out output = (vs_out) 0;
    
       
    return output;
}

float4 ps_main(vs_out input) : SV_TARGET {
    return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
