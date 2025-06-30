#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include <stack>
#include <memory>
#include "bStateTemplate.hpp"

namespace Engine {

class StateManager {
private:
    std::stack<std::unique_ptr<bStateTemplate>> stateStack;
    std::unique_ptr<bStateTemplate> newState;

    bool m_add;
    bool m_replace;
    bool m_remove;

    int windowStatus;
public:
    StateManager();
    ~StateManager() = default;

    void add(std::unique_ptr<bStateTemplate> state, bool replace = false);
    void popCurrent();
    void processState();
    std::unique_ptr<bStateTemplate>& getCurrentState();
    void setWindowState(int status);
    int getWindowState();
};

}

#endif //STATEMANAGER_HPP
