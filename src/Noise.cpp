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

float Noise::getBasicPerlin(float x, float z, bool useSeed) {
    if (useSeed) {
        x += seedValue;
        z += seedValue;
    }

    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}

float Noise::getFakePerlin(float x, float z) {
    return sinf(x) * cosf(z);
}

float Noise::getOctave(float x, float z, bool useSeed) {
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

        float noiseVal = getFakePerlin(nx, nz);
        total += noiseVal * amp;

        amp *= persistence;
        freq *= lacunarity;
    }

    return total;
}
