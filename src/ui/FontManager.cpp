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

void Engine::FontManager::setXS() const {
    ImGui::PushFont(xs);
}

void Engine::FontManager::setSM() const {
    ImGui::PushFont(sm);
}

void Engine::FontManager::setMD() const {
    ImGui::PushFont(md);
}

void Engine::FontManager::setLG() const {
    ImGui::PushFont(lg);
}

void Engine::FontManager::setXL() const {
    ImGui::PushFont(xl);
}

void Engine::FontManager::setXXL() const {
    ImGui::PushFont(xxl);
}

void Engine::FontManager::setTITLE() const {
    ImGui::PushFont(title);
}

ImFont* Engine::FontManager::getXS() const {
    return xs;
}

ImFont* Engine::FontManager::getSM() const {
    return sm;
}

ImFont* Engine::FontManager::getMD() const {
    return md;
}

ImFont* Engine::FontManager::getLG() const {
    return lg;
}

ImFont* Engine::FontManager::getXL() const {
    return xl;
}

ImFont* Engine::FontManager::getXXL() const {
    return xxl;
}

ImFont* Engine::FontManager::getTITLE() const {
    return title;
}
