#ifndef ENTITYPAINT_HPP
#define ENTITYPAINT_HPP

#include "bStateTemplate.hpp"

class EntityPaint : public bStateTemplate {
private:
    int currentShader;
public:
    EntityPaint() = default;
    ~EntityPaint() override = default;

    void init(std::shared_ptr<Context> &m_context) override;
    void process(std::shared_ptr<Context> &m_context) override;
    void draw(std::shared_ptr<Context> &m_context) override;
    void clean(std::shared_ptr<Context> &m_context) override;
};

#endif //ENTITYPAINT_HPP
