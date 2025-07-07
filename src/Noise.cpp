#include "Noise.hpp"

float Noise::getBasicPerlin(float x, float z) {
    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}

float Noise::getFakePerlin(float x, float z) {
    return sinf(x) * cosf(z);
}

float Noise::getOctave(float x, float z) {
    float total = 0.0f;
    float freq = frequency;
    float amp = amplitude;

    for (int i = 0; i < octaves; i++) {
        float nx = x * freq;
        float ny = z * freq;

        float noiseVal = getFakePerlin(nx, ny);
        total += noiseVal * amp;

        amp *= persistence;
        freq *= lacunarity;
    }

    return total;
}
