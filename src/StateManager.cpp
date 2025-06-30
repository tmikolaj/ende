#include "StateManager.hpp"

Engine::StateManager::StateManager() :
m_add(false),
m_remove(false),
m_replace(false),
windowStatus(0) {

}
void Engine::StateManager::add(std::unique_ptr<Engine::bStateTemplate> toAdd, bool replace) {
    m_add = true;
    newState = std::move(toAdd);

    m_replace = replace;
}
void Engine::StateManager::popCurrent() {
    m_remove = true;
}
void Engine::StateManager::processState() {

    if (m_remove && (!stateStack.empty())) {
        getCurrentState()->clean();
        stateStack.pop();
        m_remove = false;
    }

    if (m_add) {
        if (m_replace) {
            stateStack.top()->clean();
            stateStack.pop();
            m_replace = false;
        }

        stateStack.push(std::move(newState));
        stateStack.top()->init();
        m_add = false;
    }
}
std::unique_ptr<Engine::bStateTemplate> &Engine::StateManager::getCurrentState() {
    return stateStack.top();
}

void Engine::StateManager::setWindowState(int status) {
    windowStatus = status;
}

int Engine::StateManager::getWindowState() {
    return windowStatus;
}