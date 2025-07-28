#ifndef UIMANAGER_HPP
#define UIMANAGER_HPP

#include <string>

#include "../../external/imgui/imgui.h"

class UIManager {
private:
    bool elementCalled;
public:
    UIManager();
    ~UIManager() = default;

    bool FloatInput(const std::string& title, float& var, bool includeSlider, float slidermin = 0.0f, float slidermax = 0.0f, float width = 200.0f, float inputWidth = 0, bool setDummy = true);
    bool FloatSlider(const std::string& title, float& var, float slidermin, float slidermax, float width = 300.0f, bool setDummy = true);
    bool IntInput(const std::string& title, int& var, bool includeSlider, int slidermin = 0.0f, int slidermax = 0.0f, float width = 200.0f, float inputWidth = 0, bool setDummy = true);
    bool IntSlider(const std::string& title, int& var, int slidermin, int slidermax, float width = 300.0f, bool setDummy = true);
    void Section(const std::string& sectionName = "", ImFont* font = nullptr, float spacing = 5.0f);
    void SetItemTooltip(const std::string& text, float& timer, float hoverDelay = 1.0f, ImFont* font = nullptr);
};

#endif //UIMANAGER_HPP
