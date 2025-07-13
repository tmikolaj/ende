#ifndef SUBDIVISIONSHAPER_HPP
#define SUBDIVISIONSHAPER_HPP

#include <map>
#include <utility>
#include <algorithm>
#include "Shaper.hpp"

class SubdivisionShaper : public Shaper {
private:
    bool isCircular;
    int GetMidpoint(int i1, int i2, std::vector<float>& vertices, std::map<std::pair<int, int>, int>& cache);
public:
    SubdivisionShaper(Entity* _entity, bool _isCircular);
    ~SubdivisionShaper() override = default;

    void Apply(std::unique_ptr<Entity>& e) override;
};

#endif //SUBDIVISIONSHAPER_HPP
