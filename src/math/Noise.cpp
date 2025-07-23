#include <cstring>
#include "Noise.hpp"

Noise::Noise() :
dist(-10000, 10000) {

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

float Noise::getFractalNoise(float x, float z, bool useSeed) {
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
