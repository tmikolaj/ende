#ifndef SHAPER_HPP
#define SHAPER_HPP

#include "../entities/Entity.hpp"

class Shaper {
protected:
    Entity* entity;
public:
    explicit Shaper(Entity* _entity) : entity(_entity) { }
    virtual ~Shaper() = default;

    bool s_added = false;
    virtual void Apply(std::unique_ptr<Entity>& e) = 0;
};

#endif //SHAPER_HPP
