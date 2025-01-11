// atmosphere.hlsl

RWTexture2D<float4> output_texture;

cbuffer globals {
    float4x4 camera_view_proj;
    float4x4 camera_inv_view;
    float4x4 camera_inv_proj;
    float2 window_size;
};

// defines

#define eps 1e-6
#define pi 3.14159265359f
#define inf 1.#INF

// functions

float2 sphere_intersection(float3 ray_start, float3 ray_dir, float3 sphere_center, float sphere_radius) {
    ray_start -= sphere_center;
    
    float a = dot(ray_dir, ray_dir);
    float b = 2.0f * dot(ray_start, ray_dir);
    float c = dot(ray_start, ray_start) - (sphere_radius * sphere_radius);
    float d = b * b - 4.0f * a * c;
    
    if (d < 0.0f) {
        return -1;
    } else {
        d = sqrt(d);
        return float2(-b - d, -b + d) / (2.0f * a);
    }
}

[numthreads(16, 16, 1)]
void cs_main(uint3 id : SV_DispatchThreadID) {
    
    // calculate view vector
    float2 ndc_pos = ((id.xy / window_size) - 0.5f) * 2.0f;
    float3 view_vector = mul(float4(ndc_pos, 0.0f, 1.0f), camera_inv_proj).xyz;
    view_vector = mul(float4(view_vector.xyz, 0.0f), camera_inv_view).xyz;
    
    // setup ray
    float3 ray_start = camera_inv_view[3].xyz;
    float3 ray_dir = normalize(view_vector);
    float ray_lenght = inf;
    
    float2 intersection = sphere_intersection(ray_start, ray_dir, float3(2.0f, 0.0f, 0.0f), 1.0f);
    
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    if (intersection.x > 0.0f) {
        color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    
    output_texture[id.xy] = float4(view_vector, 1.0f);
}