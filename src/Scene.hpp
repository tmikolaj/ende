#ifndef SCENE_HPP
#define SCENE_HPP

#include "Context.hpp"

class Scene : public Engine::bStateTemplate {
private:
    std::shared_ptr<Context> m_context;
public:
    explicit Scene(std::shared_ptr<Context>& context);
    ~Scene() override = default;

    void init() override;
    void process() override;
    void draw(const Shader& sh) override;
    void clean() override;
};

#endif //SCENE_HPP
