#ifndef SUBDIVISIONSHAPER_HPP
#define SUBDIVISIONSHAPER_HPP

#include "Shaper.hpp"

class SubdivisionShaper : public Shaper {
public:
    explicit SubdivisionShaper(Entity* _entity);
    ~SubdivisionShaper() override = default;

    void Apply() override;
};

#endif //SUBDIVISIONSHAPER_HPP
