#include "StateManager.hpp"
#include "States/Scene.hpp"
#include "States/StartMenu.hpp"

Engine::StateManager::StateManager() :
m_add(false),
m_remove(false),
m_replace(false),
windowStatus(0) {

}

void Engine::StateManager::setContext(std::weak_ptr<Context> ctx) {
    context = ctx;
}

void Engine::StateManager::add(std::unique_ptr<bStateTemplate> toAdd, bool replace) {
    m_add = true;
    newState = std::move(toAdd);

    m_replace = replace;
}

void Engine::StateManager::popCurrent() {
    m_remove = true;
}

void Engine::StateManager::processState() {
    auto ctx = context.lock();

    if (m_remove && (!stateStack.empty())) {
        getCurrentState()->clean(ctx);
        stateStack.pop();
        m_remove = false;
    }

    if (m_add) {
        if (m_replace) {
            stateStack.top()->clean(ctx);
            stateStack.pop();
            m_replace = false;
        }

        stateStack.push(std::move(newState));
        stateStack.top()->init(ctx);
        m_add = false;
    }
}

std::unique_ptr<bStateTemplate> &Engine::StateManager::getCurrentState() {
    return stateStack.top();
}

void Engine::StateManager::requestStateChange(int stateIndex, bool replace) {
    if (stateIndex == STARTMENU) newState = std::move(std::make_unique<StartMenu>());
    else if (stateIndex == SCENE) newState = std::move(std::make_unique<Scene>());

    m_replace = replace;
    m_add = true;
}

void Engine::StateManager::setWindowState(int status) {
    windowStatus = status;
}

int Engine::StateManager::getWindowState() {
    return windowStatus;
}