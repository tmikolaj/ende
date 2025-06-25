#pragma once

#include "raylib.h"

namespace Engine {

class bStateTemplate {
public:
    bStateTemplate() = default;
    virtual ~bStateTemplate() = default;
    virtual void init() = 0;
    virtual void process() = 0;
    virtual void draw(const Shader& sh) = 0;
    virtual void clean() = 0;
};

}