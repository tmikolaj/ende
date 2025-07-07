#ifndef STARTMENU_HPP
#define STARTMENU_HPP

#include <string>
#include "bStateTemplate.hpp"
#include "external/rlImGui/rlImGui.h"
#include "external/imgui/imgui.h"
#include "Context.hpp"
#include "Scene.hpp"
#include "Noise.hpp"

class StartMenu : public Engine::bStateTemplate {
private:
    std::string projectName;
    char nameBuffer[25];
    bool showEmptyNameWarning;
    std::shared_ptr<Context> m_context;

    char seedBuffer[25];
    Noise noise;
public:
    explicit StartMenu(std::shared_ptr<Context>& context);
    ~StartMenu() override = default;

    void init() override;
    void process() override;
    void draw() override;
    void clean() override;
};

#endif // STARTMENU_HPP
