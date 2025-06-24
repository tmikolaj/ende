#pragma once

namespace Engine {

template<typename type>
class EngineTemplate {
public:
    EngineTemplate() = default;
    virtual ~EngineTemplate() = default;

    virtual void init() = 0;
    virtual type set() = 0;
    virtual void clean() = 0;
};

}