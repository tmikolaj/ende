#ifndef BASICPERLIN_HPP
#define BASICPERLIN_HPP

#include <cmath>

class BasicPerlin {
public:
    float frequency;
    float amplitude;

    BasicPerlin() = default;
    ~BasicPerlin() = default;
    float get(float x, float z);
};

#endif //BASICPERLIN_HPP
