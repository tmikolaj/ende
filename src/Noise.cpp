#include "Noise.hpp"

float Noise::getBasicPerlin(float x, float z) {
    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}

float Noise::getOctave(float x, float z) {

}
