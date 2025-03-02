// atmosphere.hlsl

RWTexture2D<float4> output_texture;
RWTexture2D<float4> blue_noise_texture;

cbuffer globals {
    float4x4 camera_view_proj;
    float4x4 camera_inv_view;
    float4x4 camera_inv_proj;
    float2 window_size;
	
	float4 atmosphere_tint;
	float4 rayleigh_coeff;
	float4 mie_coeff;
	float4 ozone_coeff;
	float density;
	float exposure;
    float atmosphere_height;
};

// defines

#define eps 1e-6
#define pi 3.14159265359f
#define inf 1.#INF

#define star_position (float3(0.0f, 0.0f, 100.0f))
#define planet_radius (5.0f)
#define planet_center (float3(0.0f, 0.0f, 0.0f))
#define rayleigh_height (atmosphere_height * 0.1f)
#define mie_height (atmosphere_height * 0.05f)
#define c_ozone    (float3(0.650f,  0.881f,  0.085f) * 1e-1)

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

// phase functions

float phase_rayleigh(float cos_theta) {
    return 3.0f * (1.0f + cos_theta * cos_theta) / (16.0f * pi);
}

float phase_mie(float cos_theta, float g = 0.85) {
    g = min(g, 0.9381f);
    float k = 1.55f * g - 0.55f * g * g * g;
    float k_cos_theta = k * cos_theta;
    return (1.0f - k * k) / ((4.0f * pi) * (1.0f - k_cos_theta) * (1.0f - k_cos_theta));
}

// atmosphere functions

float sample_height(float3 p) {
    return distance(p, planet_center) - planet_radius;
}

float density_rayleigh(float h) {
    return exp(-max(0, h / rayleigh_height));
}

float density_mie(float h) {
    return exp(-max(0, h / mie_height));
}

float density_ozone(float h) {
    return max(0, 1 - abs(h - 25000.0f) / 15000.0f);
}

float3 sample_density(float h) {
    return float3(density_rayleigh(h), density_mie(h), density_ozone(h));
}

float3 absorb(float3 optical_depth) {
    return exp(-(optical_depth.x * rayleigh_coeff.rgb + optical_depth.y * mie_coeff.rgb * 1.1 + optical_depth.z * c_ozone) * density);
}

float3 integrate_optical_depth(float3 ray_start, float3 ray_dir) {
    
    float2 intersection = sphere_intersection(ray_start, ray_dir, planet_center, planet_radius + atmosphere_height);
    float ray_length = intersection.y;

    int sample_count = 8;
    float step_size = ray_length / sample_count;
	
    float3 optical_depth = 0;

    for (int i = 0; i < sample_count; i++) {
        float3 local_position = ray_start + ray_dir * (i + 0.5f) * step_size;
        float local_height = sample_height(local_position);
        float3 local_density = sample_density(local_height);

        optical_depth += local_density * step_size;
    }

    return optical_depth;
}

float3 integrate_scattering(float3 ray_start, float3 ray_dir, float ray_length, out float3 transmittance) {
    
    // check for atmosphere
    float2 intersection = sphere_intersection(ray_start, ray_dir, planet_center, planet_radius + atmosphere_height);
    
    // return early if we aren't in atmosphere
    if (intersection.y < 0.0f) {
        transmittance = 1.0f;
        return 0.0f;
    }
    
    ray_length = min(ray_length, intersection.y);
    if (intersection.x > 0.0f) {
        ray_start += ray_dir * intersection.x;
        ray_length -= intersection.x;
    }

    int sample_count = 8;
    float prev_ray_time = 0;

    float3 optical_depth = float3(0.0f, 0.0f, 0.0f);
    float3 rayleigh = float3(0.0f, 0.0f, 0.0f);
    float3 mie = float3(0.0f, 0.0f, 0.0f);
    
    float sample_distribution_exponent = 1.0f + saturate(1.0f - sample_height(ray_start) / atmosphere_height) * 8.0f;

    for (int i = 0; i < sample_count; i++) {   
        
        float ray_time = pow((float) i / sample_count, sample_distribution_exponent) * ray_length;
        float step_size = (ray_time - prev_ray_time);

        float3 local_position = ray_start + ray_dir * ray_time;
        float local_height = sample_height(local_position);
        float3 local_density = sample_density(local_height);
        
        float3 light_dir = normalize(star_position - local_position);
    
        // calculate phase functions
        float cos_theta = dot(ray_dir, light_dir);
        float phase_r = phase_rayleigh(cos_theta);
        float phase_m = phase_mie(cos_theta);
        
        optical_depth += local_density * step_size;
        
        float3 view_transmittance = absorb(optical_depth);
        float3 optical_depth_light = integrate_optical_depth(local_position, light_dir);
        float3 light_transmittance = absorb(optical_depth_light);
        
        rayleigh += view_transmittance * light_transmittance * phase_r * local_density.x * step_size;
        mie += view_transmittance * light_transmittance * phase_m * local_density.y * step_size;
        
        prev_ray_time = ray_time;
    }

    transmittance = absorb(optical_depth);
    
    return ((rayleigh * rayleigh_coeff.rgb + mie * mie_coeff.rgb) * exposure);
}

[numthreads(32 , 32, 1)]
void cs_main(uint3 id : SV_DispatchThreadID) {
    
    // calculate view vector
    float2 ndc_pos = ((id.xy / window_size) - 0.5f) * 2.0f;
    float3 view_vector = mul(float4(ndc_pos, 0.0f, 1.0f), camera_inv_proj).xyz;
    view_vector = mul(float4(view_vector.xyz, 0.0f), camera_inv_view).xyz;
    
    // setup ray
    float3 ray_start = camera_inv_view[3].xyz;
    float3 ray_dir = normalize(view_vector);
    float ray_length= inf;
    
	// sample blue noise
	float blue_noise = blue_noise_texture[id.xy % 16].x * 0.02f;

	float3 transmittance = float3(0.0f, 0.0f, 0.0f);
	float3 atmosphere_color = (integrate_scattering(ray_start, ray_dir, ray_length, transmittance) * atmosphere_tint.rgb) + blue_noise;
	
	output_texture[id.xy] =  float4(atmosphere_color, 1.0f);
}