#include "BasicPerlin.hpp"

float BasicPerlin::get(float x, float z) {
    return (sinf(x * frequency) * cosf(z * frequency)) * amplitude;
}
