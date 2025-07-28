#ifndef SIMULATIONSTATE_HPP
#define SIMULATIONSTATE_HPP

#include <imgui.h>

#include "BaseState.hpp"

#include "raylib.h"

#include "../physics/ThermalErosion.hpp"

class SimulationState : public BaseState {
private:
    Shader* shader;
    ImVec4 wireframeColor;
    ThermalErosion thermalErosion;
    bool dontShowWarning;
public:
    SimulationState() = default;
    ~SimulationState() override = default;

    void init(std::shared_ptr<Context>& p_context) override;
    void process(std::shared_ptr<Context>& p_context) override;
    void draw(std::shared_ptr<Context>& p_context) override;
    void clean(std::shared_ptr<Context>& p_context) override;
};

#endif //SIMULATIONSTATE_HPP
