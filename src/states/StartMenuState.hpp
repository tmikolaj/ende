#ifndef STARTMENUSTATE_HPP
#define STARTMENUSTATE_HPP

#include <string>

#include "BaseState.hpp"

#include "external/rlImGui/rlImGui.h"
#include "external/imgui/imgui.h"

#include "../core/Context.hpp"

#include "../math/Noise.hpp"

class StartMenuState : public BaseState {
private:
    std::string projectName;
    char nameBuffer[25];
    bool showEmptyNameWarning;

    Texture endelogo;

    char seedBuffer[25];
    Noise noise;

    Texture2D endepurpleTex;
    Texture2D endeblueTex;
    Texture2D endemonochromeTex;
    Texture2D endewarmTex;
public:
    StartMenuState();
    ~StartMenuState() override = default;

    void init(std::shared_ptr<Context>& m_context) override;
    void process(std::shared_ptr<Context>& m_context) override;
    void draw(std::shared_ptr<Context>& m_context) override;
    void clean(std::shared_ptr<Context>& m_context) override;
};

#endif // STARTMENUSTATE_HPP
