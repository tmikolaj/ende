#pragma once

#include <stdexcept>

namespace Engine {

template<typename type>
class bEngineComponent {
public:
    bEngineComponent() = default;
    virtual ~bEngineComponent() = default;

    virtual void init() = 0;
    virtual void set(type& prev) = 0;
    virtual void clean() = 0;
};

}