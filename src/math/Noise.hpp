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

    float fade(float t);
    float hash(int x, int z, bool useSeed);
    float lerp(float a, float b, float t);
public:
    float frequency;
    float amplitude;

    int octaves;
    float persistence;
    float lacunarity;
    float terraceSteps;

    int seedValue;

    bool improvedFakeNoise;
    bool enableTerracing;

    Noise();
    ~Noise() = default;

    void init(char _seedBuffer[25]);
    int getSeedValue();
    void updateSeedValue(int newVal);
    int genNewSeedValue();

    // TERRAIN SPECIFIC NOISE
    float getSimplePatternTerrain(float x, float z, bool useSeed);
    float getFakePerlinTerrain(float x, float z); // HELPER for getFractalNoiseTerrain!
    float getFractalNoiseTerrain(float x, float z, bool useSeed);
    float getValueNoiseTerrain(float x, float z, bool useSeed, bool makeFractal);
    float getPerlinNoiseTerrain(float x, float z, bool useSeed, bool makeFractal);
    float getSinglePerlinNoiseTerrain(float x, float z, float freq, bool useSeed); // HELPER for getPerlinNoiseTerrain

    // ROCK SPECIFIC NOISE
    glm::vec3 getSimplePatternRock(const glm::vec3& v, bool useSeed);
};

#endif // NOISE_HPP
