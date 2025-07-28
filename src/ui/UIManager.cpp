#include "UIManager.hpp"

UIManager::UIManager() :
elementCalled(false) {

}

bool UIManager::FloatInput(const std::string& title, float& var, bool includeSlider, float slidermin, float slidermax, float width, float inputWidth, bool setDummy) {
    if (elementCalled) ImGui::Dummy(ImVec2(0, 2.5f));

    bool ret = false;
    if (inputWidth == 0.0f) {
        inputWidth = width / 2.0f;
    }
    ImGui::PushItemWidth(width);
    ImGui::Text(title.c_str());

    if (includeSlider) {
        std::string labelSlider = "##" + title + "Slider";
        ret |= ImGui::SliderFloat(labelSlider.c_str(), &var, slidermin, slidermax);
        ImGui::SameLine();
    }

    std::string labelInput = "##" + title + "Input";
    ImGui::PushItemWidth(inputWidth);
    ret |= ImGui::InputFloat(labelInput.c_str(), &var);
    ImGui::PopItemWidth();

    if (includeSlider) {
        if (var < slidermin) slidermin = var;
        if (var > slidermax) slidermax = var;
    }
    ImGui::PopItemWidth();

    if (setDummy) elementCalled = true;

    return ret;
}

bool UIManager::FloatSlider(const std::string& title, float& var, float slidermin, float slidermax, float width, bool setDummy) {
    if (elementCalled) ImGui::Dummy(ImVec2(0, 2.5f));

    bool ret = false;

    ImGui::PushItemWidth(width);

    ImGui::Text(title.c_str());
    std::string label = "##" + title + "Slider";
    ret = ImGui::SliderFloat(label.c_str(), &var, slidermin, slidermax);

    ImGui::PopItemWidth();

    if (setDummy) elementCalled = true;

    return ret;
}

bool UIManager::IntInput(const std::string &title, int &var, bool includeSlider, int slidermin, int slidermax, float width, float inputWidth, bool setDummy) {
    if (elementCalled) ImGui::Dummy(ImVec2(0, 2.5f));

    bool ret = false;
    if (inputWidth == 0.0f) {
        inputWidth = width / 2.0f;
    }
    ImGui::PushItemWidth(width);
    ImGui::Text(title.c_str());

    if (includeSlider) {
        std::string labelSlider = "##" + title + "Slider";
        ret |= ImGui::SliderInt(labelSlider.c_str(), &var, slidermin, slidermax);
        ImGui::SameLine();
    }

    std::string labelInput = "##" + title + "Input";
    ImGui::PushItemWidth(inputWidth);
    ret |= ImGui::InputInt(labelInput.c_str(), &var);
    ImGui::PopItemWidth();

    if (includeSlider) {
        if (var < slidermin) slidermin = var;
        if (var > slidermax) slidermax = var;
    }
    ImGui::PopItemWidth();

    if (setDummy) elementCalled = true;

    return ret;
}

bool UIManager::IntSlider(const std::string &title, int &var, int slidermin, int slidermax, float width, bool setDummy) {
    if (elementCalled) ImGui::Dummy(ImVec2(0, 2.5f));

    bool ret = false;

    ImGui::PushItemWidth(width);

    ImGui::Text(title.c_str());
    std::string label = "##" + title + "Slider";
    ret = ImGui::SliderInt(label.c_str(), &var, slidermin, slidermax);

    ImGui::PopItemWidth();

    if (setDummy) elementCalled = true;

    return ret;
}

void UIManager::Section(const std::string& sectionName, ImFont* font, float spacing) {
    ImGui::Dummy(ImVec2(0, spacing));
    if (!sectionName.empty() && font != nullptr) {
        ImGui::PushFont(font);
        ImGui::Text(sectionName.c_str());
        ImGui::PopFont();
    }
    ImGui::Dummy(ImVec2(0, spacing));
}

void UIManager::SetItemTooltip(const std::string& text, float& timer, float hoverDelay, ImFont* font) {
    if (font != nullptr) {
        ImGui::PushFont(font);
    }

    if (ImGui::IsItemHovered() && !ImGui::IsItemActive()) {
        if (timer == 0.0f) timer = ImGui::GetTime();

        if (ImGui::GetTime() - timer > hoverDelay) {
            ImGui::BeginTooltip();
            ImGui::Text(text.c_str());
            ImGui::EndTooltip();
        }
    } else {
        timer = 0.0f;
    }

    if (font != nullptr) ImGui::PopFont();
}