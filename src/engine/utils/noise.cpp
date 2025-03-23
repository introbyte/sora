// noise.cpp

#ifndef NOISE_CPP
#define NOISE_CPP

//- implementation 

function f32
noise_white(f32 p) {
    random_seed((u32)p);
    f32 value = random_f32();
    return value;
}

function f32
noise_white(vec2_t p) {
    u32 seed = (u32)p.y << 16 | (u32)p.x;
    random_seed(seed);
    f32 value = random_f32();
    return value;
}

function f32
noise_white(vec3_t p) {
    u32 seed = ((u32)p.z << 20) ^ ((u32)p.y << 10) ^ (u32)p.x;
	random_seed(seed);
	f32 value = random_f32();
	return value;
}


function f32
noise_simplex(f32 x) {
    
    i32 xi0 = floorf(x);
    
    f32 xf0 = x - xi0;
    f32 xf1 = xf0 - 1.0f;
    
    i32 xi = xi0 & 0xFF;
    
    f32 u = _noise_fade(xf0);
    
    i32 h0 = noise_perm[xi + 0];
    i32 h1 = noise_perm[xi + 1];
    
    return lerp(_noise_grad(h0, xf0), _noise_grad(h1, xf1), u);
}

function f32
noise_simplex(f32 x, f32 y) {
    
    i32 xi0 = (i32)x;
    i32 yi0 = (i32)y;
    
    f32 xf0 = x - xi0;
    f32 yf0 = y - yi0;
    f32 xf1 = xf0 - 1.0f;
    f32 yf1 = yf0 - 1.0f;
    
    i32 xi = xi0 & 0xFF;
    i32 yi = yi0 & 0xFF;
    
    f32 u = _noise_fade(xf0);
    f32 v = _noise_fade(yf0);
    
    i32 h00 = (i32)noise_perm[noise_perm[xi + 0] + yi + 0];
    i32 h01 = (i32)noise_perm[noise_perm[xi + 0] + yi + 1];
    i32 h10 = (i32)noise_perm[noise_perm[xi + 1] + yi + 0];
    i32 h11 = (i32)noise_perm[noise_perm[xi + 1] + yi + 1];
    
    f32 x1 = lerp(_noise_grad(h00, xf0, yf0), _noise_grad(h10, xf1, yf0), u);
    f32 x2 = lerp(_noise_grad(h01, xf0, yf1), _noise_grad(h11, xf1, yf1), u);
    
    return lerp(x1, x2, v);
}

function f32
noise_simplex(f32 x, f32 y, f32 z) {
    
    i32 xi0 = (i32)floorf(x);
    i32 yi0 = (i32)floorf(y);
    i32 zi0 = (i32)floorf(z);
    
    f32 xf0 = x - f32(xi0);
    f32 yf0 = y - f32(yi0);
    f32 zf0 = z - f32(zi0);
    f32 xf1 = xf0 - f32(1.0);
    f32 yf1 = yf0 - f32(1.0);
    f32 zf1 = zf0 - f32(1.0);
    
    i32 xi = xi0 & 0xFF;
    i32 yi = yi0 & 0xFF;
    i32 zi = zi0 & 0xFF;
    
    f32 u = _noise_fade(xf0);
    f32 v = _noise_fade(yf0);
    f32 w = _noise_fade(zf0);
    
    i32 h000 = noise_perm[noise_perm[noise_perm[xi + 0] + yi + 0] + zi + 0];
    i32 h001 = noise_perm[noise_perm[noise_perm[xi + 0] + yi + 0] + zi + 1];
    i32 h010 = noise_perm[noise_perm[noise_perm[xi + 0] + yi + 1] + zi + 0];
    i32 h011 = noise_perm[noise_perm[noise_perm[xi + 0] + yi + 1] + zi + 1];
    i32 h100 = noise_perm[noise_perm[noise_perm[xi + 1] + yi + 0] + zi + 0];
    i32 h101 = noise_perm[noise_perm[noise_perm[xi + 1] + yi + 0] + zi + 1];
    i32 h110 = noise_perm[noise_perm[noise_perm[xi + 1] + yi + 1] + zi + 0];
    i32 h111 = noise_perm[noise_perm[noise_perm[xi + 1] + yi + 1] + zi + 1];
    
    f32 x11 = lerp(_noise_grad(h000, xf0, yf0, zf0), _noise_grad(h100, xf1, yf0, zf0), u);
    f32 x12 = lerp(_noise_grad(h010, xf0, yf1, zf0), _noise_grad(h110, xf1, yf1, zf0), u);
    f32 x21 = lerp(_noise_grad(h001, xf0, yf0, zf1), _noise_grad(h101, xf1, yf0, zf1), u);
    f32 x22 = lerp(_noise_grad(h011, xf0, yf1, zf1), _noise_grad(h111, xf1, yf1, zf1), u);
    
    f32 y1 = lerp(x11, x12, v);
    f32 y2 = lerp(x21, x22, v);
    
    return lerp(y1, y2, w);
}


