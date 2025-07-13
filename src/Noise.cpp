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

float Noise::getBasicPerlinTerrain(float x, float z, bool useSeed) {
    if (useSeed) {
        x += seedValue;
        z += seedValue;
    }

    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}

float Noise::getFakePerlinTerrain(float x, float z) {
    return sinf(x) * cosf(z);
}

float Noise::getOctaveTerrain(float x, float z, bool useSeed) {
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

glm::vec3 Noise::getBasicPerlinRock(const glm::vec3& v, bool useSeed) {
    glm::vec3 input = v * frequency;

    float n = sin(input.x) + cos(input.y) + sin(input.z);

    float displacement = n * amplitude;
    glm::vec3 direction = glm::normalize(v);
    glm::vec3 displaced = direction * (1.0f + displacement);

    return displaced;
}
