#ifndef CONTEXT_HPP
#define CONTEXT_HPP

#include "StateManager.hpp"
#include "Entities/Entity.hpp"
#include "lLight.hpp"
#include "FontManager.hpp"
#include "AppUI.hpp"
#include "UIManager.hpp"
#include "ShaderManager.hpp"

enum WindowState {
    NONE = 0,
    RESTART = 1,
    EXIT = 2
};

struct Context {
    // TODO:
    std::unique_ptr<Engine::StateManager> states;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<lLight>> llights;
    Engine::FontManager fontMgr;
    std::unique_ptr<AppUI> ui;
    std::unique_ptr<Camera3D> camera;
    std::unique_ptr<UIManager> uiManager;
    std::unique_ptr<Engine::ShaderManager> shaders;

    Context() :
    states(std::make_unique<Engine::StateManager>()),
    ui(std::make_unique<AppUI>()),
    camera(std::make_unique<Camera3D>()),
    uiManager(std::make_unique<UIManager>()),
    shaders(std::make_unique<Engine::ShaderManager>()) {

    }
};

#endif // CONTEXT_HPP