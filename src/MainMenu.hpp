#ifndef MAINMENU_HPP
#define MAINMENU_HPP

#include "bStateTemplate.hpp"

class MainMenu : public Engine::bStateTemplate {
public:
    MainMenu() = default;
    ~MainMenu() override = default;

    void init() override;
    void process() override;
    void draw(const Shader& sh) override;
    void clean() override;
};

#endif //MAINMENU_HPP
