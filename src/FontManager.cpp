#include "FontManager.hpp"

void Engine::FontManager::init() {
    ImGuiIO& io = ImGui::GetIO();
    xs = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 12.0f);
    sm = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 14.0f);
    md = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 18.0f);
    lg = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 22.0f);
    xl = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 28.0f);
    xxl = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 32.0f);
    title = io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-VariableFont_wdth,wght.ttf", 48.0f);
    io.FontDefault = md;
}

void Engine::FontManager::setXS() {
    ImGui::PushFont(xs);
}

void Engine::FontManager::setSM() {
    ImGui::PushFont(sm);
}

void Engine::FontManager::setMD() {
    ImGui::PushFont(md);
}

void Engine::FontManager::setLG() {
    ImGui::PushFont(lg);
}

void Engine::FontManager::setXL() {
    ImGui::PushFont(xl);
}

void Engine::FontManager::setXXL() {
    ImGui::PushFont(xxl);
}

void Engine::FontManager::setTITLE() {
    ImGui::PushFont(title);
}
