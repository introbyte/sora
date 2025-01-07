// atmosphere.hlsl

RWTexture3D<float4> output_texture : register(u0);

float3 hash33(float3 p) {
    uint3 q = uint3(int3(p)) * uint3(1597334673U, 3812015801U, 2798796415U);
    q = (q.x ^ q.y ^ q.z) * uint3(1597334673U, 3812015801U, 2798796415U);
    return -1.0f + 2.0f * float3(q) * (1.0f / float(0xffffffffU));
}

float worley_noise(float3 uv, float freq) {
    
    float3 id = floor(uv);
    float3 p = frac(uv);
    
    float min_dist = 10000.0f;
    for (float x = -1.0f; x <= 1.0f; ++x) {
        for (float y = -1.0f; y <= 1.0f; ++y) {
            for (float z = -1.0f; z <= 1.0f; ++z) {
                float3 offset = float3(x, y, z);
                float3 h = hash33(fmod(id + offset, float3(freq, freq, freq))) * 0.5f + 0.5f;
                h += offset;
                float3 d = p - h;
                min_dist = min(min_dist, dot(d, d));
            }
        }
    }
    
    return 1.0f - min_dist;
}

float worley_fbm(float3 p, float freq, int octaves) {
    
    float g = exp2(-0.85f);
    float amp = 1.0f;
    float noise = 0.0f;
    
    for (int i = 0; i < octaves; ++i) {
        noise += amp * worley_noise(p * freq, freq);
        freq *= 2.0f;
        amp *= g;
    }
    
    return noise / float(octaves - 1.0f);
}


[numthreads(8, 8, 8)]
void cs_main(uint3 id : SV_DispatchThreadID) {
    float value = worley_fbm(float3(id.x / 256.0f, id.y / 256.0f, id.z / 256.0f), 4.0f, 5);
    output_texture[id.xyz] = float4(value, value, value, 1.0f);
}