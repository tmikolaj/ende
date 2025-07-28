#ifndef THERMALEROSION_HPP
#define THERMALEROSION_HPP

#include <vector>

class ThermalErosion {
public:
    float talus;
    float strength;

    float neighbors4[4];
    float neighbors8[8];

    int iterations;

    bool use4;

    ThermalErosion();
    ~ThermalErosion() = default;

    int computeGridWidth(const std::vector<float>& vertices);
    int computeGridHeight(const std::vector<float>& vertices);
    void Apply(std::vector<float>& vertices);
};

#endif //THERMALEROSION_HPP