function f32
noise_fbm(f32 p, u8 octaves = 4, f32 lacunarity = 2.0f, f32 gain = 0.5f) {
    f32 sum = 0.0f;
    f32 freq = 1.0f;
    f32 amp = 0.5f;
    
    for (u8 i = 0; i < octaves; i++) {
        f32 n = noise_simplex(p * freq);
        sum += n * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    
    return sum;
}

function f32
noise_fbm(vec2_t p, u8 octaves = 4, f32 lacunarity = 2.0f, f32 gain = 0.5f) {
    f32 sum = 0.0f;
    f32 freq = 1.0f;
    f32 amp = 0.5f;
    
    for (u8 i = 0; i < octaves; i++) {
        f32 n = noise_simplex(p.x * freq, p.y * freq);
        sum += n * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    
    return sum;
}

function f32
noise_fbm(vec3_t p, u8 octaves = 4, f32 lacunarity = 2.0f, f32 gain = 0.5f) {
    
    f32 sum = 0.0f;
    f32 freq = 1.0f;
    f32 amp = 0.5f;
    
    for (u8 i = 0; i < octaves; i++) {
        f32 n = noise_simplex(p.x * freq, p.y * freq, p.z * freq);
        sum += n * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    
    return sum;
}

function vec2_t
noise_fbm_range(u8 dimensions, u8 octaves = 4, f32 lacunarity = 2.0f, f32 gain = 0.5f) {
    
    const f32 dim_ranges[] = { 0.5f, 0.70710678118f, 0.86602540378f };
    
    f32 lower = 0.0f;
    f32 upper = 0.0f;
    
    if (gain == 1.0f) {
        lower = -dim_ranges[dimensions - 1] * 0.5f * octaves;
        upper = dim_ranges[dimensions - 1] * 0.5f * octaves;
    } else  {
        lower = -dim_ranges[dimensions - 1] * 0.5f * ((1.0f - powf(gain, (f32)octaves)) / (1.0f - gain));
        upper = dim_ranges[dimensions - 1] * 0.5f * ((1.0f - powf(gain, (f32)octaves)) / (1.0f - gain));
    }
    
    return vec2(lower, upper);
}


// helper functions

inlnfunc f32
_noise_fade(f32 t) { 
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inlnfunc f32
_noise_grad(i32 hash, f32 x) {
    return (hash & 0x1) ? x : -x;
}

inlnfunc f32
_noise_grad(i32 hash, f32 x, f32 y) {
    switch (hash & 0x7) {
        case 0x0: return  x + y;
        case 0x1: return  x;
        case 0x2: return  x - y;
        case 0x3: return -y;
        case 0x4: return -x - y;
        case 0x5: return -x;
        case 0x6: return -x + y;
        case 0x7: return  y;
        default:  return  0.0f;
    }
}

inlnfunc f32
_noise_grad(i32 hash, f32 x, f32 y, f32 z) {
    switch (hash & 0xF) {
        case 0x0: return  x + y;
        case 0x1: return -x + y;
        case 0x2: return  x - y;
        case 0x3: return -x - y;
        case 0x4: return  x + z;
        case 0x5: return -x + z;
        case 0x6: return  x - z;
        case 0x7: return -x - z;
        case 0x8: return  y + z;
        case 0x9: return -y + z;
        case 0xA: return  y - z;
        case 0xB: return -y - z;
        case 0xC: return  y + x;
        case 0xD: return -y + z;
        case 0xE: return  y - x;
        case 0xF: return -y - z;
        default:  return  0.0f;
    }
}


#endif // NOISE_CPP

