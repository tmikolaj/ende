#ifndef BASICPERLIN_HPP
#define BASICPERLIN_HPP

#include <cmath>
#include <random>

class Noise {
private:
    char seedBuffer[25];
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> dist;
public:
    float frequency;
    float amplitude;

    int octaves;
    float persistence;
    float lacunarity;

    int seedValue;

    Noise();
    ~Noise() = default;

    void init(char _seedBuffer[25]);
    int getSeedValue();
    void updateSeedValue(int newVal);
    int genNewSeedValue();
    float getBasicPerlin(float x, float z);
    float getFakePerlin(float x, float z);
    float getOctave(float x, float z);
};

#endif //BASICPERLIN_HPP
