#ifndef BASESTATE_HPP
#define BASESTATE_HPP

#include <memory>

struct Context;

class BaseState {
public:
    BaseState() = default;
    virtual ~BaseState() = default;
    virtual void init(std::shared_ptr<Context>& m_context) = 0;
    virtual void process(std::shared_ptr<Context>& m_context) = 0;
    virtual void draw(std::shared_ptr<Context>& m_context) = 0;
    virtual void clean(std::shared_ptr<Context>& m_context) = 0;
};

#endif //BASESTATE_HPP