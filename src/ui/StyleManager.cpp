#include "StyleManager.hpp"

#include <stdexcept>

ImVec4 StyleManager::scaleColor(const ImVec4 &c, float s) {
    return ImVec4(c.x * s, c.y * s, c.z * s, c.w * s);
}

ImVec4 StyleManager::blendColor(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t
    );
}

void StyleManager::initEndeStyle(int endestyle) {
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // === Global metrics ===
    style.WindowPadding        = ImVec2(12, 8);
    style.FramePadding         = ImVec2(8, 4);
    style.ItemSpacing          = ImVec2(10, 6);
    style.ItemInnerSpacing     = ImVec2(6, 4);
    style.ScrollbarSize        = 14;
    style.GrabMinSize          = 12;
    style.WindowRounding       = 0.0f;   // sharp main windows
    style.FrameRounding        = 4.0f;   // slight rounding for frames
    style.PopupRounding        = 8.0f;   // popups float like cards
    style.TabRounding          = 6.0f;   // segmented tabs
    style.ScrollbarRounding    = 4.0f;
    style.GrabRounding         = 4.0f;
    style.WindowBorderSize     = 1.0f;
    style.ChildBorderSize      = 1.0f;
    style.PopupBorderSize      = 1.0f;
    style.TabBorderSize        = 1.0f;

    // === Base Palette ===
    ImVec4 bgMain;
    ImVec4 bgPanel;
    ImVec4 txtMain;
    ImVec4 txtDim;
    ImVec4 neonA;
    ImVec4 neonB;
    ImVec4 divider;

    if (endestyle == ENDE_PURPLE) {
        bgMain   = ImColor(20, 10, 30);
        bgPanel  = ImColor(30, 15, 50);
        txtMain  = ImColor(240, 220, 255);
        txtDim   = ImColor(160, 130, 190);
        neonA    = ImColor(255, 0, 120);
        neonB    = ImColor(180, 0, 255);
        divider  = ImColor(60, 20, 80);

    } else if (endestyle == ENDE_MONOCHROME) {
        bgMain   = ImColor(18, 18, 18);
        bgPanel  = ImColor(28, 28, 28);
        txtMain  = ImColor(230, 230, 230);
        txtDim   = ImColor(140, 140, 140);
        neonA    = ImColor(100, 100, 100);
        neonB    = ImColor(160, 160, 160);
        divider  = ImColor(50, 50, 50);

    } else if (endestyle == ENDE_BLUE) {
        bgMain   = ImColor(15, 20, 30);
        bgPanel  = ImColor(25, 30, 45);
        txtMain  = ImColor(220, 230, 250);
        txtDim   = ImColor(130, 150, 180);
        neonA    = ImColor(0, 120, 255);
        neonB    = ImColor(0, 180, 255);
        divider  = ImColor(40, 50, 70);

    } else if (endestyle == ENDE_WARM) {
        bgMain   = ImColor(30, 15, 10);
        bgPanel  = ImColor(50, 25, 15);
        txtMain  = ImColor(255, 240, 220);
        txtDim   = ImColor(180, 150, 120);
        neonA    = ImColor(255, 100, 0);
        neonB    = ImColor(255, 180, 80);
        divider  = ImColor(70, 40, 30);

    } else {
        throw std::runtime_error("StyleManager::initEndeStyle: Unknown ende style");
    }

    // === Backgrounds ===
    colors[ImGuiCol_WindowBg] = bgMain;
    colors[ImGuiCol_ChildBg] = bgPanel;
    colors[ImGuiCol_PopupBg] = scaleColor(bgPanel, 0.9f);
    colors[ImGuiCol_Border] = divider;

    // === Text ===
    colors[ImGuiCol_Text] = txtMain;
    colors[ImGuiCol_TextDisabled] = txtDim;

    // === Frames ===
    colors[ImGuiCol_FrameBg] = scaleColor(neonA, 0.3f);
    colors[ImGuiCol_FrameBgHovered] = scaleColor(bgPanel, 0.1f);
    colors[ImGuiCol_FrameBgActive] = scaleColor(bgPanel, 0.15f);

    // === Buttons ===
    colors[ImGuiCol_Button] = bgPanel;
    colors[ImGuiCol_ButtonHovered] = scaleColor(neonB, 0.4f);
    colors[ImGuiCol_ButtonActive] = scaleColor(neonA, 0.7f);

    // === Headers ===
    colors[ImGuiCol_Header] = scaleColor(neonB, 0.4f);
    colors[ImGuiCol_HeaderHovered] = scaleColor(neonB, 0.6f);
    colors[ImGuiCol_HeaderActive] = scaleColor(neonA, 0.7f);

    // === Tabs ===
    colors[ImGuiCol_Tab] = scaleColor(bgPanel, 0.4f);
    colors[ImGuiCol_TabHovered] = scaleColor(neonB, 0.3f);
    colors[ImGuiCol_TabActive] = scaleColor(neonA, 0.5f);
    colors[ImGuiCol_TabUnfocused] = scaleColor(bgPanel, 0.25f);
    colors[ImGuiCol_TabUnfocusedActive] = scaleColor(bgPanel, 0.35f);

    // === Title Bar ===
    colors[ImGuiCol_TitleBg] = bgPanel;
    colors[ImGuiCol_TitleBgActive] = bgPanel;
    colors[ImGuiCol_TitleBgCollapsed] = scaleColor(bgPanel, 0.5f);

    // === Menus ===
    colors[ImGuiCol_MenuBarBg] = bgPanel;

    // === Scrollbars ===
    colors[ImGuiCol_ScrollbarBg] = bgPanel;
    colors[ImGuiCol_ScrollbarGrab] = scaleColor(bgPanel, 0.06f);
    colors[ImGuiCol_ScrollbarGrabHovered] = scaleColor(neonB, 0.3f);
    colors[ImGuiCol_ScrollbarGrabActive] = scaleColor(neonB, 0.5f);

    // === Inputs / Sliders ===
    colors[ImGuiCol_CheckMark] = neonB;
    colors[ImGuiCol_SliderGrab] = scaleColor(neonB, 0.8f);
    colors[ImGuiCol_SliderGrabActive] = neonA;

    // === Plots ===
    colors[ImGuiCol_PlotLines] = scaleColor(neonB, 0.6f);
    colors[ImGuiCol_PlotLinesHovered] = scaleColor(neonA, 0.8f);
    colors[ImGuiCol_PlotHistogram] = scaleColor(neonB, 0.6f);
    colors[ImGuiCol_PlotHistogramHovered] = scaleColor(neonA, 0.8f);

    // === Text Input / Combo ===
    colors[ImGuiCol_TextSelectedBg] = scaleColor(neonB, 0.3f);

    // === Resize Grips ===
    colors[ImGuiCol_ResizeGrip] = bgPanel;
    colors[ImGuiCol_ResizeGripHovered] = scaleColor(neonB, 0.4f);
    colors[ImGuiCol_ResizeGripActive] = scaleColor(neonA, 0.6f);

    // === Separators ===
    colors[ImGuiCol_Separator] = divider;
    colors[ImGuiCol_SeparatorHovered] = scaleColor(neonB, 0.3f);
    colors[ImGuiCol_SeparatorActive] = scaleColor(neonA, 0.5f);

    // === Modal Background ===
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0, 0, 0, 0.6f);
}