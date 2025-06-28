#ifndef STARTMENU_HPP
#define STARTMENU_HPP

#include "bStateTemplate.hpp"

class StartMenu : public Engine::bStateTemplate {
public:
    StartMenu() = default;
    ~StartMenu() override = default;

    void init() override;
    void process() override;
    void draw(const Shader& sh) override;
    void clean() override;
};

#endif // STARTMENU_HPP
