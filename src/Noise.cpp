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
    }
    seedValue = dist(gen);
}

int Noise::getSeedValue() {
    return seedValue;
}

void Noise::updateSeedValue(int newVal) {
    seedValue = newVal;
}

int Noise::genNewSeedValue() {
    seedValue = dist(gen);
    return seedValue;
}

float Noise::getBasicPerlin(float x, float z) {
    float offsetx =  x + seedValue;
    float offsetz =  z + seedValue;

    return (sinf(offsetx * frequency) * cosf(offsetz * frequency)) * amplitude;
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

        float noiseVal = getFakePerlin(nx + seedValue, ny + seedValue);
        total += noiseVal * amp;

        amp *= persistence;
        freq *= lacunarity;
    }

    return total;
}
