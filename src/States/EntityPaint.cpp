#include <rlImGui.h>
#include "../Context.hpp"
#include "EntityPaint.hpp"

#include <iostream>

void EntityPaint::init(std::shared_ptr<Context> &m_context) {
    currentShader = 2;
}

void EntityPaint::process(std::shared_ptr<Context> &m_context) {

}

void EntityPaint::draw(std::shared_ptr<Context> &m_context) {
    BeginDrawing();
    ClearBackground(Color{30, 30, 30, 255});

    rlImGuiBegin();

    int code = m_context->ui->DrawMainMenuBar(m_context, currentShader);
    if (currentShader != 2) currentShader = 2;
    if (code != -1) {
        ImGui::OpenPopup("Warning");
    }
    if (ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar)) {
        m_context->fontMgr.setLG();
        ImGui::Text("Warning");
        ImGui::PopFont();

        ImGui::Dummy(ImVec2(0, 5));
        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please switch back to scene in order to do that!");
        ImGui::Dummy(ImVec2(0, 5));

        ImGui::Dummy(ImVec2(280, 0));
        ImGui::SameLine();
        if (ImGui::Button("OK")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    m_context->ui->DrawStateBar(m_context, currentShader, ENTITYPAINT);

    rlImGuiEnd();

    EndDrawing();
}

void EntityPaint::clean(std::shared_ptr<Context> &m_context) {

}