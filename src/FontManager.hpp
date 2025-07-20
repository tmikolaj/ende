#ifndef FONTMANAGER_HPP
#define FONTMANAGER_HPP

#include "rlImGui.h"
#include "imgui.h"

namespace Engine {

class FontManager {
private:
    ImFont* xs;
    ImFont* sm;
    ImFont* md;
    ImFont* lg;
    ImFont* xl;
    ImFont* xxl;
    ImFont* title;
public:
    FontManager() = default;
    ~FontManager() = default;

    void init();

    void setXS();
    void setSM();
    void setMD();
    void setLG();
    void setXL();
    void setXXL();
    void setTITLE();

    ImFont* getXS();
    ImFont* getSM();
    ImFont* getMD();
    ImFont* getLG();
    ImFont* getXL();
    ImFont* getXXL();
    ImFont* getTITLE();
};

}

#endif //FONTMANAGER_HPP
