#ifndef BASICPERLIN_HPP
#define BASICPERLIN_HPP

#include <cmath>

class Noise {
public:
    float frequency;
    float amplitude;

    int octaves;
    float persistence;
    float lacunarity;

    Noise() = default;
    ~Noise() = default;

    float getBasicPerlin(float x, float z);
    float getOctave(float x, float z);
};

#endif //BASICPERLIN_HPP
