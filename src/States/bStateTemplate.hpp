#pragma once

#include <memory>
#include "raylib.h"

struct Context;

class bStateTemplate {
public:
    bStateTemplate() = default;
    virtual ~bStateTemplate() = default;
    virtual void init(std::shared_ptr<Context>& m_context) = 0;
    virtual void process(std::shared_ptr<Context>& m_context) = 0;
    virtual void draw(std::shared_ptr<Context>& m_context) = 0;
    virtual void clean(std::shared_ptr<Context>& m_context) = 0;
};