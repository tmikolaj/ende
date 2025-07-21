#include "StateManager.hpp"
#include "States/Scene.hpp"
#include "States/StartMenu.hpp"
#include "States/EntityPaint.hpp"

Engine::StateManager::StateManager() :
m_add(false),
m_remove(false),
m_replace(false),
windowStatus(0),
latency(0),
m_changePending(false) {

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
    if (latency > 0) {
        latency--;
        return;
    }
    m_changePending = false;

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

void Engine::StateManager::requestStateChange(int _stateIndex, bool replace, unsigned short changeLatency) {
    m_changePending = true;
    latency = changeLatency;

    if (_stateIndex == STARTMENU) newState = std::move(std::make_unique<StartMenu>());
    else if (_stateIndex == SCENE) newState = std::move(std::make_unique<Scene>());
    else if (_stateIndex == ENTITYPAINT) newState = std::move(std::make_unique<EntityPaint>());

    m_replace = replace;
    m_add = true;
}

bool Engine::StateManager::isChangePending() {
    return m_changePending;
}

void Engine::StateManager::setWindowState(int status) {
    windowStatus = status;
}

int Engine::StateManager::getWindowState() {
    return windowStatus;
}