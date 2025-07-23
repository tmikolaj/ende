#ifndef STATECONTROLLER_HPP
#define STATECONTROLLER_HPP

#include <stack>
#include <memory>
#include "../states/BaseState.hpp"

struct Context;

enum stateIndex {
    STARTMENU = 0,
    SCENE = 1,
    ENTITYPAINT = 2,
    SIMULATION = 3,
};

namespace Engine {

class StateController {
private:
    std::stack<std::unique_ptr<BaseState>> stateStack;
    std::unique_ptr<BaseState> newState;

    std::weak_ptr<Context> context;

    bool m_add;
    bool m_replace;
    bool m_remove;
    bool m_changePending;

    int windowStatus;
    // Controls how many times all the state function will be executed before processState
    // which will either remove, add or do nothing
    unsigned short latency;
public:
    StateController();
    ~StateController() = default;

    void setContext(std::weak_ptr<Context> ctx);

    void add(std::unique_ptr<BaseState> state, bool replace = false);
    void popCurrent();
    void processState();
    std::unique_ptr<BaseState>& getCurrentState();
    void requestStateChange(int stateIndex, bool replace = false, unsigned short changeLatency = 0);
    bool isChangePending();

    void setWindowState(int status);
    int getWindowState();
};

}

#endif // STATECONTROLLER_HPP
