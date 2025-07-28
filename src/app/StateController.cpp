#include "StateController.hpp"

#include "../states/SceneEditorState.hpp"
#include "../states/StartMenuState.hpp"
#include "../states/EntityPaintState.hpp"
#include "../states/SimulationState.hpp"

#include "../core/Context.hpp"

Engine::StateController::StateController() :
m_add(false),
m_remove(false),
m_replace(false),
windowStatus(0),
latency(0),
m_changePending(false),
m_stateIndex(STARTMENU) {

}

void Engine::StateController::setContext(std::weak_ptr<Context> ctx) {
    context = ctx;
}

void Engine::StateController::add(std::unique_ptr<BaseState> toAdd, bool replace) {
    m_add = true;
    newState = std::move(toAdd);

    m_replace = replace;
}

void Engine::StateController::popCurrent() {
    m_remove = true;
}

void Engine::StateController::processState() {
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

std::unique_ptr<BaseState>& Engine::StateController::getCurrentState() {
    return stateStack.top();
}

void Engine::StateController::requestStateChange(int _stateIndex, bool replace, unsigned short changeLatency) {
    m_changePending = true;
    latency = changeLatency;

    if (_stateIndex == STARTMENU) {
        newState = std::move(std::make_unique<StartMenuState>());
        m_stateIndex = STARTMENU;
    }
    else if (_stateIndex == SCENE) {
        newState = std::move(std::make_unique<SceneEditorState>());
        m_stateIndex = SCENE;
    }
    else if (_stateIndex == ENTITYPAINT) {
        newState = std::move(std::make_unique<EntityPaintState>());
        m_stateIndex = ENTITYPAINT;
    }
    else if (_stateIndex == SIMULATION) {
        newState = std::move(std::make_unique<SimulationState>());
        m_stateIndex = SIMULATION;
    }

    m_replace = replace;
    m_add = true;
}

bool Engine::StateController::isChangePending() {
    return m_changePending;
}

int Engine::StateController::getstateIndex() const {
    return m_stateIndex;
}

void Engine::StateController::setWindowState(int status) {
    windowStatus = status;
}

int Engine::StateController::getWindowState() {
    return windowStatus;
}

void Engine::StateController::clean() {
    auto ctx = context.lock();

    // First clean already called in Application.cpp
    stateStack.pop();

    while (!stateStack.empty()) {
        stateStack.top()->clean(ctx);
        stateStack.pop();
    }
}