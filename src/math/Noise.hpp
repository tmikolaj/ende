#ifndef NOISE_HPP
#define NOISE_HPP

#include <random>
#include <external/glm/glm.hpp>

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

    bool improvedFakeNoise;

    Noise();
    ~Noise() = default;

    void init(char _seedBuffer[25]);
    int getSeedValue();
    void updateSeedValue(int newVal);
    int genNewSeedValue();

    // TERRAIN SPECIFIC NOISE
    float getSimplePatternTerrain(float x, float z, bool useSeed);
    float getFakePerlinTerrain(float x, float z);
    float getFractalNoise(float x, float z, bool useSeed);

    // ROCK SPECIFIC NOISE
    glm::vec3 getSimplePatternRock(const glm::vec3& v, bool useSeed);
};

#endif // NOISE_HPP
