#ifndef STARTMENU_HPP
#define STARTMENU_HPP

#include <string>
#include "bStateTemplate.hpp"
#include "external/rlImGui/rlImGui.h"
#include "external/imgui/imgui.h"
#include "../Context.hpp"
#include "Scene.hpp"
#include "../Noise.hpp"

class StartMenu : public bStateTemplate {
private:
    std::string projectName;
    char nameBuffer[25];
    bool showEmptyNameWarning;

    char seedBuffer[25];
    Noise noise;
public:
    StartMenu();
    ~StartMenu() override = default;

    void init(std::shared_ptr<Context>& m_context) override;
    void process(std::shared_ptr<Context>& m_context) override;
    void draw(std::shared_ptr<Context>& m_context) override;
    void clean(std::shared_ptr<Context>& m_context) override;
};

#endif // STARTMENU_HPP
