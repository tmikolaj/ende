#ifndef STYLEMANAGER_HPP
#define STYLEMANAGER_HPP

#include "imgui.h"

enum style {
    ENDE_MONOCHROME,
    ENDE_PURPLE,
    ENDE_BLUE,
    ENDE_WARM
};

class StyleManager {
private:
    static inline ImVec4 scaleColor(const ImVec4& c, float s);
    static inline ImVec4 blendColor(const ImVec4& a, const ImVec4& b, float t);
public:
    StyleManager() = default;
    ~StyleManager() = default;

    static void initEndeStyle(int endestyle);
};

#endif //STYLEMANAGER_HPP
