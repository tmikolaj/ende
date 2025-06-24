#pragma once

namespace Engine {

class StateTemplate {
public:
    virtual void init() = 0;
    virtual void process() = 0;
    virtual void draw() = 0;
    virtual void clean() = 0;
};

}