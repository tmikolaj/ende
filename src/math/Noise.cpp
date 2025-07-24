#include <cstring>
#include "Noise.hpp"

Noise::Noise() :
dist(-10000, 10000),
improvedFakeNoise(false) {

}

void Noise::init(char _seedBuffer[25]) {
    if (_seedBuffer[0] != '\0') {
        std::strncpy(seedBuffer, _seedBuffer, sizeof(seedBuffer));
        gen.seed(std::stoi(seedBuffer));
    } else {
        gen.seed(rd());
        seedValue = 0;
    }
}

// -------------------
//       SEED
// -------------------

int Noise::getSeedValue() {
    return seedValue;
}

void Noise::updateSeedValue(int newVal) {
    seedValue = newVal;
    gen.seed(seedValue);
}

int Noise::genNewSeedValue() {
    gen.seed(seedValue);
    seedValue = dist(gen);
    return seedValue;
}

// -------------
//   TERRAIN
// -------------

float Noise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Noise::hash(int x, int z, bool useSeed) {
    const int n = x * 374761 + z * 668265;

    float result = sinf(n * 0.0001f) * 43758.5453f;

    return (result - floorf(result)) * 2.0f - 1.0f;
}

float Noise::lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Noise::getSimplePatternTerrain(float x, float z, bool useSeed) {
    if (useSeed) {
        x += seedValue;
        z += seedValue;
    }

    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}

float Noise::getFakePerlinTerrain(float x, float z) {
    if (!improvedFakeNoise) {
        return sinf(x) * cosf(z);
    } else {
        float ret = sinf(x * 0.8f + cosf(z * 0.6f)) + cosf(z * 0.7f + sinf(x * 0.4f));
        return ret * 0.5f;
    }
}

float Noise::getFractalNoiseTerrain(float x, float z, bool useSeed) {
    float total = 0.0f;
    float freq = frequency;
    float amp = amplitude;

    for (int i = 0; i < octaves; i++) {
        float nx = x * freq;
        float nz = z * freq;

        if (useSeed) {
            nx += seedValue;
            nz += seedValue;
        }

        float noiseVal = getFakePerlinTerrain(nx, nz);
        total += noiseVal * amp;

        amp *= persistence;
        freq *= lacunarity;
    }

    return total;
}

float Noise::getValueNoiseTerrain(float x, float z, bool useSeed, bool makeFractal) {

    int iterCount = makeFractal ? octaves : 1;
    float total = 0.0f;
    float freq = frequency;
    float amp = amplitude;

    for (int i = 0; i < iterCount; i++) {
        float nx = x * freq;
        float nz = z * freq;

        if (useSeed) {
            nx += static_cast<float>(seedValue);
            nz += static_cast<float>(seedValue);
        }

        const int x0 = static_cast<int>(floorf(nx));
        const int x1 = x0 + 1;
        const int z0 = static_cast<int>(floorf(nz));
        const int z1 = z0 + 1;

        const float sx = fade(nx - static_cast<float>(x0));
        const float sz = fade(nz - static_cast<float>(z0));

        const float a = hash(x0, z0, useSeed);
        const float b = hash(x1, z0, useSeed);
        const float c = hash(x0, z1, useSeed);
        const float d = hash(x1, z1, useSeed);

        const float lerp1 = lerp(a, b, sx);
        const float lerp2 = lerp(c, d, sx);

        const float value = lerp(lerp1, lerp2, sz);

        total += value * amp;

        if (makeFractal) {
            freq *= lacunarity;
            amp *= persistence;
        }
    }

    return total;
}

// -------------
//     ROCK
// -------------

glm::vec3 Noise::getSimplePatternRock(const glm::vec3& v, bool useSeed) {
    glm::vec3 input = glm::normalize(v) * frequency;

    float noiseSeed = 0.0f;
    float multiplier = 1.0f;

    if (useSeed) {
        noiseSeed = seedValue * 0.0001f;
        multiplier = 0.8f + fmodf(static_cast<float>(seedValue), 1000.0f) * 0.0002f;
        input += glm::vec3(noiseSeed);
    }

    float noise = sinf(input.x * (multiplier + 0.2f)) + sinf(input.y * (multiplier - 0.3f)) +
        sinf(input.y * (multiplier + 0.3f)) + cosf(input.z * (multiplier - 0.4f)) +
        sinf(input.z * (multiplier + 0.2f)) + sinf(input.x * (multiplier - 0.2f));

    float displacement = noise * amplitude;

    glm::vec3 displaced = glm::normalize(v) * (1.0f + displacement);
    return displaced;
}
